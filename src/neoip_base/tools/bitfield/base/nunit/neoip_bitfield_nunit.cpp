/*! \file
    \brief Definition of the unit test for the \ref bitfield_t

*/

/* system include */
/* local include */
#include "neoip_bitfield_nunit.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::is_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test is_null
	NUNIT_ASSERT(  bitfield_t().is_null() );
	NUNIT_ASSERT( !bitfield_t(42).is_null() );
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::to_canonical_string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test to_canonical_tring
	NUNIT_ASSERT( bitfield_t(4).set(1, true).set(3, true).to_canonical_string() == "0101");
	NUNIT_ASSERT( bitfield_t(4).to_canonical_string() == "0000");
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::comparison(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bitfield_t	bitfield1	= bitfield_t(42).set(1, true).set(3, true);
	bitfield_t	bitfield2	= bitfield_t(42).set(2, true).set(4, true);
	// test various comparison
	NUNIT_ASSERT( bitfield1 == bitfield1 );
	NUNIT_ASSERT( bitfield1 <= bitfield1 );
	NUNIT_ASSERT( bitfield1 >= bitfield1 );
	NUNIT_ASSERT( bitfield1 != bitfield2 );
	NUNIT_ASSERT( bitfield1 >  bitfield2 );
	NUNIT_ASSERT( bitfield2 <  bitfield1 );
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::get_next_set_unset(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bitfield_t	bitfield1	= build_random(42);
	bitfield_t	bitfield2;
	
	// copy bitfield1 to bitfield2 using the bitfield_t.get_next_set
	bitfield2	= bitfield_t(bitfield1.size());
	for(size_t idx = bitfield1.get_next_set(0); idx < bitfield1.size(); idx = bitfield1.get_next_set(idx+1)){
		bitfield2.set(idx, true);
	}
	// check that the copy is equal
	NUNIT_ASSERT( bitfield1 == bitfield2 );

	// count the number of unset bit
	size_t	nb_unset	= 0;
	for(size_t idx = bitfield1.get_next_unset(0); idx < bitfield1.size(); idx = bitfield1.get_next_unset(idx+1))
		nb_unset++;
	// check that the number of bit set + number of bit unset is equal to the total number of bits
	NUNIT_ASSERT( bitfield1.nb_set() + nb_unset == bitfield1.size());
		
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::bool_operation(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bitfield_t	bitfield1	= bitfield_t(2).set(0, true).set(1, true);
	bitfield_t	bitfield2	= bitfield_t(2).set(0,false).set(1, true);

	// test the AND operator
	NUNIT_ASSERT( (bitfield1 & bitfield2) == bitfield_t(2).set(1, true) );
	
	// test the OR operator
	NUNIT_ASSERT( (bitfield1 | bitfield2) == bitfield_t(2).set(0, true).set(1, true) );
	
	// test the XOR operator
	NUNIT_ASSERT( (bitfield1 ^ bitfield2) == bitfield_t(2).set(0, true).set(1, false) );

	// test the NOT operator
	NUNIT_ASSERT( ~bitfield2 == bitfield_t(2).set(0, true).set(1, false) );

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitfield_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bitfield_t	bitfield;
	// test serialization of a null
	NUNIT_ASSERT( is_serial_consistent(bitfield) );
	// build a random one
	bitfield	= build_random(42);
	// test serialization of a nonnull one
	NUNIT_ASSERT( is_serial_consistent(bitfield) );
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this bitfield_t is serial consistent
 */
bool	bitfield_testclass_t::is_serial_consistent(const bitfield_t &bitfield)	const throw()
{
	// init some variable
	bitfield_t	bitfield_toserial = bitfield;
	bitfield_t	bitfield_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << bitfield_toserial;
	serial >> bitfield_unserial;
	// return the result
	return bitfield_toserial == bitfield_unserial;	
}

/** \brief Build a random bitfield_t
 */
bitfield_t bitfield_testclass_t::build_random(size_t nb_bit)		const throw()
{
	bitfield_t	bitfield(nb_bit);
	// take nb_bit/2 random idx and set the bit at thos position
	for(size_t i = 0; i < bitfield.size()/2; i++)
		bitfield.set( rand() % bitfield.size(), true);
	// return the just built bitfield_t
	return bitfield;
}


NEOIP_NAMESPACE_END

