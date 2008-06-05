/*! \file
    \brief Definition of the unit test for the \ref mlist_t

- TODO write a real nunit

*/

/* system include */
/* local include */
#include "neoip_mlist_nunit.hpp"
#include "neoip_mlist_head.hpp"
#include "neoip_mlist_iter.hpp"
#include "neoip_mlist_item.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief class to test the mlist stuff
 */
class dummy_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string		m_value;
	mlist_head_t<dummy_t>*	m_mlist_head;
	mlist_item_t<dummy_t>	m_mlist_item;
public:
	/*************** ctor/dtor	***************************************/
	dummy_t(mlist_head_t<dummy_t> *p_mlist_head, const std::string &p_value)	throw() 
	{
		// copy the value
		m_value		= p_value;
		m_mlist_head	= p_mlist_head;
		
		KLOG_ERR("value=" << value() << " m_mlist_item=" << &m_mlist_item);
		
		// put this object in front of the list
		m_mlist_head->push_front(m_mlist_item, this);
	}
	~dummy_t()	throw()
	{
		// remove this object from the list
		m_mlist_head->remove(m_mlist_item);
	}

	/*************** query function	***************************************/
	const std::string &	value()	const throw()	{ return m_value;	}
	
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a mlist_t
 */
nunit_res_t	mlist_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	
	mlist_head_t<dummy_t>	mlist_head;
	dummy_t *	dummy1	= nipmem_new dummy_t(&mlist_head, "value ONE");
	dummy_t *	dummy2	= nipmem_new dummy_t(&mlist_head, "value TWO");
	dummy_t *	dummy3	= nipmem_new dummy_t(&mlist_head, "value three");
	
	if( dummy1 );	if( dummy2 );	if( dummy3 );
	
	KLOG_ERR("mlist_head.size()=" << mlist_head.size());

	KLOG_ERR("front value="<< mlist_head.front()->value() );
	KLOG_ERR("back value="<< mlist_head.back()->value() );
	
	
	// go thru the list and display the values
	mlist_iter_t<dummy_t>	iter;
	for(iter = mlist_head.begin(); iter != mlist_head.end(); iter++){
		dummy_t *	dummy	 = iter.object();
		KLOG_ERR("value="<< dummy->value() );
	}

	// delete all object pointed to 
	while( !mlist_head.empty() )	nipmem_delete mlist_head.begin().object();

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

