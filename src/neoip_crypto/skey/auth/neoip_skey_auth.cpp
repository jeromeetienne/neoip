/*! \file
    \brief Definition of the class \ref skey_auth_t

\par Brief description
\ref skey_auth_t provides computation and check using symmetric
algorithms (e.g. MD5, SHA1 etc...). The mode and the output length are tunable.
- e.g. md5/hmac/12	for a hmac based on md5 with an output of 12-byte
- e.g. md5/nokey/16	for a simple hash md5 with an output of 16-byte

*/

/* system include */
/* local include */
#include "neoip_skey_auth.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	skey_auth_t::zeroing()	throw()
{
	auth_type	= skey_auth_type_t();	
	// nothing about the gcrypt lib context because nothing in gcrypt seems to provide this function
	// - care is taken not to rely on its value
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	skey_auth_t::nullify()		throw()
{ 
	if( is_null() )	return;
	auth_type	= skey_auth_type_t();
	// close the gcrypt mac context
	deinit_gcry_hd();
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	skey_auth_t::copy(const skey_auth_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;

	bool	failed	= init_gcry_hd(other.auth_type);
	if( failed )	return;
	auth_type	= other.auth_type;	
}
	
/** \brief return true if the object is null
 */
bool	skey_auth_t::is_null()		const throw()
{
	if( auth_type.is_null() )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int skey_auth_t::compare( const skey_auth_t & other )  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the type
	return auth_type.compare(other.auth_type);
}


/** \brief convert the object into a string
 */
std::string skey_auth_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return auth_type.to_string();
}

/** \brief constructor based on a value
 */
skey_auth_t::skey_auth_t(const skey_auth_type_t &auth_type)			throw()
{
	zeroing();
	bool	failed	= init_gcry_hd(auth_type);
	if( failed )	return;
	this->auth_type	= auth_type;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         init/deinit gcrypt context
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the gcry_md_hd
 * 
 * - this init is assumed to be done everytime it is done for auth_type
 */
bool skey_auth_t::init_gcry_hd(const skey_auth_type_t &auth_type)		throw()
{
	std::string	algo_str = auth_type.get_algo().to_string();
	// get the algoid from the algo_str
	int	gcry_md_algo = gcry_md_map_name( algo_str.c_str() );
	// if the algo_str isnt supported, throw an exception
	if( gcry_md_algo == 0 ){
		KLOG_ERR("Can't map " << algo_str << " in gcrypt md_map_name");
		return true;
	}
	// compute the the proper flag
	int	gcry_md_flag = 0;
	if( auth_type.get_mode() == skey_auth_mode_t::HMAC )
		gcry_md_flag |= GCRY_MD_FLAG_HMAC | GCRY_MD_FLAG_SECURE;
	// open gcrypt mac context
	if( gcry_md_open( &gcry_md_hd, gcry_md_algo, gcry_md_flag ) ){
		KLOG_ERR("Can't open " << auth_type << " in gcrypt md_open");
		return true;
	}
	// return false if no error occured
	return false;
}

/** \brief deinit the gcry_md_hd
 * 
 * - this deinit is assumed to be done everytime it is done for auth_type
 */
void skey_auth_t::deinit_gcry_hd()						throw()
{
	// close the gcrypt mac context
	gcry_md_close( gcry_md_hd );	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         auth computation
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the key
 * 
 * - if the mode is NOKEY, key_ptr and key_len are ignored and the hash is reseted
 */
bool skey_auth_t::init_key( const void *key_ptr, size_t key_len )		throw()
{
	// if the mode is NOKEY, the hash context is reseted
	if( auth_type.get_mode() == skey_auth_mode_t::NOKEY ){
		// sanity check
		DBG_ASSERT( key_ptr == NULL && key_len == 0 );
		// reset the key
		gcry_md_reset( gcry_md_hd );
		return false;
	}
	// sanity check
	DBG_ASSERT( auth_type.get_mode() == skey_auth_mode_t::HMAC );
	DBG_ASSERT( key_ptr && key_len );
	// set the new key
	if( gcry_md_setkey( gcry_md_hd, key_ptr, key_len ) )
		return true;
	return false;
}

/** \brief update the skey_auth_t with new data
 */
void skey_auth_t::update( const void *buf_ptr, size_t buf_len )		throw()
{
	gcry_md_write( gcry_md_hd, buf_ptr, buf_len );
}

/** \brief Provide a pointer on the result of the auth computation
 * 
 * - the length of the result can be found in ::get_out_len()
 */
const void * skey_auth_t::final()						throw()
{
	// gcry_md_read does final implicitly
	return gcry_md_read(gcry_md_hd, 0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         UTILITY function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the output len of this skey_auth_t
 */
size_t skey_auth_t::get_output_len()			  		const throw()
{
	return auth_type.get_output_len();
}

/** \brief return the key len of this skey_auth_t
 */
size_t skey_auth_t::get_key_len()			  		const throw()
{
	return get_output_len();
}


NEOIP_NAMESPACE_END
