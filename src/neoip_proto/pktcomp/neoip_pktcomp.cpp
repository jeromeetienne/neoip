/*! \file
    \brief Definition of pktcomp_t

\par the "nocomp" period
Sometime the data contained in the packet can't be compress (e.g. if they are already compressed).
In order to save CPU, the period during which the data are not compressable are detected and
no compression is attempted during those period. The parameters are tunable via \ref pktcomp_profile_t. 

The algorithm is :
-# if a compression failed, increase the "nocomp_cur_nb_succ"
-# if the last "nocomp_max_nb_succ" compression attempts failed, enter a "nocomp" period
-# until this end of this period, no compression is attempted
   - the duration of this period depends on a timer policy (which is currently an exponantial backoff)
   - At the end of this period, "nocomp_cur_nb_succ" is zeroed and the algo loops over and over
   
This algorithm will adapt to the compressability of the data sent over the layer. 
- e.g. if data are compressable now and become uncompressable later, the algorithm will detect it.
- e.g. if data are uncompressable now and become compressable later, the algorithm will detect it.

\par TODO
- the MAX_UNCOMPRESSED_LEN_DFL seems quite close to a maximum size for the neoip datagram
  - how to handle this ? make it tunable.
  - maybe even negociable between the 2 peers as it is sent by one and received by the other
    so they have to both agree.
  - i would like to avoid have a BIG constant about the neoip dgram size
    - i could negociate the size and not compress if the sent packet is larger that it
    - well it is quite lame as the bigger is the dgram, the more it needs to be compressed
  - this issue is related to the fragmentations buffer too
  - what about using the recv_max_len_set to detect this ?
    - thus it is up to the caller to determine it
*/

/* system include */
/* local include */
#include "neoip_pktcomp.hpp"
#include "neoip_pktcomp_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref pktcomp_t constant
//! the default maximum length of uncompressed packets
const size_t	pktcomp_t::MAX_UNCOMPRESSED_LEN_DFL	= 64*1024;
//! the maximum number of successive packets failing to be compressed until triger a nocomp period
const size_t	pktcomp_t::NOCOMP_NB_SUCC_MAX_DFL	= 10;
//! the minimum delay of a nocomp period
const delay_t	pktcomp_t::NOCOMP_DELAY_MIN_DFL		= delay_t::from_sec(1);	
//! the maximum delay of a nocomp period
const delay_t	pktcomp_t::NOCOMP_DELAY_MAX_DFL		= delay_t::from_sec(10);	
// end of constants definition

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                      ctor/dtor
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
pktcomp_t::pktcomp_t()	throw()
{
}

/** \brief Destructor
 */
pktcomp_t::~pktcomp_t()	throw()
{
}

/** \brief Constructor with a value
 */
pktcomp_t::pktcomp_t(const compress_type_t &compress_type)	throw()
{
	compress_ctx		= compress_t(compress_type);

	max_uncompress_len	= MAX_UNCOMPRESSED_LEN_DFL;
	// init some nocomp variables
	nocomp_cur_nb_succ	= 0;
	nocomp_max_nb_succ	= NOCOMP_NB_SUCC_MAX_DFL;
	nocomp_timer_policy	= timer_expboff_t(NOCOMP_DELAY_MIN_DFL, NOCOMP_DELAY_MAX_DFL);
}

/** \brief set the parameters
 */
pktcomp_err_t pktcomp_t::set_from_profile(const pktcomp_profile_t &profile)	throw()
{
	// check the profile
	pktcomp_err_t	err = profile.check();
	if( err.failed() )	return err;
	// copy the values
	max_uncompress_len  = profile.max_uncompress_len();
	nocomp_max_nb_succ  = profile.nocomp_max_nb_succ();
	nocomp_timer_policy = timer_expboff_t(profile.nocomp_delay_min(), profile.nocomp_delay_max());
	// return noerror
	return pktcomp_err_t::OK;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                      nocomp period management
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief to be called when a pkt compression result in a expansion
 */
void pktcomp_t::nocomp_pkt_cant_compress()	throw()
{
	// update the number of failed compresion in a row
	nocomp_cur_nb_succ++;
	// if nocomp_cur_nb_succ reaches its max, start the nocomp period
	if( nocomp_cur_nb_succ >= nocomp_max_nb_succ )
		nocomp_timeout.start(nocomp_timer_policy.next(), this, NULL);
}

/** \brief to be called when a pkt compression result in a reduction
 */
void pktcomp_t::nocomp_pkt_compressed()	throw()
{
	// reset the number of failed compresion in a row
	nocomp_cur_nb_succ	= 0;
	// reset the timer_policy
	nocomp_timer_policy.reset();	
}

/** \brief callback called when the timeout_t expire
 */
bool	pktcomp_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// reset the number of failed compresion in a row
	nocomp_cur_nb_succ	= 0;
	// stop the timeout
	nocomp_timeout.stop();
	return true;
}

/** \brief return true if this pktcomp_t is during a nocomp period
 */
bool pktcomp_t::nocomp_in_progress()	const throw()
{
	if( nocomp_timeout.is_running() )	return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                      packet processing
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief compute the maximum size of the compressed data not to expand data
 * 
 * - aka the length of the uncompressed data minus the overhead minus 1 byte (to have
 *   a actual size reduction)
 */
size_t	pktcomp_t::cpu_max_compressed_len(size_t uncompressed_len)		throw()
{
	// compute the overhead size
	serial_t	serial;
	serial << pktcomp_pkttype_t(pktcomp_pkttype_t::COMPRESSED_PKT);
	size_t	overhead_len = serial.get_len();
	// if the uncompressed_len is smaller than the minimum size to compress, return 0
	if( uncompressed_len < overhead_len + 1 )	return 0;
	// return the maximum size of the compressed data
	return uncompressed_len - overhead_len - 1;
}

/** \brief receive packet from the upper layer
 */
pktcomp_err_t	pktcomp_t::pkt_from_upper(pkt_t &pkt)				throw()
{
	// if nocomp is in progress, do noting
	if( nocomp_in_progress() )	return pktcomp_err_t::OK;
	// compute the maximum size of the compressed data
	size_t	max_compressed_len	= cpu_max_compressed_len(pkt.get_len());
	if( max_compressed_len == 0 ){
		nocomp_pkt_cant_compress();	// warn nocomp of the expansion
		return pktcomp_err_t::WOULD_EXPAND;
	}
	// try to compress the packet
	datum_t	compressed	= compress_ctx.compress(pkt.to_datum(), max_compressed_len);
	// if the compression failed, return an error
	if( compressed.is_null() ){
		nocomp_pkt_cant_compress();	// warn nocomp of the expansion
		return pktcomp_err_t::WOULD_EXPAND;
	}
	// warn nocomp of the compression
	nocomp_pkt_compressed();
	// build the resulting packet
	pkt	= pkt_t();
	pkt << pktcomp_pkttype_t(pktcomp_pkttype_t::COMPRESSED_PKT);
	pkt.tail_add(compressed.get_data(), compressed.get_len());
	// return no error
	return pktcomp_err_t::OK;
}

/** \brief receive packet from the lower layer
 */
pktcomp_err_t	pktcomp_t::pkt_from_lower(pkt_t &pkt)	throw()
{
	// logging to debug
	KLOG_DBG("enter pkt=" << pkt);

	try {
		pktcomp_pkttype_t	pkttype;
		// read the pkttype (without consuming)
		pkt.unserial_peek( pkttype );
		// logging to debug
		KLOG_DBG("received a pkttype=" << std::hex << pkttype );
		
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case pktcomp_pkttype_t::COMPRESSED_PKT:	return recv_compressed_pkt(pkt);
		default:	break;
		}
	}catch(serial_except_t &e){
		return pktcomp_err_t(pktcomp_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}
	// return an error
	return pktcomp_err_t(pktcomp_err_t::BOGUS_PKT, "Unknown packet type");	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Handle the reception of a COMPRESSED_PKT
 */
pktcomp_err_t	pktcomp_t::recv_compressed_pkt(pkt_t &pkt)			throw(serial_except_t)
{
	pktcomp_pkttype_t	pkttype;
	datum_t			compressed_data;
	datum_t			uncompressed;

	// read the pkttype
	pkt >> pkttype;
	// sanity check - only this pkttype is allowed here
	DBG_ASSERT(pkttype == pktcomp_pkttype_t::COMPRESSED_PKT);

	// read the compressed data (aka the remaining of the packet)
	compressed_data	= pkt.to_datum();
	// try to decompress the packet
	uncompressed	= compress_ctx.uncompress(compressed_data, max_uncompress_len);
	// if the compression failed, return an error
	if( uncompressed.is_null() )	return pktcomp_err_t::CANT_DECOMPRESS;

	// build the resulting packet
	pkt = pkt_t(uncompressed);
	// return noerror
	return pktcomp_err_t::OK;
}

NEOIP_NAMESPACE_END



