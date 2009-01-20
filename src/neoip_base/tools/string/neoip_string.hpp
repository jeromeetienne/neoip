/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_STRING_HPP__ 
#define __NEOIP_STRING_HPP__ 
/* system include */
#include <string>
#include <vector>
#include <limits>
/* local include */
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	delay_t;

/** \brief helper to manipulate string
 */
class string_t {
public:
	static std::string	from_datum(const datum_t &datum)	throw()
					{ return std::string((char *)datum.get_data(), datum.size());	}
	static std::vector<std::string>	split(const std::string &str
						, const std::string &delimiter
						, int max_result = std::numeric_limits<int>::max())
						throw();
	static std::vector<std::string>	parse_as_argv(const std::string &str)	throw();
						
	//! Compare 2 strings ignoring the case
	static int casecmp( const std::string str1, const std::string str2 )	throw(){
		return strcasecmp( str1.c_str(), str2.c_str() );
	}
	
	static std::string	to_lower(const std::string &str)	throw();
	static std::string	to_upper(const std::string &str)	throw();
	static bool		convert_to_bool(const std::string &str)	throw();

	static std::string	size_string(uint64_t size)		throw();
	static std::string	size_sepa(uint64_t size)		throw();
	static std::string	percent_string(double percent)		throw();
	static std::string	ratio_string(double ratio)		throw();
	static std::string	delay_string(const delay_t &delay)	throw();
	static std::string	progress_bar_str(double curavail_pct, double oldavail_pct
						, size_t width)		throw();

	static uint64_t		to_uint64(const std::string &str)	throw();
	static uint32_t		to_uint32(const std::string &str)	throw();
	static uint16_t		to_uint16(const std::string &str)	throw();

	static bool		glob_match(const std::string &pattern, const std::string &str)	throw();
	static size_t		find_substr(const std::string &main_str, const std::string &substr)throw();
	static std::string	replace(const std::string &str, const std::string &old_val
					, const std::string &new_val)	throw();
	
	static std::string	rstrip(const std::string &str, const std::string &charset = " ")throw();
	static std::string	lstrip(const std::string &str, const std::string &charset = " ")throw();
	static std::string	strip(const std::string &str, const std::string &charset = " ")	throw();
	
	static std::string	escape_in(const std::string &str, const std::string &charset)	throw();
	static std::string	escape_not_in(const std::string &str, const std::string &charset)throw();
	static std::string	unescape(const std::string &str)				throw();

	static std::string	unquote(const std::string &str)				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRING_HPP__  */



