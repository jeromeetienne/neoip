/*! \file
    \brief Header of the expireset2_item_t

*/


#ifndef __NEOIP_EXPIRESET2_ITEM_HPP__ 
#define __NEOIP_EXPIRESET2_ITEM_HPP__ 
/* system include */
#include <set>
#include <iostream>
/* local include */
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T> class expireset2_t;

/** \brief The item for expireset2_t
 */
template <typename T> class expireset2_item_t : public timeout_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	expireset2_t<T>*expireset;	//!< backpointer on the expireset2_t
	T		m_value;	//!< the value itself store in the expireset2_t

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;	//!< the expliration timeout_t
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:	
	/*************** ctor/dtor	***************************************/
	expireset2_item_t(const T &m_value, const delay_t &item_ttl, expireset2_t<T> *expireset)	throw();
	~expireset2_item_t()		throw();

	/*************** Query function	*******************************/
	const T &	value()		const throw() { return m_value;				}
	delay_t		expire_delay()	const throw() { return expire_timeout.get_expire_delay();}
	
	/*************** Comparison operator	***********************/
	bool	operator ==(const expireset2_item_t<T> & other)	const throw()	{ return this->value() == other->value();	}
	bool	operator !=(const expireset2_item_t<T> & other)	const throw()	{ return this->value() != other->value();	}
	bool	operator < (const expireset2_item_t<T> & other)	const throw()	{ return this->value() <  other->value();	}
	bool	operator <=(const expireset2_item_t<T> & other)	const throw()	{ return this->value() <= other->value();	}
	bool	operator > (const expireset2_item_t<T> & other)	const throw()	{ return this->value() >  other->value();	}
	bool	operator >=(const expireset2_item_t<T> & other)	const throw()	{ return this->value() >= other->value();	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
template <typename T>	
expireset2_item_t<T>::expireset2_item_t(const T &m_value, const delay_t &item_ttl
						, expireset2_t<T> *expireset)	throw()
{
	// log to debug
	KLOG_DBG("enter value=" << m_value << " this=" << this);
	// copy the data
	this->m_value	= m_value;
	this->expireset	= expireset;
	// put the itement in the database
	expireset->item_db.push_back(this);
	// start the timeout
	expire_timeout.start(item_ttl, this, NULL);
}

/** \brief Destructor
 */
template <typename T>	
expireset2_item_t<T>::~expireset2_item_t()		throw()
{
	// remove the itement from the database
	expireset->item_db.remove(this);		
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback notified when timeout_t expire
 */
template <typename T>	
bool expireset2_item_t<T>::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// autodelete
	nipmem_delete this;
	// return 'dontkeep'remove
	return false;
}
	
NEOIP_NAMESPACE_END


#endif	/* __NEOIP_EXPIRESET2_ITEM_HPP__  */



