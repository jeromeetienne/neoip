/*! \file
    \brief Declaration of the bt_piece_cpuhash_t
    
*/


#ifndef __NEOIP_BT_PIECE_CPUHASH_CB_HPP__ 
#define __NEOIP_BT_PIECE_CPUHASH_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_piece_cpuhash_t;
class	bt_err_t;
class	bt_id_t;

/** \brief the callback class for bt_piece_cpuhash_t
 */
class bt_piece_cpuhash_cb_t {
public:
	/** \brief callback notified by \ref bt_piece_cpuhash_t 
	 */
	virtual bool neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_bt_piece_cpuhash
							, const bt_err_t &bt_err
							, const bt_id_t &piecehash)throw() = 0;
	// virtual destructor
	virtual ~bt_piece_cpuhash_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PIECE_CPUHASH_CB_HPP__  */



