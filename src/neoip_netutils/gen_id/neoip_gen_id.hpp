/*! \file
    \brief Header of the gen_id_t

\par Possible improvement
- to introduce the pkcs5 key derivation is the output of the hash is smaller than
  the id size
*/


#ifndef __NEOIP_GEN_ID_HPP__ 
#define __NEOIP_GEN_ID_HPP__ 
/* system include */
#include <iostream>
#include <iomanip>
#include <string>
/* local include */
#include "neoip_skey_auth.hpp"		// for deriving gen_id_t from string
#include "neoip_random_pool.hpp"	// for build_random()
#include "neoip_serial.hpp"		// for binary serialization
#include "neoip_xmlrpc.hpp"		// for xmlrpc serialization
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>	class gen_id_t;
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
	T operator >> (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id, int nb_bit)	throw();
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
	serial_t& operator << (serial_t& serial, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)throw();
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
	serial_t& operator >> (serial_t& serial, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)	throw(serial_except_t);
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)	throw();
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)		throw(xml_except_t);

/** \brief class definition for a generic ID
 * 
 * - here ID = relatively small bunch of byte 
 *   - typically between 64 and 512 bit
 *   - with a preference for 128-bit and 160-bit as it is the output size of md5/sha1
 * - it may be derived from a plain string via a hash function passed as parameter parameter
 * - it has been made generic and template to centralize the code of each ID
 *   - btw the template thing create large dependencies in the compilation, so be aware
 *     that modifying this module will likely produce a large recompilation :)
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
class gen_id_t {
private:
	uint8_t	buffer[NB_BYTE];		//!< the buffer containing the data

	void	ctor_from_str(const std::string &id_str)		throw();
public:
	/*************** ctor/dtor	***************************************/
	gen_id_t()				throw();
	gen_id_t(const std::string &id_str)	throw()	{ ctor_from_str(id_str);		}
	gen_id_t(const char *id_str)		throw()	{ ctor_from_str(std::string(id_str));	}
	gen_id_t(const datum_t &datum)		throw();
	
	/*************** query function	***************************************/
	static size_t	size()		throw()		{ return NB_BYTE;			}
	bool		is_null()	const throw();
	bool		is_any()	const throw()	{ return *this == this->build_any();	}
	bool		is_fully_qualified() const throw() { return !is_null() && !is_any();	}

	/*************** Convertion	***************************************/
	datum_t		to_datum(datum_flag_t flag = datum_t::FLAG_DFL)	const throw()
							{ return datum_t(buffer, sizeof(buffer), flag);	}
	std::string	to_stdstring()	const throw()	{ return std::string((char*)buffer, sizeof(buffer));	}

	/*************** Bit Operation	***************************************/
	T	operator ^ (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw();
	T	operator & (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw();
	friend T operator >> <> (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &value, int nb_bit) throw();
	size_t	bit_get_highest_set()			const throw();
	T	bit_clear_n_highest(size_t n_bit)	throw();
	/*************** Static build	***************************************/
	static T	build_random()		throw();
	static T	build_null()		throw();
	static T	build_any()		throw();

	/*************** Comparison function	*******************************/
	int	compare(const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &other)		const throw();
	bool 	operator == (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> & other)	const throw()	{ return compare(other) >= 0;	}

	/*************** Convertion function	*******************************/
	std::string	to_canonical_string()				const throw();
	static T	from_canonical_string(const std::string &str)	throw();

	/*************** display function	*******************************/
	std::string	to_string()		const throw();
	friend	std::ostream &	operator << (std::ostream & os, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id) throw()
					{ return os << gen_id.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << <> (serial_t& serial, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)
										throw();
	friend	serial_t &operator >> <> (serial_t& serial, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)
									 	throw(serial_except_t);

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << <> (xmlrpc_build_t& xmlrpc_build, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)	throw();
	friend	xmlrpc_parse_t &operator >> <> (xmlrpc_parse_t& xmlrpc_parse, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)
										throw(xml_except_t);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor which produce a gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::gen_id_t()	throw()
{
	// zero the data to create a null gen_id_t
	memset(buffer, 0, sizeof(buffer));
}

/** \brief Constructor from a datum_t
 * 
 * - if the datum_t is as long as the gen_id_t, copy it as is
 *   - else derive it thru the hash function
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::gen_id_t(const datum_t &datum)	throw()
{
	// if the datum length is equal to the buffer one, copy it as in
	if( datum.get_len() == sizeof(buffer) ){
		memcpy(buffer, datum.get_data(), sizeof(buffer) );
		return;
	}
	
	// if the datum length is different from the buffer one, derive the buffer from the datum
	// - build the id as the hash output of datum
	std::string	auth_type_str	= skey_auth_algo_t(AUTH_ALGO_ENUM).to_string() 
						+ "/nokey/" + OSTREAMSTR(NB_BYTE);		
	skey_auth_t	skey_auth(auth_type_str.c_str());
	// sanity check - the hash output len MUST be equal to the id.buffer size
	DBG_ASSERT( skey_auth.get_output_len() == sizeof(buffer) );
	// take the new data into account
	skey_auth.update( datum );
	// copy the result in the buffer
	memcpy(buffer, skey_auth.final(), sizeof(buffer));	
}

/** \brief Constructor from string
 * 
 * - if the std::string is .empty() this produce a null gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
void gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::ctor_from_str(const std::string &id_str)	throw()
{
	// nullify the object
	*this = build_null();
	// check if the input string is a hexstring of the proper length
	if( id_str.size() == sizeof(buffer)*2 + 2 && id_str.substr(0, 2) == "0x" ){
		// convert a hexstring into number
		for( size_t i = 0; i < sizeof(buffer); i++ ){
			int	hi = std::tolower(id_str[2 + i*2 +0]);
			int	lo = std::tolower(id_str[2 + i*2 +1]);
			hi -= hi >= 'a' ? 'a' - 10 : '0';
			lo -= lo >= 'a' ? 'a' - 10 : '0';
			buffer[i] = (hi << 4) + lo;
		}
	}else if( !id_str.empty() ){ // NOTE: build the id as hash output of the string IIF not empty
		// init the hash function
		std::string	auth_type_str	= skey_auth_algo_t(AUTH_ALGO_ENUM).to_string() 
							+ "/nokey/" + OSTREAMSTR(NB_BYTE);		
		skey_auth_t	skey_auth(auth_type_str.c_str());
		// sanity check - the hash output len MUST be equal to the id.buffer size
		DBG_ASSERT( skey_auth.get_output_len() == sizeof(buffer) );
		// take the new data into account
		skey_auth.update( id_str.c_str(), id_str.size() );
		// copy the result in the buffer
		memcpy( buffer, skey_auth.final(), sizeof(buffer) );		
	}
	return;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bit operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the xor between 2 gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::operator ^(const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &other) const throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> result;
	for( size_t i = 0; i < sizeof(buffer); i++ )
		result.buffer[i] = buffer[i] ^ other.buffer[i];
	return T(result);
}

/** \brief return the xor between 2 gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::operator &(const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &other) const throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> result;
	for( size_t i = 0; i < sizeof(buffer); i++ )
		result.buffer[i] = buffer[i] & other.buffer[i];
	return T(result);
}


/** \brief return the index of the most significant bits 
 * 
 * - if the buffer[] is 20byte long, the returned value is between [0,159]
 * - if the gen_id_t is null or 0x00 or 0x01, the returned value is 0
 * - if the gen_id_t is 0xf, the returned value is 3
 * - if the gen_id_t is 0xfff...fff, the returned value is 159
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
size_t	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::bit_get_highest_set()		const throw()
{
	size_t	i, j;
	// count the number of empty bytes in the most significant part of the gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>
	for( i = 0; i < sizeof(buffer) && buffer[i] == 0; i++ );
	
	// if the gen_id_t is already fully scaned, stop here
	if( i == sizeof(buffer) )	return 0;
	
	// sanity check - here the buffer[i] MUST be non null
	DBG_ASSERT(buffer[i]);

	// find the most significant bit in the buffer[i] byte	
	// - NOTE: works only because buffer[i] is non null
	for( j = 7; !(buffer[i] & (1<<j)); j-- );

	return ((sizeof(buffer) - 1 - i) * 8) + j;
}

/** \brief clear the n most significant bit
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::bit_clear_n_highest(size_t n_bit)
										throw()
{
	size_t		n_byte_to_clear	= n_bit/8;
	int		last_byte_mask	= 0xff >> (n_bit % 8);
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>	tmp = *this;
	size_t		i;
	// log to debug
	KLOG_DBG("n_bit=" << n_bit << " n_byte_to_clear=" << n_byte_to_clear<< " last_byte_mask=0x" << std::hex << last_byte_mask);
	// sanity check - check that n_bit isnt larger than the max
	DBG_ASSERT( n_bit <= sizeof(buffer)*8 );
	// zero the first full bytes
	for( i = 0; i < n_byte_to_clear; i++ )
		tmp.buffer[i] = 0;
	// zero the partial mask
	tmp.buffer[i]	&= last_byte_mask;
	// return the result
	return T(tmp);
}

/** \brief Overload the operator to shift bits to the right
 * 
 * - it is a unsigned bit shift, so most significant bit inserted are always 0
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
operator >> (const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id, int nb_bit)	throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>	res;
	int		byte_off= nb_bit / 8;
	int		bit_off	= nb_bit % 8;
	// go thru the whole res.buffer[]
	for( int i = 0; i < (int)sizeof(gen_id.buffer); i++ ){
		uint8_t	right_val, left_val;
		int	right_idx	= i-byte_off;
		int	left_idx	= i-byte_off-1;
		left_val	= left_idx  < 0 ? 0 : gen_id.buffer[left_idx];
		right_val	= right_idx < 0 ? 0 : gen_id.buffer[right_idx];
		res.buffer[i]	= (left_val << (8-bit_off)) | (right_val >> bit_off);
	}
	// return the result
	return T(res);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		is_null
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the object is null, false otherwise
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
bool gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::is_null()	const throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>	null_id;
	// note: use memcmp() directly and not the comparison operator to avoid infinite reccursion
	if( memcmp(buffer, null_id.buffer, sizeof(buffer)) )	return false;
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			static build function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief returned a gen_id_t filled with random NORMAL
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::build_random()	throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> gen_id;
	// read 'normal random' data and fill the whole buffer with it
	random_pool_t::read_normal(gen_id.buffer, sizeof(gen_id.buffer));
	// return the object itself
	return T(gen_id);
}

/** \brief return a null object
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::build_null()		throw()
{
	// return a gen_id_t build with a default constructor which always build a null gen_id_t
	return T(gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>());
}

/** \brief return a ANY gen_id
 * 
 * - aka a gen_id_t with all bits set
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::build_any()		throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>	gen_id;
	// set all the bits to 1
	memset(gen_id.buffer, 0xFF, sizeof(gen_id.buffer));
	// return a gen_id_t build with a default constructor which always build a null gen_id_t
	return T(gen_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
int gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::compare(const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// log to debug
	KLOG_DBG("comparing " << *this << " with " << other << " returning " 
			<< memcmp(buffer, other.buffer, sizeof(buffer)) );	

	// if both length are equal, compare the data
	return memcmp( buffer, other.buffer, sizeof(buffer) );	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			canonical_string convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return a std::string containing the gen_id_t into canonical from
 * 
 * - MUST keep the same, not intended to be nice for display
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
std::string gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::to_canonical_string()			const throw()
{
	std::ostringstream	oss;
	for( size_t i = 0 ; i < sizeof(buffer); i++ )
		oss << std::hex << std::setfill('0') << std::setw(2) << (int)buffer[i];
	return oss.str();
}


/** \brief convert gen_id_t from canonical_string to internal representation
 * 
 * - MUST keep the same, not intended to be nice for display
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>
T
gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::from_canonical_string(const std::string &str)	throw()
{
	gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>	gen_id;
	// check if the input string is a hexstring of the proper length
	if( str.size() != sizeof(gen_id.buffer)*2 )	return T(gen_id);
	// convert a hexstring into number
	for( size_t i = 0; i < sizeof(gen_id.buffer); i++ ){
		int	hi = std::tolower(str[i*2 +0]);
		int	lo = std::tolower(str[i*2 +1]);
		hi -= hi >= 'a' ? 'a' - 10 : '0';
		lo -= lo >= 'a' ? 'a' - 10 : '0';
		gen_id.buffer[i] = (hi << 4) + lo;
	}
	// return the just built gen_id
	return T(gen_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 * 
 * - The format of the string may change over time. If a canonical way to display
 *   it is needed, use to_canonical_string()
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE>  
std::string gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE>::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "0x";
	for( size_t i = 0 ; i < sizeof(buffer); i++ )
		oss << std::hex << std::setfill('0') << std::setw(2) << (int)buffer[i];
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial gen_id_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
serial_t& operator << (serial_t& serial, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)	throw()
{
	serial.append( gen_id.buffer, sizeof(gen_id.buffer) );
	return serial;
}

/** \brief unserialze a gen_id_t
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
serial_t& operator >> (serial_t& serial, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)  		throw(serial_except_t)
{
	// check the payload length
	if(serial.get_len() < (ssize_t)sizeof(gen_id.buffer))
		nthrow_serial_plain("gen_id_t Payload Too Short");
	// read the payload
	memcpy( gen_id.buffer, serial.get_data(), sizeof(gen_id.buffer) );
	// consume the buffer
	serial.consume( sizeof(gen_id.buffer) );
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc gen_id_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a gen_id_t into a xmlrpc
 * 
 * - it is stored as a string with the output of .to_string()
 *   - TODO this seems bad, it should be from a datum_t with sending base64
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)	throw()
{
	// put a string representing the gen_id_t
	xmlrpc_build << gen_id.to_canonical_string();
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a gen_id_t into a xmlrpc
 * 
 * - it is stored as a string with the output of .to_string()
 *   - TODO this seems bad, it should be from a datum_t with sending base64
 */
template <typename T, skey_auth_algo_t::strtype_enum AUTH_ALGO_ENUM, size_t NB_BYTE> 
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, gen_id_t<T, AUTH_ALGO_ENUM, NB_BYTE> &gen_id)throw(xml_except_t)
{
	std::string	value;
	// get value from the xmlrpc
	xmlrpc_parse >> value;
	// convert the value
	gen_id	= gen_id.from_canonical_string(value);
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                GEN_ID_DECLARATION macro
// - there is some stuff i dont understand between typedef and forward declaration
//   - so a class heritage is used instead
//   - used for the ctor and the interaction with the inherited class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define	NEOIP_GEN_ID_DECLARATION_START(class_name, AUTH_ALGO_ENUM, NB_BYTE)				\
	class class_name : public gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE> {					\
	public:												\
	class_name(const char *id_str)		throw(): gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE>(id_str) {} 	\
	class_name(const std::string &id_str)	throw(): gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE>(id_str) {} 	\
	class_name(const gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE> &gen_id) throw()				\
				: gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE>(gen_id) 	{} 			\
	class_name() throw()	: gen_id_t<class_name, AUTH_ALGO_ENUM, NB_BYTE>() 		{}
// NOTE: it is allowed to add stuff here (e.g. additionnal methods)
#define	NEOIP_GEN_ID_DECLARATION_END(class_name, AUTH_ALGO_ENUM, NB_BYTE)	\
	};


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_GEN_ID_HPP__  */










