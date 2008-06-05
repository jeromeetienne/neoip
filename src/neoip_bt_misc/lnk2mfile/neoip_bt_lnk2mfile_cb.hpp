/*! \file
    \brief Declaration of the bt_lnk2mfilea_t
    
*/


#ifndef __NEOIP_BT_LNK2MFILE_CB_HPP__ 
#define __NEOIP_BT_LNK2MFILE_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_lnk2mfile_t;
class	bt_lnk2mfile_type_t;
class	bt_mfile_t;
class	bt_err_t;

/** \brief the callback class for bt_lnk2mfile_t
 */
class bt_lnk2mfile_cb_t {
public:
	/** \brief callback notified by \ref bt_lnk2mfile_t is completed
	 * 
	 * @param bt_err	is not .succeed() if an error occured
	 * @lnk2mtype		contains the type of the link
	 * @bt_mfile		contains the bt_mfile_t derived from the link
	 */
	virtual bool neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
					, const bt_err_t &bt_err
					, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile) throw() = 0;
	// virtual destructor
	virtual ~bt_lnk2mfile_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_LNK2MFILE_CB_HPP__  */



