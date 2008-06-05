/*! \file
    \brief Definition for handling the convertion of bt_clineopt_t


*/

/* system include */
#include <getopt.h>
/* local include */
#include "neoip_clineopt_helper.hpp"
#include "neoip_clineopt_err.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a std::vector<string> based on a clineopt_arr_t
 */
clineopt_err_t clineopt_helper_t::parse(std::vector<std::string> &arg_arr, strvar_db_t &strvar_db
				, const clineopt_arr_t &clineopt_arr)	throw()
{
	// loop on the arg_arr
	while( !arg_arr.empty() ){
		std::string arg_str	= arg_arr[0];
		// if arg_str doesnt start with "-" or "--", leave the loop now
		if( arg_str.substr(0,1) != "-" && arg_str.substr(0,2) != "--" )	break;
		// remove the trailling dash
		if( arg_str.substr(0,2) == "--" )	arg_str	= arg_str.substr(2);
		if( arg_str.substr(0,1) == "-" )	arg_str	= arg_str.substr(1);
		// try to find the arg_str into the clineopt_arr
		size_t	opt_idx;
		for(opt_idx = 0; opt_idx < clineopt_arr.size(); opt_idx++){
			if( clineopt_arr[opt_idx].may_handle(arg_str) )	break;
		}
		// if the arg_str has not been found, return an error
		if( opt_idx == clineopt_arr.size() )
			return clineopt_err_t(clineopt_err_t::UNKNOWN_OPTION, arg_arr[0]);
		// remove the first element of the arg_arr to consume the option
		arg_arr.erase( arg_arr.begin() );
		// if this option is no with parameter, set it up
		if( clineopt_arr[opt_idx].param_mode() == clineopt_mode_t::NOTHING ){
			// insert the variable in the strvar_db
			strvar_db.append(clineopt_arr[opt_idx].canonical_name(), "true");
			// continue the loop
			continue;
		}
		// sanity check - here the param_mode MUST NOT be NOTHING
		DBG_ASSERT( clineopt_arr[opt_idx].param_mode() != clineopt_mode_t::NOTHING );
		// set the arg_parameter
		std::string	arg_param	= clineopt_arr[opt_idx].default_value();
		// if a parameter is provided in the arg_arr, take it from there		
		if( arg_arr.size() >= 1 && arg_arr[0].substr(0,1) != "-" ){
			// overwrite the default value with the provided value
			arg_param	= arg_arr[0];
			// remove the first element of the arg_arr to consume the parameter
			arg_arr.erase( arg_arr.begin() );
		}
		// if the arg_arr doesnt contain enougth element to have a parameter, report an error
		if( arg_param.empty() && clineopt_arr[opt_idx].param_mode() == clineopt_mode_t::REQUIRED )
			return clineopt_err_t(clineopt_err_t::NO_PARAM, clineopt_arr[opt_idx].canonical_name());
		// insert the variable in the strvar_db
		strvar_db.append(clineopt_arr[opt_idx].canonical_name(), arg_param);
	}
	
	// return no error
	return clineopt_err_t::OK;
}

/** \brief Check that the strvar_db is valid
 * 
 * - NOTE: currently it check for the presence of all mandatory options
 */
clineopt_err_t	clineopt_helper_t::check(strvar_db_t &strvar_db
					, const clineopt_arr_t &clineopt_arr)	throw()
{
	// check that all required option are in the strvar_db_t, else return an error
	for(size_t i = 0; i < clineopt_arr.size(); i++){
		const clineopt_t &	clineopt	= clineopt_arr[i];
		// if this clineopt_t is not required, goto the next
		if( clineopt.option_mode() != clineopt_mode_t::REQUIRED )	continue;
		// if this clineopt_t is present in the strvar_db_t, goto the next
		if( strvar_db.contain_key(clineopt.canonical_name()) )		continue;
		// return an error MISSING_OPTION
		return clineopt_err_t(clineopt_err_t::MISSING_OPTION, clineopt.canonical_name());
	}
	
	// return no error
	return clineopt_err_t::OK;
}


/** \brief Generate the help string for the clineopt_arr_t
 */
std::string	clineopt_helper_t::help_string(const clineopt_arr_t &clineopt_arr)	throw()
{
	std::ostringstream	oss;
	// display every option
	for(size_t i = 0; i < clineopt_arr.size(); i++){
		const clineopt_t &	option	= clineopt_arr[i];
		oss << "--" << option.canonical_name();
		if( option.param_mode() == clineopt_mode_t::NOTHING)	oss << "        ";
		if( option.param_mode() == clineopt_mode_t::OPTIONAL)	oss << " [param]";
		if( option.param_mode() == clineopt_mode_t::REQUIRED)	oss << "  param ";
		oss << ": " << option.help_string() << "\n";
		if( !option.alias_name_db().empty() ){
			oss << "\t\t" << "Alternative alias string :";
			for(size_t j = 0; j < option.alias_name_db().size(); j++ ){
				if( j != 0 && j < option.alias_name_db().size()-1)	oss << " or ";
				oss << "'" << option.alias_name_db()[j] << "'";
			}
			oss << "\n";
		}
		if( !option.default_value().empty() )
			oss << "\t\t" << "Default value :" << option.default_value() << "\"\n";
		if( option.option_mode()== clineopt_mode_t::REQUIRED )
			oss << "\t\t" << "This option is required." << "\n";
	}
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






