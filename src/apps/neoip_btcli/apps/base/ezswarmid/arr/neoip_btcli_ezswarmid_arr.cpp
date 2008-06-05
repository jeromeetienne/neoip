/*! \file
    \brief Definition of the \ref btcli_ezswarmid_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_btcli_ezswarmid_arr.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         action function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Return a btcli_ezswarmid_arr_t for this bt_ezsession_t;
 */
btcli_ezswarmid_arr_t	btcli_ezswarmid_arr_t::from_bt_ezsession(bt_ezsession_t *bt_ezsession)	throw()
{
	btcli_ezswarmid_arr_t	ezswarmid_arr;
	// loop thru the bt_ezsession_t to get the bt_ezswarmid_t from each of bt_ezswarm_t
	const bt_ezsession_t::ezswarm_db_t &		ezswarm_db = bt_ezsession->ezswarm_db();
	bt_ezsession_t::ezswarm_db_t::const_iterator	iter;
	for(iter = ezswarm_db.begin(); iter != ezswarm_db.end(); iter++){
		bt_ezswarm_t *	bt_ezswarm	= *iter;
		// get the infohash from this bt_ezswarm
		const bt_id_t &	infohash	= bt_ezswarm->mfile().infohash();
		// build the btcli_ezswarmid_t and append it to the ezswarmid_arr;
		ezswarmid_arr	+= btcli_ezswarmid_t().infohash(infohash);
	}
	// return the just built btcli_ezswarmid_arr_t;
	return ezswarmid_arr;
}

NEOIP_NAMESPACE_END;




