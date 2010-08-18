/*! \file
    \brief Definition of the ipport_strlist_src_t

\par Implementation Note
- coded with brain off, so may be bugged

*/

/* system include */
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <iomanip>
/* local include */
#include "neoip_ipport_strlist_src.hpp"
#include "neoip_host2ip.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ipport_strlist_src_t::ipport_strlist_src_t()		throw()
{
	// zero some field
	host2ip		= NULL;
	callback	= NULL;
}

/** \brief Desstructor
 */
ipport_strlist_src_t::~ipport_strlist_src_t()		throw()
{
	// delete host2ip_t if needed
	nipmem_zdelete host2ip;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation for this object
 */
inet_err_t	ipport_strlist_src_t::start(ipport_strlist_src_cb_t *callback, void *userptr)throw()
{
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	// set some parameter
	cur_idx			= 0;
	want_more		= false;
	// return no error
	return inet_err_t::OK;
}

/** \brief append a new ipport_str in the database of this object
 * 
 * - NOTE: the ipport_str may contain an hostname in the address part
 */
ipport_strlist_src_t &	ipport_strlist_src_t::append(const std::string &ipport_str)	throw()
{
	// sanity check - the object MUST NOT be start()ed
	DBG_ASSERT( callback == NULL );

	// append the ipport_str
	strlist_db.push_back(ipport_str);
	// return the object itself
	return *this;
}

/** \brief Load a file containing a list of ipport_addr_t (possibly containing
 *         hostname) and push it in the ipport_strlist_src_t
 * 
 * @return a inet_err_t
 */
inet_err_t	ipport_strlist_src_t::append_from_file(const std::string &filename)	throw()
{
	char		line[1024+1];
	std::ifstream 	file_in;
	// open the file
	try {
		file_in.open( filename.c_str(), std::ios::in );
	} catch(...){
		KLOG_ERR("Cant open filename " + filename);
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant open filename " + filename);
	}
	
	// if the opening failed, return an error
	if( !file_in.good() ){
		KLOG_ERR("Cant open filename " + filename);
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant open filename " + filename);
	}
	
	// read all line in the file
	while( file_in.good() ){
		file_in.getline( line, sizeof(line)-1 );
		// if the line start with a '#', it is a comment, skip it
		if( line[0] == '#' )	continue;
		// if the line is empty, skip it
		if( strlen(line) == 0 )	continue;
		// NOTE: here the line buffer contains a valid C kindof string
		this->append(line);
	}
	// return no error
	return inet_err_t::OK;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       utility function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Parse an ipport_str
 * 
 * - NOTE: if the ipport_str is malformed, it assert!!!
 */
std::pair<std::string, size_t>	ipport_strlist_src_t::parse_ipport_str(const std::string &ipport_str)
										const throw()
{
	// log to debug
	KLOG_DBG("ipport_str=" << ipport_str);
	// split by the ':' which split the address from the port number
	std::vector<std::string>	tmp = string_t::split(ipport_str, ":", 2);
	// sanity check - the string MUST be of the form "string:number"
	DBG_ASSERT( tmp.size() == 2 );
	// convert the port
	long int	port = strtol(tmp[1].c_str(), NULL, 10);
	// sanity check - the port 
	DBG_ASSERT( port != LONG_MAX && port != LONG_MIN );
	// return the result
	return std::make_pair(tmp[0], (size_t)port);
}

/** \brief determine if the addr_str is a raw ip address or not
 */
bool	ipport_strlist_src_t::is_raw_ipaddr_str(const std::string &addr_str)	const throw()
{
	// try convert the addr_str into a ipaddr_t
	ip_addr_t	ip_addr = addr_str.c_str();
	// log to debug
	KLOG_DBG("addr_str=" << addr_str << " ipaddr_isnull=" << ip_addr.is_null());
	// it the convertion failed, it isnt considered a raw ip address
	if( ip_addr.is_null() )		return false;
	// else it is considered a hostname
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the ipport_addr_t
 */
bool ipport_strlist_src_t::notify_callback(const ipport_addr_t &ipport_addr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ipport_strlist_src_cb( userptr, *this, ipport_addr );
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Internal function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Main function to deliver the ip_port_str
 * 
 * @return a 'tokeep'
 */
bool ipport_strlist_src_t::do_delivery()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - want_more MUST be true
	DBG_ASSERT( want_more );
	DBG_ASSERT( host2ip == NULL );
	
	/*************** Deliver the tobedelivered_db	***********************/
	// deliver one item from tobedelivered_db if it is not empty
	if( tobedelivered_db.empty() == false ){
		// get the first ipport_addr_t in the tobedelived_db
		ipport_addr_t	ipport_addr	= tobedelivered_db.front();
		tobedelivered_db.pop_front();
		// clear the want_more before notifing the callback
		want_more	= false;		
		// notify the callback of the current address
		bool tokeep	= notify_callback(ipport_addr);
		// if the object has been deleted, return now
		if( !tokeep )	return false;
		// return tokeep
		return true;
	}
	
	
	
	/*************** Deliver the strlist_db	from the idx position	*******/
	// determine the ipport_addr to notify
	ipport_addr_t	ipport_addr;
	if( cur_idx < strlist_db.size() ){
		std::string	ipport_str	= strlist_db[cur_idx];
		// parse the ipport_str
		std::pair<std::string, size_t>	ipport_pair = parse_ipport_str(ipport_str);
		// if the first part of ipport_str is a hostname, launch a host2ip_t on it
		if( is_raw_ipaddr_str(ipport_pair.first) == false ){
			// log to debug
			KLOG_DBG("launching host2ip_t on " << ipport_pair.first);
			// sanity check - host2ip_t MUST NOT be running
			DBG_ASSERT( host2ip == NULL );
			DBG_ASSERT( zerotimer.empty() );
			// launch a host2ip_t passing the port as userptr
			inet_err_t	inet_err;
			host2ip		= nipmem_new host2ip_t();
			inet_err	= host2ip->start(ipport_pair.first, this, (void *)ipport_pair.second);
			DBG_ASSERT( inet_err.succeed() );
			// increase the cur_idx
			cur_idx++;
			DBG_ASSERT( cur_idx <= strlist_db.size() );
			// return a 'tokeep'
			return true;
		}
		// convert the ipport_str into a ipport_addr_t
		ipport_addr	= ipport_str.c_str();
		// sanity check - the convertion MUST succeed
		DBG_ASSERT( !ipport_addr.is_null() );
	}else{
		// if the cur_idx equal to strlist_db.size() this mean this bunch of notification
		// is over, and deliver a empty ip_addr_t to notify the caller of this fact.
		DBG_ASSERT( cur_idx == strlist_db.size() );
		// build a null ipport_addr_t
		ipport_addr	= ipport_addr_t();
	}

	// clear the want_more before notifing the callback
	want_more	= false;		
	// notify the callback of the current address
	bool tokeep	= notify_callback(ipport_addr);
	// if the object has been deleted, return now
	if( !tokeep )	return false;


	// goto the next address (with round-robin)
	cur_idx++;
	// warp around the cur_idx IIF it is *greater* than the strlist_db.size()
	// NOTE: cur_idx MAY be equal to strlist_db.size() here, it it is so
	//       this means this bunch has reached it end and the next
	//       ipport_addr_t to be delivered is a null ipport_addr_t
	if( cur_idx > strlist_db.size() )	cur_idx = 0;

	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by host2ip_t when the result is known
 */
bool	ipport_strlist_src_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
			, const inet_err_t &inet_err, const std::vector<ip_addr_t> &result_arr)	throw()
{
	// display the result
	KLOG_DBG("enter host2ip returned err=" << inet_err << " with " << result_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());
	// delete the host2ip_t
	nipmem_zdelete	host2ip;
	// if the host2ip_t failed to return an ip address, goes on the next
	// - if an error occured in host2ip, dont notify the caller - just goes to the next
	if( inet_err.failed() || result_arr.empty() ){
		get_more();
		return false;
	}

	/* NOTE: another possible strategy
	 * - currently only the first ip_addr_t of the hostent_t is delivered
	 * - it could be possible to deliver all the addresses in order to 
	 *   play 'tricks' with the DNS record.
	 */

	// build the ipport_addr from the first address of hostent_t and the port from userptr
	uint16_t	port		= (intptr_t)cb_userptr;
	ipport_addr_t	ipport_addr	= ipport_addr_t(result_arr[0], port);
	// put the result at the end of tobedelivered_db
	tobedelivered_db.push_back(ipport_addr);
	// call the main do_delivery() function
	do_delivery();
	// NOTE: here the object may have been deleted, so dont it after this point

	// return dontkeep - as host2ip_t is surely deleted	
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	ipport_strlist_src_t:: neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)
										throw()
{
	// log to debug
	KLOG_DBG("enter");
	// if the caller no more want more, return immediatly
	// - this may happen in some race case e.g. the caller do get_more(), the
	//   zerotimer_t is launched, some delivery is made thru a source callback,
	//   the caller no more want_more after that, and then the zerotimer_t
	//   callback is notified.
	if( !want_more )	return true;
	
	// call the main deliver function
	// NOTE: this function may delete the object, dont use it beyong this point
	return do_delivery();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Misc function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Warn the ipport_strlist_src_t that more addresses are wished
 * 
 * - it may be called several times in a row, is the same as calling it only once
 *   - it just indicate the wish of the caller to get_more()
 * - NOTE: this is ok to use it during a neoip_ipport_strlist_src_cb_t callback 
 */
void	ipport_strlist_src_t::get_more()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the object MUST be started
	DBG_ASSERT( callback );
	// set the want_more
	want_more	= true;
	// launch a zerotimer IIF it is empty and host2ip_t is not running
	if( zerotimer.empty() && !host2ip )	zerotimer.append(this, NULL);
}

/** \brief Push back this address into the address source such as it will be 
 *         the next address notified in the callback.
 * 
 * - NOTE: this is ok to use it during a neoip_ipport_strlist_src_cb_t callback 
 */
void	ipport_strlist_src_t::push_back(const ipport_addr_t &ipport_addr)	throw()
{
	// sanity check - the object MUST be started
	DBG_ASSERT( callback );
	// push the ipport_addr_t at the begining of tobedelivered_db
	tobedelivered_db.push_front(ipport_addr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object into a string
 */
std::string	ipport_strlist_src_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "[";
	for(size_t i = 0; i < strlist_db.size(); i++){
		oss << "[" << strlist_db[i] << "]";
	}
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END


