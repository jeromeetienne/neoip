/*! \file
    \brief Header of the \ref bud_main_t class

*/


#ifndef __NEOIP_BUD_MAIN_HPP__ 
#define __NEOIP_BUD_MAIN_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bud_lshare.hpp"
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	http_fdir_t;
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;

/** \brief To handle the service of buddy
 */
class bud_main_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	udp_vresp_t *		udp_vresp;	//!< the udp_vresp for the kad_listener_t
	kad_listener_t *	kad_listener;	//!< the kad_listener of the daemon
	kad_peer_t *		kad_peer;	//!< the kad_peer in kad_listener_t
	kad_realmid_t		kad_realmid;	//!< the realm_id attached by the subscribed peer
	std::list<std::pair<kad_recid_t, kad_keyid_t> > reckeyid_db;	//!< the list of the recid/keyid published
									//!< in the kad_peer_t

	bud_lshare_db_t		bud_lshare_db;	//!< the database of all the bud_lshare_t

	std::list<http_fdir_t *> http_fdir_db;		//!< the list of all http_fdir_t


	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();

	/*************** http_search handling 	*******************************/
	class					http_search_t;
	std::list<http_search_t *>		http_search_db;
	void http_search_link(http_search_t *search)	throw()	{ http_search_db.push_back(search);	}
	void http_search_unlink(http_search_t *search)	throw()	{ http_search_db.remove(search);		}

	bool	kad_init()		throw();
	void	kad_deinit()		throw();

	bool	lshare_init()		throw();
	void	lshare_deinit()		throw();
		
	bool	publish_init()		throw();
	void	publish_deinit()	throw();
public:
	/*************** ctor/dtor	***************************************/
	bud_main_t()	throw();
	~bud_main_t()	throw();
	
	/*************** setup function	***************************************/
	bool		start()		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BUD_MAIN_HPP__  */



