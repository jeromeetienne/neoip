/*! \file
    \brief Definition of the \ref bencode_t class
    
*/

/* system include */
#include <iostream>
#include <glib.h>
/* local include */
#include "neoip_bencode.hpp"
#include "neoip_dvar_int.hpp"
#include "neoip_dvar_str.hpp"
#include "neoip_dvar_arr.hpp"
#include "neoip_dvar_map.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              from_dvar function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a bencoded string from a dvar_t
 */
std::string	bencode_t::from_dvar( const dvar_t &dvar )			throw()
{
	std::ostringstream	oss;
	
	switch( dvar.type().get_value() ){
	case dvar_type_t::INTEGER:
			// in the bencode format, integer are signed and i choosed 64bit
			oss << "i" << dvar.integer().to_int64() << "e";
			break;
	case dvar_type_t::STRING:
			oss << dvar.str().size() << ":" << dvar.str().get();
			break;
	case dvar_type_t::ARRAY:
			oss << "l";
			for(size_t i = 0; i < dvar.arr().size(); i++)
				oss << bencode_t::from_dvar(dvar.arr()[i]);
			oss << "e";
			break;
	case dvar_type_t::MAP:{
			const std::map<std::string, dvar_t> &		inmap	=  dvar.map().get_inmap();
			std::map<std::string, dvar_t>::const_iterator	iter;
			oss << "d";
			for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
				const std::string &	key	= iter->first;
				const dvar_t		item	= iter->second;				
				oss << key.size() << ":" << key;
				oss << bencode_t::from_dvar(item);
			}
			oss << "e";			
			break;}
	default:	DBG_ASSERT( 0 );
	}
	
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              to_dvar function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a dvar_t from a bencoded istringstream
 */
dvar_t		bencode_t::to_dvar_iss(std::istringstream &iss)	throw()
{
	char	c_begin;
	// log to debug
	KLOG_DBG("enter iss=" << iss.str());
	// sanity check - the iss MUST not be empty
	DBG_ASSERT( !iss.str().empty() );
	
	// get the first character
	iss >> c_begin;
	// if an error occured, return a null dvar_t
	if( iss.eof() || iss.bad() )		return dvar_t();
	
	// parse the rest according to the first character
	switch(c_begin){
	case 'i':{	// in the bencode format, integer are signed and i choosed 64bit
			int64_t	val;
			char	c_end;
			iss >> val;
			iss >> c_end;
			// if an error occured, return a null dvar_t
			if( iss.eof() || iss.bad() )		return dvar_t();
			// if the last character is not 'e', return a null dvar_t
			if( c_end != 'e' )			return dvar_t();
			return dvar_int_t(val);}
	case '0':	case '1':	case '2':	case '3':	case '4':
	case '5':	case '6':	case '7':	case '8':	case '9':{
			int	len	= c_begin - '0';
			char	c_digit;
			// parse the length
			do{
				iss.get(c_digit);
				// if an error occured, return a null dvar_t
				if( iss.eof() || iss.bad() )	return dvar_t();
				// update the length
				if( std::isdigit(c_digit) )	len	= len * 10 + (c_digit - '0');
				// if the char is not ':' or a digit, return an error
				if( c_digit != ':' && !std::isdigit(c_digit) )	return dvar_t();
				// loop until ':' is found
			}while( c_digit != ':' );
			// read the string of len character - with \0 allowed
			char *	buffer	= (char *)nipmem_alloca(len);
			iss.read(buffer, len);
			// if an error occured, return a null dvar_t
			if( iss.eof() || iss.bad() )	return dvar_t();
			// return the dvar_str_t
			return dvar_str_t( std::string(buffer, len) );}
	case 'l':{	char	c_end;
			dvar_t	dvar	= dvar_arr_t();
			while( true ){
				// get the first character
				iss.get( c_end );
				// if an error occured, return a null dvar_t
				if( iss.eof() || iss.bad() )		return dvar_t();
				// if it is the end of the list, return the built dvar_t
				if( c_end == 'e' )			return dvar;
				// as c_end is not the end, put it back in the stream
				iss.putback( c_end );
				// get the variable
				dvar_t	tmp	= to_dvar_iss(iss);
				// if parsing this variable failed, return a null dvar_t
				if( tmp.is_null() )	return dvar_t();
				// add this dvar_t to the dvar_arr_t
				dvar.arr()	+= tmp;
			};}
	case 'd':{	char	c_end;
			dvar_t	dvar	= dvar_map_t();
			while( true ){
				// get the first character
				iss.get( c_end );
				// if an error occured, return a null dvar_t
				if( iss.eof() || iss.bad() )		return dvar_t();
				// if it is the end of the list, return the built dvar_t
				if( c_end == 'e' )			return dvar;
				// as c_end is not the end, put it back in the stream
				iss.putback( c_end );
				// get the key variable
				dvar_t	tmp_key	= to_dvar_iss(iss);
				// if parsing this variable failed, return a null dvar_t
				if( tmp_key.is_null() )				return dvar_t();
				// if the key is not a string
				if( tmp_key.type() != dvar_type_t::STRING )	return dvar_t();
				// get the variable
				dvar_t	tmp_var	= to_dvar_iss(iss);
				// if parsing this variable failed, return a null dvar_t
				if( tmp_var.is_null() )				return dvar_t();
				// add this dvar_t to the dvar_arr_t
				dvar.map().insert(tmp_key.str().get(), tmp_var);
			};}
	default:	return dvar_t();
	}


	// return a null dvar_t
	return dvar_t();
}

/** \brief build a dvar_t from a bencoded string
 */
dvar_t		bencode_t::to_dvar(const std::string &str)		throw()
{
	std::istringstream	iss( str );
	return to_dvar_iss( iss );
}

/** \brief Helper on top of the other to_dvar
 */
dvar_t		bencode_t::to_dvar(const datum_t &datum)		throw()
{
	return to_dvar(string_t::from_datum(datum));
}

NEOIP_NAMESPACE_END


