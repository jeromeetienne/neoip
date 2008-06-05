/*! \file
    \brief Definition of the unit test for the \ref delaygen_t

*/

/* system include */
/* local include */
#include "neoip_delaygen_nunit.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delaygen_arg_t
 */
nunit_res_t	delaygen_testclass_t::delaygen_arg(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// set it to null
	delaygen_arg_t	delaygen_arg;
	NUNIT_ASSERT( delaygen_arg.is_null() );
	
	// set it to a regular
	delaygen_arg	= delaygen_regular_arg_t().period(delay_t::from_sec(3));
	NUNIT_ASSERT( !delaygen_arg.is_null() );
	NUNIT_ASSERT(  delaygen_arg.is_regular() );
	NUNIT_ASSERT( !delaygen_arg.is_expboff() );

	// set it to a expboff
	delaygen_arg	= delaygen_expboff_arg_t().min_delay(delay_t::from_sec(1))
							.max_delay(delay_t::from_sec(4));
	NUNIT_ASSERT( !delaygen_arg.is_null() );
	NUNIT_ASSERT( !delaygen_arg.is_regular() );
	NUNIT_ASSERT(  delaygen_arg.is_expboff() );

	// test the copy operator
	delaygen_arg_t	delaygen_arg2(delaygen_arg);

	// test the assignement operator
	delaygen_arg2	= delaygen_arg;

	// set it back to null
	delaygen_arg	= delaygen_arg_t();
	NUNIT_ASSERT( delaygen_arg.is_null() );
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delaygen_t
 */
nunit_res_t	delaygen_testclass_t::regular_nonrandom(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delaygen_arg_t	delaygen_arg	= delaygen_regular_arg_t().first_delay(delay_t::from_sec(10))
								.period(delay_t::from_sec(3))
								.timeout_delay(delay_t::from_sec(17));
	delaygen_t	delaygen	= delaygen_t(delaygen_arg);
	
	// check the first delay
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(10) );

	// notify the expiration of the first delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(10) );	
	delaygen++;

	// check the second delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(3) );

	// notify the expiration of the second delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(13) );	
	delaygen++;

	// check the third delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(3) );

	// notify the expiration of the third delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(16) );	
	delaygen++;

	// check the forth delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(1) );

	// notify the expiration of the forth delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(17) );	
	NUNIT_ASSERT( delaygen.is_timedout() == true );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delaygen_t
 */
nunit_res_t	delaygen_testclass_t::regular_random(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delaygen_arg_t	delaygen_arg	= delaygen_regular_arg_t().first_delay(delay_t::from_sec(10))
								.period(delay_t::from_sec(5))
								.timeout_delay(delay_t::from_sec(20))
								.random_range(0.2);
	delaygen_t	delaygen;
	delay_t		delay;
	size_t		i, nb_try	= 20;

	// test the first delay is always in the proper range	
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delay		= delaygen.current();
		NUNIT_ASSERT( delay >= delay_t::from_sec(8) && delay <= delay_t::from_sec(12));
	}
	// test that first delay is sometime different from the base value
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		if( delaygen.current() != delay_t::from_sec(10) )	break;
	}
	NUNIT_ASSERT( i != nb_try );

	// test the period delay is always in the proper range	
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delaygen++;	// skip the first delay
		delay		= delaygen.current();
		NUNIT_ASSERT( delay >= delay_t::from_sec(4) && delay <= delay_t::from_sec(6));
	}
	
	// test that period delay is sometime different from the base value
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delaygen++;	// skip the first delay
		if( delaygen.current() != delay_t::from_sec(5) )	break;
	}
	NUNIT_ASSERT( i != nb_try );

	// test if the timeout is always the same nomatter the random
	for(size_t i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		while( !delaygen.is_timedout() ){
			delaygen++;
			delaygen.notify_expiration();
		}
		NUNIT_ASSERT( delaygen.elapsed_delay() == delay_t::from_sec(20) );
	}

	// return no error
	return NUNIT_RES_OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delaygen_t
 */
nunit_res_t	delaygen_testclass_t::expboff_nonrandom(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delaygen_arg_t	delaygen_arg	= delaygen_expboff_arg_t().first_delay(delay_t::from_sec(5))
								.min_delay(delay_t::from_sec(2))
								.max_delay(delay_t::from_sec(6))
								.timeout_delay(delay_t::from_sec(18));
	delaygen_t	delaygen	= delaygen_t(delaygen_arg);

	// check the first delay
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(5) );

	// notify the expiration of the first delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(5) );	
	delaygen++;

	// check the second delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(2) );

	// notify the expiration of the second delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(7) );
	delaygen++;

	// check the third delay
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(4) );

	// notify the expiration of the third delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(11) );	
	delaygen++;

	// check the forth delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(6) );

	// notify the expiration of the forth delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() == false );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(17) );	
	delaygen++;

	// check the fifth delay	
	NUNIT_ASSERT( delaygen.current() == delay_t::from_sec(1) );

	// notify the expiration of the forth delay
	delaygen.notify_expiration();
	NUNIT_ASSERT( delaygen.is_timedout() );
	NUNIT_ASSERT( delaygen.get_elapsed_delay() == delay_t::from_sec(18) );	
	
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delaygen_t
 */
nunit_res_t	delaygen_testclass_t::expboff_random(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delaygen_arg_t	delaygen_arg	= delaygen_expboff_arg_t().first_delay(delay_t::from_sec(10))
								.min_delay(delay_t::from_sec(5))
								.max_delay(delay_t::from_sec(6))
								.timeout_delay(delay_t::from_sec(23))
								.random_range(0.2);
	delaygen_t	delaygen;
	delay_t		delay;
	size_t		i, nb_try	= 20;

	// test the first delay is always in the proper range	
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delay		= delaygen.current();
		NUNIT_ASSERT( delay >= delay_t::from_sec(8) && delay <= delay_t::from_sec(12));
	}

	// test that first delay is sometime different from the base value
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		if( delaygen.current() != delay_t::from_sec(10) )	break;
	}
	NUNIT_ASSERT( i != nb_try );

	// test the min_delay is always in the proper range	
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delaygen++;	// skip the first delay
		delay		= delaygen.current();
		NUNIT_ASSERT( delay >= delay_t::from_sec(4) && delay <= delay_t::from_sec(6) );
	}

	// test that min_delay is sometime different from the base value
	for(i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		delaygen++;	// skip the first delay
		if( delaygen.current() != delay_t::from_sec(5) )	break;
	}
	NUNIT_ASSERT( i != nb_try );

	// test if the timeout is always the same nomatter the random
	for(size_t i = 0; i < nb_try; i++){
		delaygen	= delaygen_t(delaygen_arg);
		while( !delaygen.is_timedout() ){
			delaygen++;
			delaygen.notify_expiration();
		}
		NUNIT_ASSERT( delaygen.elapsed_delay() == delay_t::from_sec(23) );
	}

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

