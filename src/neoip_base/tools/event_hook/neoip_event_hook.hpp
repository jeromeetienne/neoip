/*! \file
    \brief Header of the \ref event_hook_t.cpp

*/


#ifndef __NEOIP_EVENT_HOOK_HPP__ 
#define __NEOIP_EVENT_HOOK_HPP__ 

/* system include */
#include <vector>
#include <list>
/* local include */
#include "neoip_event_hook_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief class to hook events
 */
class event_hook_t {
private:
	class hook_item {
	public:	event_hook_cb_t *	callback;
		void *			userptr;
		/*************** ctor/dtor	*******************************/
		hook_item(event_hook_cb_t *callback, void *userptr): callback(callback),userptr(userptr){}
		/*************** comparison operator	***********************/
		bool operator == ( const hook_item & other )	const	{
			if( callback != other.callback )	return false;
			if( userptr  != other.userptr )		return false;
			return true;
		}
	};
	class hook_level {
		public:	std::list<hook_item>	item_list;
	};
private:
	std::vector<hook_level>	level_arr;
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks	
public:
	/*************** ctor/dtor	***************************************/
	event_hook_t(size_t nb_level=0)	throw();
	~event_hook_t()			throw();
 
	/*************** utility function	*******************************/
	bool	notify(size_t level_no)							throw();
	void	append(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw();
	void	remove(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw();
	bool	contain(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_EVENT_HOOK_HPP__  */



