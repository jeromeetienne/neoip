/*! \file
    \brief Definition of the unit test for the \ref bt_pieceprec_t

*/

/* system include */
/* local include */
#include "neoip_bt_pieceprec_nunit.hpp"
#include "neoip_bt_pieceprec.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_pieceprec_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test some constansts
	NUNIT_ASSERT( bt_pieceprec_t(bt_pieceprec_t::NOTNEEDED).is_notneeded() );
	NUNIT_ASSERT( bt_pieceprec_t(bt_pieceprec_t::LOWEST).is_lowest() );
	NUNIT_ASSERT( bt_pieceprec_t(bt_pieceprec_t::HIGHEST).is_highest() );
	NUNIT_ASSERT( bt_pieceprec_t(bt_pieceprec_t::DEFAULT).is_default() );
	
	// return no error
	return NUNIT_RES_OK;
}


/** \brief Test function
 */
nunit_res_t	bt_pieceprec_testclass_t::array(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	bt_pieceprec_arr_t	pieceprec_arr;
	// test the bt_pieceprec_arr_t::assign function
	pieceprec_arr.assign(5, bt_pieceprec_t::NOTNEEDED);
	NUNIT_ASSERT( pieceprec_arr[0].is_notneeded() && pieceprec_arr[1].is_notneeded()
				&& pieceprec_arr[2].is_notneeded() && pieceprec_arr[3].is_notneeded()
				&& pieceprec_arr[4].is_notneeded() );

	// test the bt_pieceprec_arr_t::set_range
	pieceprec_arr.set_range( 1, 3, 3, 1);
	NUNIT_ASSERT( pieceprec_arr[0] == 0 && pieceprec_arr[1] == 3 && pieceprec_arr[2] == 2
				&& pieceprec_arr[3] == 1 && pieceprec_arr[4] == 0);
	
	// return no error
	return NUNIT_RES_OK;
}


/** \brief Test function
 */
nunit_res_t	bt_pieceprec_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	bt_pieceprec_arr_t	pieceprec_arr_toserial;
	pieceprec_arr_toserial.assign(5, bt_pieceprec_t::NOTNEEDED);
	pieceprec_arr_toserial.set_range( 1, 3, 3, 1);	
	
	bt_pieceprec_arr_t	pieceprec_arr_unserial;
	serial_t		serial;
	// do the serial/unserial
	serial << pieceprec_arr_toserial;
	serial >> pieceprec_arr_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( pieceprec_arr_toserial == pieceprec_arr_unserial );
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

