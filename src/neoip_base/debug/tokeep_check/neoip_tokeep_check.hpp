/*! \file
    \brief Declaration of the tokeep_check_t

*/


#ifndef __NEOIP_TOKEEP_CHECK_HPP__ 
#define __NEOIP_TOKEEP_CHECK_HPP__ 
/* system include */
/* local include */
#include "neoip_slotpool.hpp"
#include "neoip_cpp_demangle.hpp"
#include "neoip_cpp_backtrace.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref tokeep_check_t is a debug tool to check if the proper value 
 *         of 'tokeep' is returned from callbacks
 * 
 * \par Principles
 * - in the ctor, it register itself in a global database
 * - in the dtor, it unregister itself
 * - to check the 'tokeep' value
 *   -# the tokeep_check_id_t MUST be backed up 
 *   -# the callback returning a 'tokeep' is notified
 *   -# then a test is done on the database to ensure that the returned tokeep
 *      match the database state.
 *      - aka return true/tokeep means the object is still in the database
 *      - aka return false/dontkeep means the object is no more in the database
 * 
 * \par Usage
 * - the object notifying the callback define a tokeep_check_t in itself
 *   - thus the ctor/dtor of the tokeep_check_t matches the one of the 
 *     notifying object.
 * - each time, a callback is notified, the tokeep_check must be used to 
 *   check the 'tokeep' value returned by the callback
 *   - TOKEEP_CHECK_BACKUP() before the callback notification
 *   - TOKEEP_CHECK_MATCH(tokeep) after the callback notification
 * 
 * \par Implementation note:
 * - to identify a given object, a slotpool_t is used in the database
 *   - to use a pointer would be vulnerable to race condition if the object
 *     is deleted and another get allocated at the same address.
 * - NOTE: as the slotpool_t::find() is never used, it is useless to store
 *   the pointer of the object for the tokeep_check_t object
 *   - it could be optimized with a slotpool_t like which doesnt have pointer
 *   - But what would be the point ? why duplicate codes in order to improve 
 *     efficiency as tokeep_check_t is only a debug feature.
 */
class tokeep_check_t : NEOIP_COPY_CTOR_ALLOW {
private:
	slot_id_t	slot_id;	//!< the slot_id of this object
	
	/*************** internal function	*******************************/	
	void		internal_ctor()	throw();
	void		internal_dtor()	throw();
public:
	/*************** ctor/dtor	***************************************/
	tokeep_check_t()				throw();
	~tokeep_check_t()				throw();
	tokeep_check_t(const tokeep_check_t &other)	throw();
	tokeep_check_t &	operator=(const tokeep_check_t& other);

	/*************** query function	***************************************/
	const slot_id_t	get_slot_id()	const throw()	{ return slot_id;	}
};

/** \brief Context to save before notifying the callback
 */
struct tokeep_check_backup_t : NEOIP_COPY_CTOR_ALLOW {
	slot_id_t	slot_id;
	std::string	callback_typename;
	cpp_backtrace_t	backtrace;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       define for the 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// default macro to handle the tokeep_check
#define TOKEEP_CHECK_DECL_DFL()		TOKEEP_CHECK_DECL(tokeep_check)
#define TOKEEP_CHECK_BACKUP_DFL(type_name)								\
			TOKEEP_CHECK_BACKUP(tokeep_check_backup, tokeep_check, type_name)
#define TOKEEP_CHECK_MATCH_DFL(returned_tokeep)	TOKEEP_CHECK_MATCH(returned_tokeep, tokeep_check_backup)


#if !NO_TOKEEP_CHECK_DECLARE
// to put in the declaration of the object notifying the callback
#	define TOKEEP_CHECK_DECL(tokeep_check_name)	tokeep_check_t	tokeep_check_name
#else
#	define TOKEEP_CHECK_DECL(tokeep_check_name)
#endif

#if !NO_TOKEEP_CHECK_USAGE && !NO_TOKEEP_CHECK_DECLARE
// to do before notifying the callback
#	define TOKEEP_CHECK_BACKUP(backup_name, tokeep_check_name, type_name)				\
				tokeep_check_backup_t	backup_name;					\
				backup_name.slot_id		= (tokeep_check_name).get_slot_id();	\
				backup_name.callback_typename	= neoip_cpp_typename(type_name);	\
				backup_name.backtrace		= cpp_backtrace_t().initialize()
// to do after notifying the callback
#	define TOKEEP_CHECK_MATCH(returned_tokeep, backup_name)					\
	do{											\
		DBG_ASSERT( tokeep_check_slotpool );						\
		if( returned_tokeep != tokeep_check_slotpool->exist(backup_name.slot_id) ){	\
			KLOG_STDERR("callback =" << backup_name.callback_typename 		\
					<< " backtrace=" << backup_name.backtrace		\
					<< " returned_tokeep=" << returned_tokeep		\
					<< " but should be " << tokeep_check_slotpool->exist(backup_name.slot_id) \
					<< ". slotid=" << backup_name.slot_id);			\
			DBG_ASSERT( 0 );							\
		}										\
	}while(0)
#else
#	define TOKEEP_CHECK_BACKUP(backup_name, tokeep_check_name, type_name)	do{} while(0)
#	define TOKEEP_CHECK_MATCH(returned_tokeep, backup_name)			do{} while(0)
#endif
// global definition for the tokeep_check_slotpool
extern slotpool_t *	tokeep_check_slotpool;

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TOKEEP_CHECK_HPP__  */



