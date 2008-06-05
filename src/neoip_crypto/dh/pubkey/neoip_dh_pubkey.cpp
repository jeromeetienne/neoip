/*! \file
    \brief Definition of the class \ref dh_pubkey_t

\par Brief description
This modules handle the diffie hellman pubkeyeters

*/

/* system include */
/* local include */
#include "neoip_dh_pubkey.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	dh_pubkey_t::zeroing()	throw()
{
	gcry_pubkey	= NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	dh_pubkey_t::nullify()		throw()
{ 
	if( is_null() )	return;
	if( gcry_pubkey ){
		gcry_mpi_release( gcry_pubkey );
		gcry_pubkey	= NULL;
	}
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	dh_pubkey_t::copy(const dh_pubkey_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;

	gcry_pubkey	= gcry_mpi_new(0);
	gcry_pubkey	= gcry_mpi_copy(other.gcry_pubkey);
}
	
/** \brief return true if the object is null
 */
bool	dh_pubkey_t::is_null()		const throw()
{
	if( gcry_pubkey == NULL )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int dh_pubkey_t::compare( const dh_pubkey_t & other )  const throw()
{
	int	result;
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the public key
	result = gcry_mpi_cmp(gcry_pubkey, other.gcry_pubkey);
	if( result )	return result;
	
	// here both are considered equal
	return 0;
}


/** \brief convert the object into a string
 */
std::string dh_pubkey_t::to_string()	const throw()
{
	char		buffer[5*1024];
	std::string	str;
	if( is_null() )	return "null";

	// dump the generator
	gcry_mpi_print(GCRYMPI_FMT_HEX, (uint8_t*)buffer, sizeof(buffer), NULL, gcry_pubkey);
	str += std::string(buffer);
	return str;	
}

/** \brief convert the object to a datum_t
 */
datum_t dh_pubkey_t::to_datum()				const throw()
{
	uint8_t	data_buf[5*1024];				// TODO to replace
	int	err;
	size_t	data_len = sizeof(data_buf);
	// dump the pubkey in simple unsigned integer
	err = gcry_mpi_print(GCRYMPI_FMT_USG, data_buf, data_len, &data_len, gcry_pubkey);
	DBG_ASSERT( err >= 0 );
	// return the datum - no need for datum_t::SECMEM as it is public
	return	datum_t(data_buf, data_len);
}


/** \brief convert a datum_t into a dh_pubkey_t
 */
dh_pubkey_t dh_pubkey_t::from_datum(const datum_t &datum)			throw()
{
	dh_pubkey_t	dh_pubkey;
	int		err;
	// get the pubkey from memory
	err = gcry_mpi_scan( &dh_pubkey.gcry_pubkey, GCRYMPI_FMT_USG, datum.uint8_ptr()
							, datum.length(), NULL );
	if( err < 0 ){
		KLOG_ERR("Error importing serialized dh public key due to "
						+ std::string(gcry_strerror(err)));
		return dh_pubkey_t();
	}
	// return the justbuilt dh_pubkey_t
	return dh_pubkey;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          SERIALIZATION
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a dh_shsecret
 */
serial_t& operator << ( serial_t& serial, const dh_pubkey_t &dh_pubkey )	throw()
{
	uint8_t	data_buf[5*1024];				// TODO to replace
	int	err;
	size_t	data_len = sizeof(data_buf);
	// dump the pubkey in simple unsigned integer
	err = gcry_mpi_print(GCRYMPI_FMT_USG, data_buf, data_len, &data_len, dh_pubkey.gcry_pubkey);
	DBG_ASSERT( err >= 0 );
	// serial the datum
	serial << datum_t(data_buf, data_len);
	return serial;
}

/** \brief unserialize a dh_pubkey
 */
serial_t& operator >> ( serial_t& serial, dh_pubkey_t &dh_pubkey )		throw(serial_except_t)
{
	datum_t		datum;
	int		err;
	// get the datum
	serial >> datum;
	// get the pubkey from memory
	err = gcry_mpi_scan( &dh_pubkey.gcry_pubkey, GCRYMPI_FMT_USG, (uint8_t *)datum.get_data()
							, (size_t)datum.get_len(), NULL );
	if( err < 0 ){
		throw serial_except_t("Error importing serialized dh public key due to "
						+ std::string(gcry_strerror(err)));	
	}
	return serial;
}

NEOIP_NAMESPACE_END



