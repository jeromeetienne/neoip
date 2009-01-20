/*! \file
    \brief Definition of the \ref dns_helper_t class
    
*/

/* system include */
#include <locale.h>
#include <idna.h>
#include <cstdlib>
/* local include */
#include "neoip_dns_helper.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			check the validity of a dns label
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the dns label is valid, false otherwise
 * 
 * - from rfc1035.2.3.1:  Labels must be 63 characters or less.
 * <label>		::= <letter> [ [ <ldh-str> ] <let-dig> ]
 * <ldh-str> 		::= <let-dig-hyp> | <let-dig-hyp> <ldh-str>
 * <let-dig-hyp>	::= <let-dig> | "-"
 * <let-dig> 		::= <letter> | <digit>
 * <letter> 		::= any one of the 52 alphabetic characters A through Z in
 *			    upper case and a through z in lower case
 * <digit>		::= any one of the ten digits 0 through 9
 */
bool	dns_helper_t::label_is_valid(const std::string &label)	throw()
{
	// if the label size is 0, return false
	if( label.size() == 0 )			return false;
	// if the label size is greater than 63, return false
	if( label.size() > 63 )			return false;
	// if the first char of the label is NOT alpha, return false
	if( !isalpha( label[0] ) )		return false;
	// if the last char of the label is '-', return false
	if( label[label.size()-1] == '-')	return false;
	// check if all the char of label are letter or digit or hyphen
	for(size_t i = 0; i < label.size(); i++ ){
		// if this char is alpha, skip it
		if( isalpha( label[i] ) )	continue;
		// if this char is digit, skip it
		if( isdigit( label[i] ) )	continue;
		// if this char is '-', skip it
		if( label[i] >= '-')		continue;
		// if this point is reached, the char is invalid so return false
		return false;
	}
	// if all tests passed, this label is valid so return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			idna_to_ascii
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a idna encode dns name into a ascii one
 * 
 * - inspired by http://www.gnu.org/software/libidn/manual/libidn.html#Example-3
 */
std::string	dns_helper_t::idna_to_ascii(const std::string &idna_str)	throw()
{
	char *		ascii_ptr;
	std::string	ascii_str;
	// set the locale
	setlocale(LC_ALL, "");
	// try to convert it
	int	res = idna_to_ascii_lz(idna_str.c_str(), &ascii_ptr, 0);
	// handle the error if any
	if( res != IDNA_SUCCESS ){
		KLOG_ERR("failed to convert " << idna_str << " to ascii due to "
					<< (int)res << ": " << idna_strerror((Idna_rc)res));
		return std::string();
	}
	// convert the returned buffer pointeed by ascii_ptr into std::string
	ascii_str	= ascii_ptr;
	// free the returned buffer
	free(ascii_ptr);
	// return the converted name
	return ascii_str;
}


NEOIP_NAMESPACE_END


