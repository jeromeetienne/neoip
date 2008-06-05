/*! \file
    \brief Definition of the upnp_portcleaner_t

\par Algorithm
- basic: if a given upnp_portdesc_t is observed as unchanged during more than
  ttl second, then it is considered staled and must be deleted
  - corrolary: any upnp_bindport_t MUST refresh the upnp_portdesc_t inside
    the upnp router every ttl second or less.

\par Description of the tag
- it has a predefine prefix 'ttl'
- it is followed by a number of second which is the actual time to live
- then a slash '/'
- then a nonce
- then a slash '/'
- example: desc='ttl900/e/neoip bt TCP'
  - 'ttl900/e/' is the prefix tag
  - 900 is the lease_delay - aka 900sec aka 5min
  - e is the nonce

\par About lease_delay and buggy upnp router
- some bugger upnp router doesnt support to have a non-infinit lease_delay
- lease_delay are usefull in case of ungracefull shutdown
  - this allow the port allocation to disapears instead of being allocated for ever
  - ungracefull shutdown may be a software crash (e.g. OS, programm itself)
    a power-outtage for the computer, or a network issue (e.g. unplugging the wire)
  - in short those cases are quite frequent. 
- POSSIBLE SOLUTION:
  - have a tag in the description string indicating the time of expiration
    and the fact it has been allocation by this program.
  - this tag is appended to the caller provided description_str
  - later scan all the allocation ports, and remove the ones which has
    the progname as tag and which has an expired ttl.

*/

/* system include */
/* local include */
#include "neoip_upnp_portcleaner.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_scanallport.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_string.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_portcleaner_t::upnp_portcleaner_t()		throw()
{
	// zero some fields
	upnp_watch	= NULL;
	upnp_scanallport= NULL;
}

/** \brief Desstructor
 */
upnp_portcleaner_t::~upnp_portcleaner_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete upnp_scanallport_t if needed
	nipmem_zdelete	upnp_scanallport;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_portcleaner_t &upnp_portcleaner_t::profile(const upnp_portcleaner_profile_t &a_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( a_profile.check().succeed() );	
	// copy the parameter
	this->m_profile	= a_profile;
	// return the object iself
	return *this;
}

/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_portcleaner_t::start(upnp_watch_t *upnp_watch)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->upnp_watch	= upnp_watch;

	// init the probe_timeout stuff
	probe_delaygen	= delaygen_t(profile().probe_delaygen_arg());
	probe_timeout.start(probe_delaygen.current(), this, NULL);
	
	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			static function to build/parse tag_desc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a tag_desc from a original description
 */
std::string	upnp_portcleaner_t::build_tag_desc(const std::string desc_orig
			, const delay_t &portcleaner_lease
			, const std::string &portcleaner_nonce)	throw()
{
	std::ostringstream	oss;
	// build the tag as prefix
	oss << "ttl" << portcleaner_lease.to_sec_32bit(); 
	oss << "/" << portcleaner_nonce << "/";
	// append the original description
	oss << desc_orig;
	// return the just built string
	return oss.str();
}

/** \brief Parse a tag_desc returning the original description and extracting the expire_date 
 */
std::string	upnp_portcleaner_t::parse_tag_desc(const std::string desc_str
			, delay_t &portcleaner_lease, std::string &portcleaner_nonce)	throw()
{
	// zero the returned field - thus they are zeroed in case of error
	portcleaner_lease	= delay_t();
	portcleaner_nonce	= std::string();
	
	// split the desc_str
	std::vector<std::string>	words	= string_t::split(desc_str, "/", 3);
	// if the number of words IS NOT 3, it isnt a portcleaner tag, return desc_str asis
	if( words.size() != 3 )			return desc_str;
	// if first words doesnt start with ttl, return desc_str asis
	if( words[0].substr(0, 3) != "ttl" )	return desc_str;
	// if second words is of size 0, return desc_str asis
	if( words[1].size() == 0 )		return desc_str;
	
	// extract the original description
	std::string	desc_orig= words[2];
	// extract the portcleaner_lease
	std::string	ttl_str	= words[0].substr(3);
	uint32_t	nb_sec	= strtol(ttl_str.c_str(), NULL, 10);
	portcleaner_lease	= delay_t::from_sec(nb_sec);
	// extract the portcleaner_nonce
	portcleaner_nonce	= words[1];
	// return the desc_orig	
	return desc_orig;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_portcleaner_t::neoip_timeout_expire_cb(void *cb_userptr, timeout_t &cb_timeout)	throw()
{
	upnp_err_t	upnp_err;	
	// sanity check - upnp_scanallport MUST be NULL
	DBG_ASSERT( upnp_scanallport == NULL );	
	// log to debug 
	KLOG_DBG("enter");

	// notify the expiration to the delaygen
	probe_delaygen.notify_expiration();
	
	// stop the probe_timeout - it will be relaunched on upnp_scanallport_t completion
	probe_timeout.stop();

	// if upnp_watch_t declares upnp_unavail at this point, just relaunch the probe_timeout
	if( upnp_watch->upnp_unavail() ){
		// relaunch the probe_timeout for the next upnp_scanallport_t
		probe_timeout.start(probe_delaygen.pre_inc(), this, NULL);
		// return tokeep
		return true;		
	}

	// launch a upnp_call_statusinfo_t
	upnp_scanallport= nipmem_new upnp_scanallport_t();
	upnp_err	= upnp_scanallport->start(upnp_watch->current_disc_res(), this, NULL);
	DBG_ASSERT( upnp_err.succeed() );	// TODO poor error management
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_scanallport_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_scanallport_t when completed
 */
bool 	upnp_portcleaner_t::neoip_upnp_scanallport_cb(void *cb_userptr, upnp_scanallport_t &cb_scanallport
						, const upnp_err_t &upnp_err)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	
	// in case of success, process the result
	if( upnp_err.succeed() )	process_new_db(upnp_scanallport->portdesc_arr());

	// delete the upnp_scanallport_t
	nipmem_zdelete	upnp_scanallport;
	// relaunch the probe_timeout for the next upnp_scanallport_t
	probe_timeout.start(probe_delaygen.pre_inc(), this, NULL);	
	// return dontkeep - as the object has just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			process upnp_scanallport_t result
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Process the new database from the upnp_scanallport_t
 * 
 * ALGO:
 * 1. remove from cur_db all item which are not in new_db
 * 2. create an item in cur_db for each new element of new_db
 * 3. remote from cur_db all item which are obsolete
 */
void	upnp_portcleaner_t::process_new_db(const upnp_portdesc_arr_t &new_db)	throw()
{
	// log to debug
	KLOG_DBG("enter new_db=" << new_db);
	KLOG_DBG("enter cur_db=" << cur_db);
	
	// 1. remove from cur_db all item which are not in new_db
	upnp_portcleaner_item_arr_t	tmp;
	for(size_t i = 0; i < cur_db.size(); i++){
		const upnp_portcleaner_item_t	&item	= cur_db[i];
		// if this item upnp_portdesc_t IS NOT in new_db, goto the next
		if( !new_db.contain( item.portdesc() ) )	continue;
		// copy this item
		tmp.append( item );
	}
	cur_db	= tmp;

	// 2. create an item in cur_db for each new element of new_db
	for(size_t i = 0; i < new_db.size(); i++){
		const upnp_portdesc_t &	portdesc	= new_db[i];
		// if this upnp_portdesc_t has no portcleaner_tag, ignore it
		if( !portdesc.has_portcleaner_tag() )	continue;
		// try to find this portdesc in the cur_db
		size_t	j;
		for(j = 0; j < cur_db.size() && portdesc != cur_db[j].portdesc(); j++);
		// if this portdesc has been found in cur_db, goto the next
		if( j != cur_db.size() )	continue;
		// create a new item for this portdesc with the present creation_date
		upnp_portcleaner_item_t	item(portdesc, date_t::present());
		cur_db.append( item );
	}

{
	// 3. remove from cur_db all item which are obsolete
	// - remove it from the cur_db and launch a upnp_call_delport_t on them
	upnp_portcleaner_item_arr_t	tmp;
	for(size_t i = 0; i < cur_db.size(); i++){
		const upnp_portcleaner_item_t	&item	= cur_db[i];
		// if this item is not obsolete, copy this item and continue
		if( !item.is_obsolete() ){
			tmp.append( item );
			continue;
		}
		// log to debug
		KLOG_ERR("cleaning " << item.portdesc());
		// launch a upnp_call_delport_t for this item - and dont copy it
		const upnp_portdesc_t &	portdesc	= item.portdesc();
		upnp_err_t upnp_err	= upnp_watch->launch_delport(portdesc.ipport_pview().port()
						, portdesc.sockfam(), upnp_watch->current_disc_res());
		if( upnp_err.failed() )	KLOG_ERR("cant launch upnp_call_delport_t due to " << upnp_err);
	}
	cur_db	= tmp;
}
}


NEOIP_NAMESPACE_END


