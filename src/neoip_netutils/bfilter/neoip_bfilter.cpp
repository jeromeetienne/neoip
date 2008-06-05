/*! \file
    \brief Implementation of \ref bfilter_t

\par Implementation Notes
- this is a first attempts at bloom filter. it seems to work. but clearly
  far for quality product

*/

/* system include */
#include <cmath>
/* local include */
#include "neoip_bfilter.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor for the class
 */
bfilter_t::bfilter_t()	throw()
{
}

/** \brief Constructor from bfilter_param_t
 */
bfilter_t::bfilter_t(const bfilter_param_t &param)	throw()
{
	// copy the parameters
	filter_width	= param.filter_width();
	nb_hash		= param.nb_hash();
	// allocate the filter_arr
	filter_arr.reserve(filter_width);
	// fill the filter_arr with false
	for(size_t i = 0; i < filter_width; i++ )	filter_arr.push_back(false);
}

/** \brief Destructor for the class
 */
bfilter_t::~bfilter_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           Element function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

datum_t	bfilter_t::build_key(const datum_t &elem)	const throw()
{
	skey_auth_t	skey_auth("sha1/nokey/20");
	// init the 'key'
	bool	failed = skey_auth.init_key(NULL, 0);
	DBG_ASSERT( !failed );
	// compute the hash over the element
	skey_auth.update( elem.get_data(), elem.get_len() );
	// return the result
	return skey_auth.get_output();
}

/** \brief Extract the bit_idx of the key for the hash_idx
 */
size_t	bfilter_t::bit_idx_from_hash_idx(const datum_t &key, size_t hash_idx)	const throw()
{
	// compute the number of byte to extract per hash - in a dirty way
	int	byte_per_hash	= -1;
	if( filter_width <= 255 )	byte_per_hash	= 1;
	else if(filter_width <= 65535)	byte_per_hash	= 2;
	else				DBG_ASSERT( 0 );
	
	uint8_t	*p		= (uint8_t *)key.get_data() + byte_per_hash * hash_idx;
	int	bit_idx		= 0;
	// sanity check
	KLOG_DBG("byte_per_hash=" << byte_per_hash);
	KLOG_DBG("hash_idx=" << hash_idx);
	KLOG_DBG("key len=" << key.get_len());
	DBG_ASSERT( byte_per_hash * (hash_idx+1) < key.get_len() );
	// compute the bit index
	while( byte_per_hash-- ){
		bit_idx	<<= 8;
		bit_idx	+= *p;
		p++;
	}
	// log to debug
	KLOG_DBG("key=" << key.to_string() << " hash_idx=" << hash_idx
				<< " bit_idx before clamping=" << std::hex << bit_idx
				<< " bit_idx after clamping=" << std::dec << (bit_idx % filter_width));
	// return the bit index clamped by the filter_width
	return bit_idx % filter_width;
}

/** \brief Add an element to the bloom filter
 */
bfilter_t &	bfilter_t::insert(const datum_t &elem)		throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( !is_null() );
	// build the key
	datum_t	key	= build_key(elem);
	// set the bits for all the hash_idx
	for(size_t hash_idx = 0; hash_idx < nb_hash; hash_idx++ ){
		// compute the bit_idx fot this hash_idx
		int	bit_idx = bit_idx_from_hash_idx(key, hash_idx);
		// set the bit_idx in the filter_arr
		filter_arr[bit_idx]	= true;
	}
	// return the object itself
	return *this;
}


/** \brief Test if an element is contained in the bloom filter
 */
bool	bfilter_t::contain(const datum_t &elem)	const throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( !is_null() );
	// build the key
	datum_t	key	= build_key(elem);
	// set the bits for all the hash_idx
	for(size_t hash_idx = 0; hash_idx < nb_hash; hash_idx++ ){
		// compute the bit_idx fot this hash_idx
		int	bit_idx = bit_idx_from_hash_idx(key, hash_idx);
		// if this bit is not set, the element is NOT contained
		if( filter_arr[bit_idx]	== false )	return false;
	}
	// if the code reachs this point, the element is contained
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	bfilter_t::compare(const bfilter_t &other)		const throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( filter_width == other.filter_width && nb_hash == other.nb_hash );

	// compare the filter array
	if( filter_arr < other.filter_arr )	return -1;
	if( filter_arr > other.filter_arr )	return +1;
	
	// NOTE: both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//             inter-bfilter_t operation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Unify between 2 bfilter_t
 */
bfilter_t	bfilter_t::unify(const bfilter_t & other)	const throw()
{
	bfilter_t	result(bfilter_param_t().filter_width(filter_width).nb_hash(nb_hash));
	// sanity check - they both have the same parameters
	DBG_ASSERT( filter_width == other.filter_width && nb_hash == other.nb_hash );
	// do a bitwise OR on the filter_arr
	for(size_t i = 0; i < filter_width; i++ )
		result.filter_arr[i] = filter_arr[i] || other.filter_arr[i];
	// return the result
	return result;
}

/** \brief Intersect between 2 bfilter_t
 */
bfilter_t	bfilter_t::intersect(const bfilter_t & other)	const throw()
{
	bfilter_t	result(bfilter_param_t().filter_width(filter_width).nb_hash(nb_hash));
	// sanity check - they both have the same parameters
	DBG_ASSERT( filter_width == other.filter_width && nb_hash == other.nb_hash );
	// do a bitwise AND on the filter_arr
	for(size_t i = 0; i < filter_width; i++ )
		result.filter_arr[i] = filter_arr[i] && other.filter_arr[i];
	// return the result
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//             static function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compute the false positive rate
 * 
 * - NOTE: this is a static function which is intended to be used to tune the 
 *         bfilter_t parameter
 * - use the formula: false positive rate = ( 1 - e(-nb_hash*nb_inserted_key/filter_width) ) ^ nb_hash
 */
double	bfilter_t::cpu_false_positive_rate(size_t filter_width, size_t nb_hash
						, size_t nb_inserted_key)	throw()
{
	double	tmp	= 1.0 - exp( -1.0 * nb_hash * nb_inserted_key / filter_width );
	return pow(tmp, nb_hash);
}

/** \brief Compute the optimal parameter for a given desired error rate and a number of key
 * 
 * - TODO unclear it is providing good result...
 */
bfilter_param_t	bfilter_t::cpu_optimal_param(double desired_error_rate, size_t nb_inserted_key)	throw()
{
	size_t	lowest_width	= 999999;	// fake number to be sure to be higher than actual one
	size_t	best_nb_hash	= 999999;
	// go thru all the reasonable nb_hash
	for( size_t nb_hash = 1; nb_hash < 100; nb_hash++ ){
		// compute the width required for this nb_hash
		double width	= ( -1.0 * nb_hash * nb_inserted_key )
					/ log( 1.0 - pow(desired_error_rate, 1.0 / nb_hash) );
		// if the width for this nb_hash is less than the previous one, save it
		if( lowest_width > width ){
			lowest_width	= (size_t)width;
			best_nb_hash	= nb_hash;
		}
	}
	// return a bfilter_param_t containing the result
	return bfilter_param_t().filter_width(lowest_width).nb_hash(best_nb_hash);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bfilter_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "filter_width=" << filter_width;
	oss << " ";
	oss << "nb_hash=" << nb_hash;
	oss << " ";
	oss << "filter=";
	for(size_t i = 0; i < filter_width; i++ )	oss << filter_arr[i];
	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END



