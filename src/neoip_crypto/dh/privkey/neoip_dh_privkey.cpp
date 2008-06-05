/*! \file
    \brief Definition of the class \ref dh_privkey_t

\par Brief description
This modules handle the diffie hellman privkeyeters

*/

/* system include */
/* local include */
#include "neoip_dh_privkey.hpp"
#include "neoip_random_pool.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	dh_privkey_t::zeroing()	throw()
{
	gcry_privkey	= NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	dh_privkey_t::nullify()		throw()
{ 
	if( is_null() )	return;
	
	if( gcry_privkey ){
		gcry_mpi_release( gcry_privkey );
		gcry_privkey = NULL;
	}
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	dh_privkey_t::copy(const dh_privkey_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;

	gcry_privkey	= gcry_mpi_snew(0);
	gcry_privkey	= gcry_mpi_copy(other.gcry_privkey);
	pubkey		= other.pubkey;
}
	
/** \brief return true if the object is null
 */
bool	dh_privkey_t::is_null()		const throw()
{
	if( gcry_privkey == NULL )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int dh_privkey_t::compare( const dh_privkey_t & other )  const throw()
{
	int	result;
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the private key
	result = gcry_mpi_cmp(gcry_privkey, other.gcry_privkey);
	if( result )	return result;

	// compare the public key
	result = pubkey.compare(other.pubkey);
	if( result )	return result;	
	
	// here both are considered equal
	return 0;
}


/** \brief convert the object into a string
 */
std::string dh_privkey_t::to_string()	const throw()
{
	char		buffer[5*1024];
	std::string	str;
	if( is_null() )	return "null";

	// dump the private key
	gcry_mpi_print(GCRYMPI_FMT_HEX, (uint8_t*)buffer, sizeof(buffer), NULL, gcry_privkey);
	str += "private key=";
	str += buffer;
	// dump the associated public key
	str += " public key=" + pubkey.to_string();
	return str;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               ctor with value
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor with value
 * 
 * - init the private key and compute the public key from it
 */
dh_privkey_t::dh_privkey_t(const dh_param_t &param)				throw()
{
	int	key_len_byte	= param.get_key_len() + 7 / 8;
	uint8_t	*tmp		= (uint8_t *)nipmem_alloca(key_len_byte);
	int	err, i;

	// zero the local struct
	zeroing();

	// get some randomness to init the private key
	random_pool_t::read_normal(tmp, key_len_byte);

	// init the private key
	gcry_privkey	= gcry_mpi_snew(0);
	err = gcry_mpi_scan( &gcry_privkey, GCRYMPI_FMT_USG, tmp, key_len_byte, NULL );
	if( err < 0 ){
		KLOG_ERR( "Cant scan the dh privkey due to "+std::string(gcry_strerror(err)));
		gcry_mpi_release( gcry_privkey );
		return;
	}

	// check the key is <= to the group
	// - if not, clear privkey's highest bit and loop until it does
	for(i=gcry_mpi_get_nbits(gcry_privkey);gcry_mpi_cmp(gcry_privkey, param.get_gcry_group()) > 0;i--)
		gcry_mpi_clear_bit( gcry_privkey, i );

	// compute and set the public key
	pubkey.gcry_pubkey	= gcry_mpi_new(0);
	gcry_mpi_powm(pubkey.gcry_pubkey, param.get_gcry_generator(), gcry_privkey
								, param.get_gcry_group());
}

NEOIP_NAMESPACE_END



