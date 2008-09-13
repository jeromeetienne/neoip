/*! \file
    \brief Definition of the \ref string_t class
    
*/

/* system include */
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>
/* local include */
#include "neoip_string.hpp"
#include "neoip_base_oswarp.hpp"
#include "neoip_delay.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief split a string into several based on delimiters
 */
std::vector<std::string> string_t::split(const std::string &line, const std::string &delimiter
						, int max_result)		throw()
{
	std::vector<std::string>	result;
	size_t				old_pos	= 0;
	size_t				pos;
	// parse the line
	do{
		// find the next delimiter position
		pos	= line.find_first_of(delimiter, old_pos);
		// if no delimiter has been found, set the position to the end of line
		if( pos == std::string::npos || (int)result.size() >= max_result-1 )
			pos = line.size();
		// put the sub-string into the results
		result.push_back( line.substr(old_pos, pos-old_pos) );
		// update the old_pos
		old_pos	= pos+1;
	}while( pos < line.size() );
	// return the result 
	return result;
		
}

/** \brief parse the str in parameter as a argv array (the one in main() from cmdline)
 */
std::vector<std::string>	string_t::parse_as_argv(const std::string &str)	throw()
{
	std::vector<std::string>	result;
	// use the split function on the input string
	std::vector<std::string>	tmp_res	= split(str, " ");
	// copy all non-empty item of tmp_res into result
	// - NOTE: this one is due to how split handle duplicates. if many separators
	//   are seen in a row, they is no special case, it will create many empty string.
	// - this loop just remove those empty string 
	for(size_t i = 0; i < tmp_res.size(); i++ ){
		// if this item is empty, goto the next
		if( tmp_res[i].empty() )	continue;
		// copy this item into the result
		result.push_back( tmp_res[i] );
	}
	// return the result
	return result;
}

/** \brief return a string with all upper case letter switch to lower case
 */
std::string	string_t::to_lower( const std::string &str )	throw()
{
	std::string	tmp = str;
	transform( tmp.begin(), tmp.end(), tmp.begin(), tolower);
	return tmp;
}

/** \brief return a string with all lower case letter switch to upper case
 */
std::string	string_t::to_upper( const std::string &str )	throw()
{
	std::string	tmp = str;
	transform( tmp.begin(), tmp.end(), tmp.begin(), toupper);
	return tmp;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               nice string display stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a string into a bool
 */
bool	string_t::convert_to_bool(const std::string &str)	throw()
{
	// test for various representation of true
	if( !string_t::casecmp(str, "true") )	return true;
	if( !string_t::casecmp(str, "yes") )	return true;
	if( !string_t::casecmp(str, "y") )	return true;
	if( !string_t::casecmp(str, "1") )	return true;
	// if none matches return false
	// TODO not very robust to error detection
	return false;
}

/** \brief Convert a size in 64-bit into a string
 * 
 * - it must always return a 10char string
 */
std::string	string_t::size_string(uint64_t size)		throw()
{
	double		val	= size;
	std::string	unit_str;
	// convert the val into a float number between [0, 1024[
	if( val >= 1024.0 * 1024.0 * 1024.0){
		val 		/= 1024.0 * 1024.0 * 1024.0;
		unit_str	= "Gbyte";
	}else if( size >= 1024.0 * 1024.0 ){
		val 		/= 1024.0 * 1024.0;
		unit_str	= "Mbyte";
	}else if( size >= 1024.0 ){
		val 		/= 1024.0;
		unit_str	= "Kbyte";
	}else{
		unit_str	= "bytes";
	}

	std::ostringstream	oss;
	// go thru snprintf to get the string as i didnt found a c++ way to do it	
	char		tmp[128];
	std::string	format;
	double		max_value;
	if( unit_str == "bytes" ){
		format		= " %4.0f";
		max_value	= 1024.0;
	}else if( val >= 1000.0 ){
		format		= " %4.0f";
		max_value	= 1024.0;
	}else if( val >= 100.0 ){
		format		= "%3.1f";
		max_value	= 999.9;
	}else if( val >= 10.0 ){
		format		= "%2.2f";
		max_value	= 99.99;
	}else{
		format		= "%1.3f";
		max_value	= 9.999;
	}
	// bound the val by the max_value to avoid inconsistent display due to rounding
	val	= std::min(val, max_value);
	// display the stuff
	snprintf(tmp, sizeof(tmp), format.c_str(), val);
	// build the string
	oss << tmp << unit_str;
	// sanity check - the output MUST already be 10-char long
	//if( oss.str().size() != 10 )	KLOG_ERR("weird bug=" << oss.str());
	DBG_ASSERT( oss.str().size() == 10 );
	// return the just built string
	return oss.str();
}

/** \brief Convert a size in 64-bit into a string
 */
std::string	string_t::size_sepa(uint64_t size)		throw()
{
	std::string		size_str	= OSTREAMSTR(size);
	std::ostringstream	oss;
	// build the string
	size_t	cur_off	= 0;
	size_t	cur_len	= size_str.size() % 3;
	if( cur_len == 0 )	cur_len	= 3;
	while( true ){
		// put a separator if it is not the first bunch
		if( cur_off != 0 )	oss << ",";
		// display this bunch
		oss << size_str.substr(cur_off, cur_len);
		// test if the whole string has been processed
		if( cur_off + cur_len >= size_str.size() )	break;
		// update cur_off
		cur_off	+= cur_len;
		// the next bunch length is 3
		cur_len	= 3;
	}
	// return the just built string
	return oss.str();
}

/** \brief Convert a double [0-1] into a percent string
 * 
 * - it must always return a 5char string
 */
std::string	string_t::percent_string(double percent)		throw()
{
	std::ostringstream	oss;
	// if the percent is nan (not an number), handle a special case
#ifdef _WIN32
	if( _isnan(percent) )	return " nan%";
#else
	if( isnan(percent) )	return " nan%";
#endif
	// sanity check - percent MUST NOT be greater than 1
	DBG_ASSERT( percent <= 1.0 );
	DBG_ASSERT( percent >= 0.0 );
	// multiply the percent by 100
	percent	*= 100.0;
	// go thru snprintf to get the string as i didnt found a c++ way to do it	
	char		tmp[128];
	std::string	format;
	double		max_value;
	if( percent == 100.0  ){
		format		= " %3.0f";
		max_value	= 100.0;
	}else if( percent >= 10.0 ){
		format		= "%2.1f";
		max_value	= 99.9;
	}else{
		format		= "%1.2f";
		max_value	= 9.99;
	}
	// bound the percent by the max_value to avoid inconsistent display due to rounding
	// - normalization to avoid a rounding from the displayer - aka 99.9999% displayed as 100.0%
	percent	= std::min(percent, max_value);
	// display the stuff
	snprintf(tmp, sizeof(tmp), format.c_str(), percent);
	// build the string
	oss << tmp << "%";
	// sanity check - the output MUST already be 5-char long
	DBG_ASSERT( oss.str().size() == 5 );
	// return the just built string
	return oss.str();
}

/** \brief Convert a double [0-infinit] into a ratio string
 * 
 * - this is similar to string_t::percent_string but less rigurous
 *   - the ratio may be > 1.0
 *   - the output is not garanteed to be 5char 
 *     - even if it tries too, large ratio may produce larger output
 */
std::string	string_t::ratio_string(double ratio)		throw()
{
	std::ostringstream	oss;
	// if the percent is nan (not an number), handle a special case
#ifdef _WIN32
	if( _isnan(ratio) )	return " nan%";
#else
	if( isnan(ratio) )	return " nan%";
#endif
	// multiply the ratio by 100
	ratio	*= 100.0;
	// go thru snprintf to get the string as i didnt found a c++ way to do it	
	char		tmp[128];
	std::string	format;
	if( ratio >= 100.0  )		format		= " %3.0f";
	else if( ratio >= 10.0 )	format		= "%2.1f";
	else				format		= "%1.2f";
	// display the stuff
	snprintf(tmp, sizeof(tmp), format.c_str(), ratio);
	// build the string
	oss << tmp << "%";
	// return the just built string
	return oss.str();
}

/** \brief Convert a delay_t to a nice std::string
 */
std::string	string_t::delay_string(const delay_t &_delay)		throw()
{
	delay_t			delay	= _delay;
	std::ostringstream	oss;
	// handle the special values
	if( delay.is_null() )		return "null  ";
	else if(delay.is_infinite())	return "never ";

	delay_t		base_val1, base_val2;
	std::string	unit_val1, unit_val2;
	if( delay > delay_t::from_sec(24*60*60) ){
		base_val1	= delay_t::from_sec(24*60*60);
		unit_val1	= "d";
		base_val2	= delay_t::from_sec(60*60);
		unit_val2	= "h";
	} else if( delay > delay_t::from_sec(60*60) ){
		base_val1	= delay_t::from_sec(60*60);
		unit_val1	= "h";
		base_val2	= delay_t::from_sec(60);
		unit_val2	= "m";
	} else {
		base_val1	= delay_t::from_sec(60);
		unit_val1	= "m";
		base_val2	= delay_t::from_sec(1);
		unit_val2	= "s";
	}
	
	delay_t	val1	= delay / base_val1;
	delay_t	val2	= (delay - val1*base_val1) / base_val2;
	
	// display the val1 - IIF non-zero
	if( val1.to_uint64() )	oss << std::setw(2) << val1.to_uint64() << unit_val1 << std::setfill('0');
	else			oss << "   "; 
	// display the val2
	oss << std::setw(2) << val2.to_uint64() << unit_val2;

	// sanity check - the output MUST already be 6-char long
	if( oss.str().size() != 6 )	KLOG_ERR("weird bug=" << oss.str());	// TODO to remove
	DBG_ASSERT( oss.str().size() == 6 );

	// return the just built string
	return oss.str();
}

/** \brief return a std::string of large of <width> character
 */
std::string	string_t::progress_bar_str(double curavail_pct, double oldavail_pct, size_t width) throw()
{
	std::ostringstream	oss;	
	// sanity check - the width MUST be at least 2
	DBG_ASSERT( width > 2 );
	// sanity check - the curavail_pct MUST be between 0.0 and 1.0 included
	DBG_ASSERT( curavail_pct >= 0.0 && curavail_pct <= 1.0 );
	// sanity check - the oldavail_pct MUST be <= curavail_pct
	DBG_ASSERT( oldavail_pct <= curavail_pct );
	
	// put the head of the progress bar
	oss << "[";

	double	oldcur_ratio	= oldavail_pct / curavail_pct;
	double	char_percent	= 1.0 / (width-2);
	size_t	nb_anyavail	= size_t(curavail_pct / char_percent);
	size_t	nb_oldavail	= size_t(oldcur_ratio * nb_anyavail);
	size_t	nb_curavail	= nb_anyavail - nb_oldavail;
	size_t	nb_nonavail	= (width-2)  - nb_anyavail;
	
	if( nb_oldavail > 0 )	oss << std::string(nb_oldavail, '+');
	if( nb_curavail > 1 )	oss << std::string(nb_curavail-1, '=');
	if( nb_curavail > 0 )	oss << ">";
	if( nb_nonavail > 0 )	oss << std::string(nb_nonavail, ' ');

	// put the tail of the progress bar
	oss << "]";
	
	// sanity check - the just built string size() MUST be width 
	DBG_ASSERT( oss.str().size() == width );	
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			string to a uint64/uint32
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a string describing a size to a uint64_t
 * 
 * - the syntax of this string is "[0-9]+[BkKmMgG]?"
 *   - "3200" = "3200B" = "3200B"3200
 * - if there are no suffix the unit is supposed to be 'byte'
 * - else [bB] = byte, [kK] = Kbyte, [mM] = Mbyte, [gG] = Gbyte
 * - TODO handle some error case :)
 */
uint64_t	string_t::to_uint64(const std::string &str)	throw()
{
	uint64_t	mult	= 1024; 	// NOTE: used here to get the type casting correct
	double		result	= 0.0;
	// log to debug
	KLOG_DBG("enter str=" << str);
	// get the number 
	result	= strtod(str.c_str(), 0);
	// try to find a unit
	size_t	unit_pos = str.find_first_of("bBkKmMgG");
	// if there are no unit, return the result now
	if( unit_pos == std::string::npos )			return uint64_t(result);
	// if the unit is a byte, return the result unchanged
	if( str[unit_pos] == 'b' || str[unit_pos] == 'B' )	return uint64_t(result);
	// if the unit is a Kbyte, return the result * 1024
	if( str[unit_pos] == 'k' || str[unit_pos] == 'K' )	return uint64_t(result*mult);
	// if the unit is a Mbyte, return the result * 1024 * 1024
	if( str[unit_pos] == 'm' || str[unit_pos] == 'M' )	return uint64_t(result*mult*mult);
	// if the unit is a Gbyte, return the result * 1024 * 1024 * 1024
	if( str[unit_pos] == 'g' || str[unit_pos] == 'G' )	return uint64_t(result*mult*mult*mult);
	// return the parsed result
	return uint64_t(result);
}
	
/** \brief Convert a string describing a size to a uint32_t
 * 
 * - see string_t::to_uint64() for details
 */
uint32_t	string_t::to_uint32(const std::string &str)	throw()
{
	// convert the str into a uint64_t
	uint64_t	result	= to_uint64(str);
	// sanity check - the result MUST be able to fit into uint32_t
	DBGNET_ASSERT( result <= (uint64_t)std::numeric_limits<uint32_t>::max() );
	// convert the result to uint32_t and return it
	return uint32_t(result);
}

/** \brief Convert a string describing a size to a uint16_t
 * 
 * - see string_t::to_uint64() for details
 */
uint16_t	string_t::to_uint16(const std::string &str)	throw()
{
	// convert the str into a uint64_t
	uint64_t	result	= to_uint64(str);
	// sanity check - the result MUST be able to fit into uint16_t
	DBGNET_ASSERT( result <= (uint64_t)std::numeric_limits<uint16_t>::max() );
	// convert the result to uint16_t and return it
	return uint16_t(result);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			string searching
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the str parameter match the glob pattern, false otherwise
 */
bool	string_t::glob_match(const std::string &pattern, const std::string &str)	throw()
{
	// if it doesnt matches, return false
	if( base_oswarp_t::fnmatch(pattern.c_str(), str.c_str(), 0) )	return false;
	// else return true
	return true;
}

/** \brief Return the offset of the first occurence of substr in maistr, or string::npos if no matches
 * 
 * - TODO strstr() can do that too :)
 */
size_t	string_t::find_substr(const std::string &mainstr, const std::string &substr)	throw()
{
	// if the mainstr is larger than the substr, return npos now
	if( mainstr.size() < substr.size() )	return std::string::npos;
	// go thru the mainstr
	for(size_t i = 0; i < mainstr.size() - substr.size() + 1;i++){
		// if the mainstr at this index matches the substr, return the index
		if( mainstr.substr(i, substr.size()) == substr )
			return i;
	}
	// if no match is found, return std::string::npos
	return std::string::npos;
}

/** \brief Replace in str all instances of old_val by new_val
 */ 
std::string	string_t::replace(const std::string &orig_str, const std::string &old_val
						, const std::string &new_val)	throw()
{
	std::ostringstream	oss;
	size_t			cur_pos	= 0;
	// log to debug
	KLOG_DBG("in (" << orig_str << ") replace all (" << old_val << ") by (" << new_val <<")");
	// iterate over the orig_string
	while(true){
		// backup the cur_pos into old_pos
		size_t	old_pos	= cur_pos;
		// try to find the old_val within orig_str from cur_pos
		cur_pos	= orig_str.find(old_val, cur_pos);
		// append this range of orig_str into the result
		oss	<< orig_str.substr(old_pos, cur_pos - old_pos);
		// if no matches, leave the loop now
		if( cur_pos == std::string::npos )	break;
		// put the new_val into the result
		oss	<< new_val;
		// update the cur_pos to go beyong the old_val
		cur_pos	+= old_val.size();
	}
	// return the result
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     URI type of escape
// - currently i dont have much knowledge about this
//   - aka is it canonical / normalized etc...
// - so it is to be considered half-backed
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Escape all the char of a string contained in a set
 */
std::string	string_t::escape_in(const std::string &str, const std::string &charset)	throw()
{
	std::string	result;
	// go thru the original string	
	for( size_t i = 0; i < str.size(); i++ ){
		// if it isnt in the charset, simply copy it
		if( charset.find(str[i]) == std::string::npos ){
			result	+= str[i];			
		}else{	// if it is in the charset, replace it by its %xx representation
			std::ostringstream	tmp;
			tmp << "%" << std::hex << std::setfill('0') << std::setw(2) << (int)str[i];
			result	+= tmp.str();
		}
	}
	// return the result
	return result;
}

/** \brief Escape all the char of a string NOT contained in a set
 */
std::string	string_t::escape_not_in(const std::string &str, const std::string &charset)	throw()
{
	std::string	result;
	// go thru the original string	
	for( size_t i = 0; i < str.size(); i++ ){
		// if it isnt in the charset, simply copy it
		if( charset.find(str[i]) != std::string::npos ){
			result	+= str[i];			
		}else{	// if it is in the charset, replace it by its %xx representation
			std::ostringstream	tmp;
			tmp << "%" << std::hex << std::setfill('0') << std::setw(2) << ((int)str[i] & 0xff);
			result	+= tmp.str();
		}
	}
	// return the result
	return result;
}

/** \brief UnEscape escapted char of a string
 */
std::string	string_t::unescape(const std::string &str)	throw()
{
	std::string	result;
	// go thru the source string
	for( size_t i = 0; i < str.size(); i++ ){
		// test if it is a escape sequence
		if( str[i] == '%' && i + std::string("%xx").size()-1 < str.size() 
				&& isxdigit(str[i+1]) && isxdigit(str[i+2]) ){
			// if it is a escape sequence, convert it in the proper char
			int	val = strtoul(str.substr(i+1, 2).c_str(), NULL, 16);
			// skip the escape sequence
			i	+= std::string("%xx").size()-1;
			// copy the converted char
			result	+= (char)val;
		}else{
			// if it is NOT a escape sequence, just copy the char
			result += str[i];
		}
	}
	// return the result
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      String stripping
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief strip all characters in charset at the right of str
 */
std::string	string_t::rstrip(const std::string &str, const std::string &charset)	throw()
{
	size_t		i;
	// if the str to strip is already empty, return it now
	// - it ease end case management in the loop
	if( str.size() == 0 )	return std::string();
	
	// find the position of the first char which isnt in the charset
	for( i = 0; i < str.size() && charset.find(str[str.size()-1-i]) != std::string::npos ; i++ );

	// if the end has been reached, return a null std::string
	if( i == str.size() )	return std::string();

	// return the rest of the string	
	return str.substr(0, str.size()-i);
}


/** \brief strip all characters in charset at the lest of str
 */
std::string	string_t::lstrip(const std::string &str, const std::string &charset)	throw()
{
	size_t		i;
	// find the position of the first char which isnt in the charset
	for( i = 0; i < str.size() && charset.find(str[i]) != std::string::npos ; i++ );

	// if the end has been reached, return a null std::string
	if( i == str.size() )	return std::string();

	// return the rest of the string	
	return str.substr(i);
}


/** \brief strip all characters in charset at the right and left of str
 */
std::string	string_t::strip(const std::string &str, const std::string &charset)	throw()
{
	std::string	tmp = rstrip( str, charset);
	return lstrip(tmp, charset);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      unquote
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief remove leading/trailing quote
 */
std::string	string_t::unquote(const std::string &str)	throw()
{
	return strip(str, "\"\'");
}

NEOIP_NAMESPACE_END


