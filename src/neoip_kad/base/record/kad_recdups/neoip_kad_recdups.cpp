/*! \file
    \brief Definition of the \ref kad_recdups_t
  
*/

/* system include */
#include <iomanip>
#include <algorithm>

/* local include */
#include "neoip_kad_recdups.hpp"
#include "neoip_rand.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
kad_recdups_t::kad_recdups_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Sort this kad_recdups_t according to the record ID
 */
kad_recdups_t & kad_recdups_t::sort_by_recid()			throw()
{
	std::sort(recval_db.begin(), recval_db.end());
	// return the object itself
	return *this;
}

/** \brief return an iterator on the record with the same recid, or .end() otherwise
 */
std::vector<kad_recdups_t::recval_t>::iterator	kad_recdups_t::get_by_recid(const kad_recid_t &recid)
										throw()
{
	std::vector<recval_t>::iterator	iter;
	// go thru the whole array (not very efficient to say the least)
	for( iter = recval_db.begin(); iter != recval_db.end(); iter++ ){
		// if this element has the same recid, leave the loop
		if( iter->get_recid() == recid )	break;
	}
	// return the iterator
	return iter;	
}

/** \brief insert an kad_rec_t to this kad_recdups_t
 * 
 * - If a record with the same recid already exist, do a tie check
 *   - the record with the largest ttl wins
 */
kad_recdups_t &	kad_recdups_t::update(const kad_rec_t &kad_rec)	throw()
{
	// try to find a record with the same recid
 	std::vector<recval_t>::iterator	iter	= get_by_recid(kad_rec.get_recid());
 	// if there is a record with the same recid, do a tie check
 	// - if the new record to insert has a shorted ttl than the existing one, discard the new
 	if( iter != recval_db.end() ){
 		// get local record
		kad_rec_t	local = kad_rec_t(iter->recid, keyid, iter->ttl, iter->payload);
		// if local record is more 'interesting' than the remote one, discard the remote one
 		if( local.is_tie_winner_against(kad_rec) )	return *this;
 		// if the remote record is more 'interesting', delete the local one
 		recval_db.erase(iter);
 	}
 
	// if keyid isnt yet set, set it up with the first insert()
	if( keyid.is_null() )	keyid = kad_rec.get_keyid();
	// sanity check - the kad_rec's keyid MUST be the same as the kad_recdups_t one
	DBG_ASSERT( keyid == kad_rec.get_keyid() );
	// add the record in the recval_db
	recval_db.push_back( recval_t(kad_rec) );
	// return the object itself
	return *this;	
}


/** \brief insert an kad_recdups_t to this kad_recdups_t;
 */
kad_recdups_t &	kad_recdups_t::update(const kad_recdups_t &other)	throw()
{
	// loop over each record of the other kad_recdups_t and insert it
	for( size_t i = 0; i < other.size(); i++ )
		update(other[i]);
	// return the object itself
	return *this;
}

/** \brief return the record of this idx
 */
kad_rec_t kad_recdups_t::operator[](int idx)	const throw()
{
	// sanity check - the idx MUST be in the alloed range
	DBG_ASSERT( (size_t)idx < recval_db.size() );
	// get the recval_t of this idx
	const recval_t &	recval = recval_db[idx];
	// build the kad_rec_t to return
	return kad_rec_t(recval.recid, keyid, recval.ttl, recval.payload);
}

/** \brief Truncate the nb_rec_to_del records at the tail of the kad_recdups
 */
void	kad_recdups_t::truncate_at_tail(size_t nb_rec_to_del)	throw()
{
	for( size_t i = 0; i < nb_rec_to_del; i++ )
		recval_db.pop_back();
}

/** \brief Randomize the order of this kad_recdups_t
 */
void	kad_recdups_t::randomize_order()			throw()
{
	for( size_t i = 0; i < recval_db.size(); i++ ){
		size_t		idx1	= (size_t)neoip_rand(recval_db.size());
		size_t		idx2	= (size_t)neoip_rand(recval_db.size());
		
		recval_t	tmp	= recval_db[idx1];
		recval_db[idx1]	= recval_db[idx2];
		recval_db[idx2] = tmp;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string kad_recdups_t::to_string()			const throw()
{
	std::vector<kad_recdups_t::recval_t>::const_iterator	iter;
	std::ostringstream	oss;
	oss << "rec id=" << keyid;
	// display the whole list
	for( iter = recval_db.begin(); iter != recval_db.end(); iter++ ){
		const kad_recdups_t::recval_t	&	recval = *iter;
		oss << " payload=" << recval.get_payload() << " ttl=" << recval.get_ttl();
	}
	// return the built string
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial kad_recdups_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the space overhead of a kad_recdups_t without any records
 * 
 * - TODO find a better name for those functions
 */
size_t	kad_recdups_t::get_serial_hd_main()			const throw()
{
	uint32_t	nb_rec = 0;	// = 0 to avoid a compiler warning from the cpp compiler
	kad_keyid_t	keyid;
	serial_t	serial;
	// serialize the number of record and a keyid
	serial << nb_rec;
	serial << keyid;
	// return the result length
	return serial.get_len();
}

/** \brief Return the space overhead of a single record in a kad_recdups_t
 * 
 * - TODO find a better name for those functions
 */
size_t	kad_recdups_t::get_serial_hd_per_rec()	const throw()
{
	kad_keyid_t	keyid;
	datum_t		payload;	// an empty data
	delay_t		ttl;
	serial_t	serial;
	// serialize the number of record and a keyid
	serial << keyid;
	serial << payload;
	serial << ttl;	
	// return the result length
	return serial.get_len();	
}

/** \brief Return the number of records of this kad_recdups_t which would fit in
 *         max_len byte if they were serialized
 */
size_t	kad_recdups_t::nb_serialized_rec_in_size(size_t max_len)	const throw()
{
// compute the number of records which are serializable
	size_t	cur_len	= 0;
	size_t	nb_rec	= 0;
	cur_len	+= get_serial_hd_main();
	// sanity check - the max_len MUST be at least the sizeof 
	DBG_ASSERT( max_len == 0 || cur_len <= max_len );
	// count the number of record to be serialized in amount of data
	for( ; nb_rec < recval_db.size(); nb_rec ++ ){
		// compute the length of this record
		size_t	rec_len = get_serial_hd_per_rec() + recval_db[nb_rec].get_payload().get_len();
		// if this record cause the limit to be exceeded exist
		if( max_len && cur_len + rec_len > max_len )	break;
		// update the current length
		cur_len += rec_len;
	}
	// return the result
	return nb_rec;
}

/** \brief remove elements from the last until the serialization of this kad_recdups_t 
 *         dont exceed max_len
 * 
 * @return the number of truncated elements
 */
size_t	kad_recdups_t::truncate_to_serial_len(size_t max_len)	throw()
{
// compute the number of records which are serializable
	size_t	cur_len	= 0;
	size_t	nb_rec	= 0;
	cur_len	+= get_serial_hd_main();
	// sanity check - the max_len MUST be at least the sizeof 
	DBG_ASSERT( max_len == 0 || cur_len <= max_len );
	// count the number of record to be serialized in amount of data
	for( ; nb_rec < recval_db.size(); nb_rec ++ ){
		// compute the length of this record
		size_t	rec_len = get_serial_hd_per_rec() + recval_db[nb_rec].get_payload().get_len();
		// if this record cause the limit to be exceeded exist
		if( max_len && cur_len + rec_len > max_len )	break;
		// update the current length
		cur_len += rec_len;
	}
// remove the elements at the end
	size_t	nb_to_remove	= recval_db.size() - nb_rec;
	for( size_t i = 0; i < nb_to_remove; i++ )	recval_db.pop_back();
	// return the number of removed elements
	return nb_to_remove;
}


/** \brief serialize a kad_recdups_t
 */
serial_t& operator << (serial_t& serial, const kad_recdups_t &kad_recdups)		throw()
{
	// serialize the number of record
	uint32_t  nb_rec = kad_recdups.size();
	serial << nb_rec;

	// serialize the record id
	serial << kad_recdups.keyid;
	
	// serialize the nodes in the list
	std::vector<kad_recdups_t::recval_t>::const_iterator	iter;
	for( iter = kad_recdups.recval_db.begin(); iter != kad_recdups.recval_db.end(); iter++ ){
		const kad_recdups_t::recval_t	&	recval = *iter;		
		serial << recval.get_recid();
		serial << recval.get_payload();
		serial << recval.get_ttl();
	}
	// return the result
	return serial;
}

/** \brief unserialze a kad_recdups_t
 */
serial_t& operator >> (serial_t& serial, kad_recdups_t &kad_recdups)		throw(serial_except_t)
{
	kad_recid_t	recid;
	kad_keyid_t	keyid;
	datum_t		rec_payload;
	delay_t		rec_ttl;
	uint32_t	nb_rec;
	// reset the kad_recdups
	kad_recdups = kad_recdups_t();
	// unserialize the number of record
	serial >> nb_rec;
	// if there are no record serialized, exit now
	if( nb_rec == 0 )	return serial;
	// unserialize the keyid
	serial >> keyid;
	// unserialize each record
	for( size_t i = 0; i < nb_rec; i++ ){
		serial >> recid;
		serial >> rec_payload;
		serial >> rec_ttl;
		kad_recdups.update(kad_rec_t(recid, keyid, rec_ttl, rec_payload));
	}
	// return the data still unserialized
	return serial;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_recdups_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_recdups_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_recdups_t &kad_recdups)	throw()
{
	// begin the array
	xmlrpc_build << xmlrpc_build_t::ARRAY_BEG;
	// serialize all elements of the kad_recdups_t in the array
	for( size_t i = 0; i < kad_recdups.size(); i++ )
		xmlrpc_build	<< kad_recdups[i];
	// end the array 
	xmlrpc_build << xmlrpc_build_t::ARRAY_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_recdups_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_recdups_t &kad_recdups)	throw(xml_except_t)
{
	// begin the array
	xmlrpc_parse >> xmlrpc_parse_t::ARRAY_BEG;
	// go thru all the element of this array
	while( xmlrpc_parse.has_more_sibling() ){
		kad_rec_t	kad_rec;
		// get a kad_rec_t
		xmlrpc_parse	>> kad_rec;
		// put it in the kad_recdups_t
		kad_recdups.update(kad_rec);
	}
	// end the array
	xmlrpc_parse >> xmlrpc_parse_t::ARRAY_END;
	// return the object itself
	return xmlrpc_parse;
}


NEOIP_NAMESPACE_END;






