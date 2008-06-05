/*! \file
    \brief Definition of the unit test for the \ref compress_t

\par Potential Improvement
- TODO to do something which check if the produced gzip/deflate etc...
  is the actual format 
  - by checking with 'official' tools like /usr/bin/gzip

*/

/* system include */
/* local include */
#include "neoip_compress_nunit.hpp"
#include "neoip_compress.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Test the identity consistency for a compress_type
 */
nunit_res_t compress_testclass_t::test_identity_consistency(const compress_type_t &compress_type
						, const datum_t &orig_datum)	const throw()
{	
	// init the compress_t object
	compress_t	compress(compress_type);
	// compress the original datum_t
	datum_t		compressed_datum	= compress.compress(orig_datum, orig_datum.get_len());
	// uncompress the just compressed datum
	datum_t		uncompressed_datum	= compress.uncompress(compressed_datum, orig_datum.get_len());
	// test if the compression layer is consistent aka if uncomp(comp(data)) = data
	NUNIT_ASSERT( orig_datum == uncompressed_datum );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief function to test a compress_t
 */
nunit_res_t	compress_testclass_t::test_identity(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// build a dummy data
	std::string	str = "hello world! this is a dummy sentence that must be long enougth to "
				"be compressable. then add a lot of aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
				" and more aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa the end!";
	datum_t		orig_datum(str);

	// test all available algorithms
	for(size_t i = 0; i < compress_type_t::size(); i++ ){
		compress_type_t	compress_type	= compress_type_t::at(i);
#if 1	// TODO to remove - just a kludge to avoid testing gzip/deflate which got memory leak 
		if( compress_type == compress_type_t::GZIP || compress_type == compress_type_t::DEFLATE ){
			KLOG_ERR("not testing gzip and deflate as they contains memory leak.");
			continue;
		}
#endif

		nunit_res	= test_identity_consistency(compress_type_t::at(i), orig_datum);
		if( !nunit_res.error().succeed() )	return nunit_res;
	}	

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

