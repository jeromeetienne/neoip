/*! \file
    \brief Definition of the unit test for the \ref acl_t

\par Brief description
This module performs an unit test for the acl_t classes

\par NOTE
- an acl allows to to determine if a given name is allowed to perform a given operation
- the name must be authenticated
- action, operation pattern, name pattern, authority pattern

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_acl_utest.hpp"
#include "neoip_acl.hpp"
#include "neoip_acl_dstring.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_datum.hpp"

using namespace neoip;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             ACL_T
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief dummy class to store operation name for acl_utest
 */
class neoip_acl_utest_op_name_t {
private:
	std::string	op_name;
public:
	neoip_acl_utest_op_name_t( const std::string &op_name )	throw() 
		: op_name(op_name) {}
friend	class neoip_acl_utest_op_pattern_t;
};

/** \brief dummy class to store operation pattern for acl_utest
 */
class neoip_acl_utest_op_pattern_t {
private:
	std::string	op_pattern;
public:
	neoip_acl_utest_op_pattern_t()	throw()	{}
	neoip_acl_utest_op_pattern_t( const std::string &op_pattern )	throw() 
		: op_pattern(op_pattern) {}
	bool pattern_match( const neoip_acl_utest_op_name_t &op_name )	const throw(){
		// perform pattern match based on the glob function
		if( string_t::glob_match(op_pattern, op_name.op_name))	return true;
		return false;
	}
friend std::ostream& operator << (std::ostream & os, const neoip_acl_utest_op_pattern_t &utest_pat)	throw();
friend serial_t& operator << (serial_t& serial, const neoip_acl_utest_op_pattern_t &utest_pat)	throw();
friend serial_t& operator >> (serial_t& serial, neoip_acl_utest_op_pattern_t &utest_pat) 	throw(serial_except_t);
};
std::ostream& operator << (std::ostream & os, const neoip_acl_utest_op_pattern_t &utest_pat)	throw()
{
	os << utest_pat.op_pattern;
	return os;
}
serial_t& operator << (serial_t& serial, const neoip_acl_utest_op_pattern_t &utest_pat)	throw()
{
	datum_t	datum(utest_pat.op_pattern.c_str(),utest_pat.op_pattern.size()+1);
	serial << datum;
	return serial;
}
serial_t& operator >> (serial_t& serial, neoip_acl_utest_op_pattern_t &utest_pat) 	throw(serial_except_t)
{
	datum_t	datum;
	serial >> datum;
	utest_pat.op_pattern = (char *)datum.get_data();
	return serial;
}

typedef acl_t<neoip_acl_utest_op_name_t, neoip_acl_utest_op_pattern_t> neoip_acl_utest_t;

/** \brief unit test for the neoip_crypto_acl class
 */
int neoip_acl_t_utest()
{
	int			n_error = 0;
	neoip_acl_utest_t	acl_utest;
	acl_action_t		acl_action;

	// add rules to the acl
	// allow john.smith.org
	acl_utest.add( acl_action_t::ALLOW, neoip_acl_utest_op_pattern_t("john.smith.org") );
	// deny everybody else from smith.org
	acl_utest.add( acl_action_t::DENY, neoip_acl_utest_op_pattern_t("*.smith.org") );
	// ask for everybody else
	acl_utest.add( acl_action_t::ASK, neoip_acl_utest_op_pattern_t("*") );
	
	// check if the acl works as expected
	if( acl_utest.find(neoip_acl_utest_op_name_t("john.smith.org")) != acl_action_t::ALLOW
		|| acl_utest.find(neoip_acl_utest_op_name_t("alice.smith.org")) != acl_action_t::DENY
		|| acl_utest.find(neoip_acl_utest_op_name_t("alice.wonder.org")) != acl_action_t::ASK ){
		KLOG_ERR("acl_t::find() doesnt produce the good result! BUG!!");
		n_error++;
	}

	// test if serialization is consistent
	serial_t	serial;
	serial << acl_utest;
 	neoip_acl_utest_t	unserial_acl;
	serial >> unserial_acl;
	if( OSTREAMSTR(acl_utest) != OSTREAMSTR(unserial_acl) ){
		KLOG_ERR("serialization of acl_t isnt consistent! BUG!!");
		n_error++;
	}
	return n_error;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             ACL_DSTRING
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief test the acl_dstrint_t
 */
static int neoip_acl_dstring_utest()
{
	int	n_error = 0;

	acl_dstring_t	acl_dstring;
 	acl_dstring_t	unserial_acl_dstring;
	
	acl_dstring.add(acl_action_t::ALLOW	, "alice.bob.org"	, "rootca.neo-ip.org");
	acl_dstring.add(acl_action_t::DENY	, "*.bob.org"		, "rootca.neo-ip.org");
	acl_dstring.add(acl_action_t::ASK	, "*"			, "rootca.neo-ip.org");
	
	// check if the acl works as expected
	if( acl_dstring.find("alice.bob.org", "rootca.neo-ip.org") != acl_action_t::ALLOW
		|| acl_dstring.find("colin.bob.org", "rootca.neo-ip.org") != acl_action_t::DENY
		|| acl_dstring.find("ernest.org", "rootca.neo-ip.org") != acl_action_t::ASK ){
		KLOG_ERR("acl_dstring_t::find() doesnt produce the good result! BUG!!");
		n_error++;
	}	
	
	// test if serialization is consistent
	serial_t	serial;
	serial << acl_dstring;
	serial >> unserial_acl_dstring;
	if( OSTREAMSTR(acl_dstring) != OSTREAMSTR(unserial_acl_dstring) ){
		KLOG_ERR("serialization of acl_dstring_t isnt consistent! BUG!!");
		n_error++;
	}
//	KLOG_ERR("acl_dstring="<< acl_dstring);

	return n_error;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             MAIN
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief unit test for the neoip_crypto_acl class
 */
int neoip_acl_utest()
{
	int			n_error = 0;

	n_error += neoip_acl_dstring_utest();
	n_error += neoip_acl_t_utest();
	

	if( n_error )	goto error;
	KLOG_ERR("ACL_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("ACL_UTEST FAILED!!!!");
	return -1;
}

