/*! \file
    \brief Declaration of the bt_id_t

*/


#ifndef __NEOIP_BT_ID_HPP__ 
#define __NEOIP_BT_ID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(bt_id_t, skey_auth_algo_t::SHA1, 20);

public:
	/*************** peerid helper	***************************************/
	std::string	peerid_progfull()	const throw();
	std::string	peerid_progname()	const throw();
	std::string	peerid_progvers()	const throw();
	static bt_id_t	build_peerid(const std::string &progname, const std::string &progvers)	throw();

NEOIP_GEN_ID_DECLARATION_END	(bt_id_t, skey_auth_algo_t::SHA1, 20);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ID_HPP__  */



