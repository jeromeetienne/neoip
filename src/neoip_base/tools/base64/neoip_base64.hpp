/*! \file
    \brief Header of the neoip_base64 class
    
*/


#ifndef __NEOIP_BASE64_HPP__ 
#define __NEOIP_BASE64_HPP__ 
/* system include */
#include "neoip_string.hpp"
/* local include */
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief helper to manipulate base64
 */
class base64_t {
public:
public:	/////////////////////// constant declaration ///////////////////////////
	//! the alphabete 'normal' aka the one by default
	//! - from rfc3548.3.table1 "The Base 64 Alphabet"
	static const std::string	norm_alphabet;
	//! the alphabete 'safe' aka the one which is safe for uri and filename
	//! - from rfc3548.4.table2 "The "URL and Filename safe" Base 64 Alphabet"
	static const std::string	safe_alphabet;
private:
	//!< return true if 'c' belong to the base64 alphabet, false otherwise
	static bool		belong_to_alphabet(uint8_t c, const std::string &alphabet)	throw()
					{ return alphabet.find(c, 0) != std::string::npos;	}
	//!< return true if 'c' is base64 padding, false otherwise
	static bool		is_padding(uint8_t c)	throw()	{ return c == '='; }
public:
	/*************** encode/decode function	*******************************/
	static std::string	encode(void *data_ptr, size_t data_len, const std::string &alphabet = base64_t::norm_alphabet)	throw();
	static datum_t		decode(const std::string &str, const std::string &alphabet = base64_t::norm_alphabet)		throw();
	static std::string	encode_safe(void *data_ptr, size_t data_len)	throw()	{ return encode(data_ptr, data_len, safe_alphabet);	}
	static datum_t		decode_safe(const std::string &str)		throw()	{ return decode(str, safe_alphabet);			}

	/*************** shortcut based on datum	***********************/
	static datum_t		decode(const datum_t &datum, const std::string &alphabet = base64_t::norm_alphabet) throw()
				{ return decode(datum.to_stdstring(), alphabet);	}
	static std::string	encode(const datum_t &datum, const std::string &alphabet = base64_t::norm_alphabet) throw()
				{ return encode(datum.get_data(), datum.get_len());	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BASE64_HPP__  */



