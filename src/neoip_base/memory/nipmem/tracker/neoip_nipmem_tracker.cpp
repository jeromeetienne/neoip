/*! \file
    \brief definition of the \ref nipmem_tracker_t

*/

/* system include */
#include <iostream>
#include <sstream>
/* local include */
#include "neoip_nipmem_tracker.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nipmem_tracker_t::nipmem_tracker_t()	throw()
{
}

/** \brief Destructor
 */
nipmem_tracker_t::~nipmem_tracker_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			global stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! pointer on the global nipmem_tracker_t - note that it is never freed
static nipmem_tracker_t *	global_nipmem_tracker_ptr		= NULL;

/** \brief return a point on the global nipmem_tracker_t
 */
nipmem_tracker_t *get_global_nipmem_tracker()	throw()
{
	// if the global_nipmem_tracker_ptr is not yet created, create it now
	if( !global_nipmem_tracker_ptr ){
		global_nipmem_tracker_ptr	= new nipmem_tracker_t();
		DBG_ASSERT( global_nipmem_tracker_ptr );
	}
	// return the global_nipmem_tracker_ptr
	return global_nipmem_tracker_ptr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              UTILITY function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief display all the zone
 */
void nipmem_tracker_t::display_all_zone()		const throw()
{
	std::map<void *, zone_t>::const_iterator	iter;
	// go thru the whole zone_db
	for( iter = zone_db.begin(); iter != zone_db.end(); iter++ ){
		const zone_t &	zone = iter->second;
		KLOG_STDERR(zone.to_string() << "\n");
	}
}

/** \brief display all the zone
 */
void nipmem_tracker_t::display_summary()		const throw()
{
#if NO_NIPMEM_SMASHING && NO_NIPMEM_TRACKING
	return;
#else
	// dump a report of all the unfreed allocation
	std::map<void *, zone_t>::const_iterator	iter;
	size_t	total = 0;
	for( iter = zone_db.begin(); iter != zone_db.end(); iter++ ){
		const zone_t &	zone = iter->second;
		KLOG_STDERR("Unfreed " <<  zone.to_string() << "\n");
		// update the total amount of unfreed memory
		total += zone.buffer_len;
	}
	// display the total of unfreed memory
	KLOG_STDERR("The amount of unfreed memory is "<< total << "-byte\n");
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              ZONE insert/remove
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief insert a zone
 */
void nipmem_tracker_t::zone_insert(void *buffer_ptr, ssize_t buffer_len, bool secure_f, const char * filename
					, int lineno, const char * fct_name
					, const std::string &type_name)		throw()
{
	// build the zone_t
	zone_t	zone(buffer_ptr, buffer_len, secure_f, filename, lineno, fct_name, type_name);
	// insert it in the database
	bool	succeed = zone_db.insert(std::make_pair(buffer_ptr, zone)).second;
	DBG_ASSERT( succeed );
}

/** \brief insert a zone
 */
size_t nipmem_tracker_t::zone_remove(void *buffer_ptr, bool secure_f, const char *filename
					, int lineno, const char * fct_name
					, const std::string &type_name)		throw()
{
	std::map<void *, zone_t>::iterator	iter;
	// try to find this element in the database
	iter = zone_db.find(buffer_ptr);
	// if this element is NOT in the database
	if( iter == zone_db.end() ){
		KLOG_STDOUT("Try to free a unallocated zone at " << filename 
				<< ":" << lineno<< ":" << fct_name << "()" << "\n");
		EXP_ASSERT( 0 );
	}
	// Copy on the found element
	// - a copy is needed to return the length after the removal from the database
	zone_t	zone	= iter->second;
	// test if the zone to remove match the stored one
	if( secure_f != zone.secure_f ){
		KLOG_STDOUT("Try to free a " << (zone.secure_f ? "secure" : "unsecure")
						<< " allocated zone via a "	
						<< (secure_f ? "secure" : "unsecure")
						<< " free" << " at " << filename << ":" << lineno
						<< ":" << fct_name << "()"
						<< ". The zone is " << zone.to_string()
						<< "\n");
		EXP_ASSERT( 0 );
	}
	// test if the typename match
	if( type_name != zone.type_name ){
		KLOG_STDOUT("Try to free a " << type_name << " at " << filename << ":" << lineno
						<< ":" << fct_name << "()"
						<< " when a " << zone.type_name
						<< " has been allocated at" << zone.to_string()
						<< "\n");
		EXP_ASSERT( 0 );
	}
	// remove this element from the databse
	zone_db.erase(buffer_ptr);
	// return the size of the removed zone
	return zone.buffer_len;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        nipmem_tracker_t::zone_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object into a string
 */
std::string nipmem_tracker_t::zone_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	if( secure_f )	oss << "secure";
	else		oss << "unsecure";
	oss << " zone " << buffer_ptr << " of " << buffer_len << "-byte";
	oss << " for a " << type_name;
	oss << " at " << filename << ":" << lineno << ":" << fct_name << "()";
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END







