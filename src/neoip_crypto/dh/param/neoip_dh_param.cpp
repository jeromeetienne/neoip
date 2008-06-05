/*! \file
    \brief Definition of the class \ref dh_param_t

\par Brief description
This modules handle the diffie hellman parameters

*/

/* system include */
/* local include */
#include "neoip_dh_param.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	dh_param_t::zeroing()	throw()
{
	gcry_group	= NULL;
	gcry_generator	= NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	dh_param_t::nullify()		throw()
{ 
	if( is_null() )	return;
	if( gcry_group ){
		gcry_mpi_release( gcry_group );
		gcry_group = NULL;
	}
	if( gcry_generator ){
		gcry_mpi_release( gcry_generator );
		gcry_generator = NULL;
	}
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	dh_param_t::copy(const dh_param_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;

	// init the group
	gcry_group	= gcry_mpi_new(0);
	gcry_group	= gcry_mpi_copy(other.gcry_group);
	// init the generator
	gcry_generator	= gcry_mpi_new(0);
	gcry_generator	= gcry_mpi_copy(other.gcry_generator);
}
	
/** \brief return true if the object is null
 */
bool	dh_param_t::is_null()		const throw()
{
	if( gcry_group == NULL )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int dh_param_t::compare( const dh_param_t & other )  const throw()
{
	int	result;
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the group
	result = gcry_mpi_cmp(gcry_group, other.gcry_group);
	if( result )	return result;

	// compare the generator
	result = gcry_mpi_cmp(gcry_generator, other.gcry_generator);
	if( result )	return result;
	
	// here both are considered equal
	return 0;
}


/** \brief convert the object into a string
 */
std::string dh_param_t::to_string()	const throw()
{
	char	buffer[5*1024];
	std::string	str;
	if( is_null() )	return "null";

	// dump the generator
	gcry_mpi_print(GCRYMPI_FMT_HEX, (uint8_t*)buffer, sizeof(buffer), NULL, gcry_generator);
	str += "generator=";
	str += std::string(buffer);
	// dump the group
	gcry_mpi_print(GCRYMPI_FMT_HEX, (uint8_t*)buffer, sizeof(buffer), NULL, gcry_group);
	str += " group=";
	str += std::string(buffer);
	return str;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     constructor with value
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with value
 */
dh_param_t::dh_param_t(const std::string & group_str, const std::string & generator_str)	throw()
{
	int	err;
	// initial init
	zeroing();	
	// Convert the group_str into gcrypt format
	gcry_group	= gcry_mpi_new(0);
	err = gcry_mpi_scan( &gcry_group, GCRYMPI_FMT_HEX, (uint8_t *)group_str.c_str(), 0, NULL );
	if( err < 0 ){
		KLOG_ERR( "Cant scan the dh group due to "+std::string(gcry_strerror(err)));
		gcry_mpi_release( gcry_group );
		return;
	}

	// Convert the generator_str into gcrypt format
	gcry_generator	= gcry_mpi_new(0);
	err = gcry_mpi_scan( &gcry_generator, GCRYMPI_FMT_HEX, (uint8_t *)generator_str.c_str(), 0, NULL);
	if( err < 0 ){
		std::string	reason = "Cant scan the dh generator due to "+std::string(gcry_strerror(err));
		gcry_mpi_release( gcry_group );
		gcry_mpi_release( gcry_generator );
		KLOG_ERR( reason );
		return;
	}
}

/** \brief Return the key length in bit
 */
int	dh_param_t::get_key_len()			const throw()
{
	return gcry_mpi_get_nbits( gcry_group );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   predefined dh_param_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build a dh_param_t matching ike modp group 1
 * 
 * - TODO add more group and put references in comments
 */
dh_param_t	dh_param_t::build_ike_mopd_grp1()	throw()
{
	return dh_param_t(	"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
				"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
				"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
				"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
				"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381"
				"FFFFFFFFFFFFFFFF", "2");
}

NEOIP_NAMESPACE_END



