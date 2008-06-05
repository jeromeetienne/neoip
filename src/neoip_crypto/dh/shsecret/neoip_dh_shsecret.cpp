/*! \file
    \brief Definition of the class \ref dh_shsecret_t

\par Brief description
This modules handle the diffie hellman shsecreteters

*/

/* system include */
/* local include */
#include "neoip_dh_shsecret.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	dh_shsecret_t::zeroing()	throw()
{
	gcry_shsecret	= NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	dh_shsecret_t::nullify()		throw()
{ 
	if( is_null() )	return;
	gcry_shsecret	= gcry_mpi_snew(0);
	if( gcry_shsecret ){
		gcry_mpi_release( gcry_shsecret );
		gcry_shsecret	= NULL;
	}
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	dh_shsecret_t::copy(const dh_shsecret_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;
	gcry_shsecret	= gcry_mpi_snew(0);
	gcry_shsecret	= gcry_mpi_copy(other.gcry_shsecret);
}
	
/** \brief return true if the object is null
 */
bool	dh_shsecret_t::is_null()		const throw()
{
	if( gcry_shsecret == NULL )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int dh_shsecret_t::compare( const dh_shsecret_t & other )  const throw()
{
	int	result;
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the public key
	result = gcry_mpi_cmp(gcry_shsecret, other.gcry_shsecret);
	if( result )	return result;
	
	// here both are considered equal
	return 0;
}


/** \brief convert the object into a string
 */
std::string dh_shsecret_t::to_string()	const throw()
{
	char		buffer[5*1024];
	std::string	str;
	if( is_null() )	return "null";

	// dump the generator
	gcry_mpi_print(GCRYMPI_FMT_HEX, (uint8_t*)buffer, sizeof(buffer), NULL, gcry_shsecret);
	str += std::string(buffer);
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
dh_shsecret_t::dh_shsecret_t(const dh_privkey_t &privkey, const dh_pubkey_t &remote_pubkey
							, const dh_param_t &param)	throw()
{
	// zero the object
	zeroing();
	// compute and set the shared secret
	gcry_shsecret	= gcry_mpi_snew(0);
	gcry_mpi_powm( gcry_shsecret, remote_pubkey.get_gcry_pubkey(), privkey.get_gcry_privkey()
							, param.get_gcry_group() );	
}

/** \brief convert the object to a datum_t
 */
datum_t dh_shsecret_t::to_datum()				const throw()
{
	uint8_t	data_buf[5*1024];				// TODO to replace
	int	err;
	size_t	data_len = sizeof(data_buf);
	// dump the shsecret in simple unsigned integer
	err = gcry_mpi_print(GCRYMPI_FMT_USG, data_buf, data_len, &data_len, gcry_shsecret);
	DBG_ASSERT( err >= 0 );
	// serial the datum
	return datum_t(data_buf, data_len, datum_t::SECMEM);
}

NEOIP_NAMESPACE_END



