/*! \file
    \brief Definition of the \ref bt_mfile_t

- TODO maybe a do_store/no_store for each subfile
  - would allow to select the file you want to be written
  - maybe something more flexible to say isavail/unavail to show which file
    is locally present or not.
  - it would be used in bt_io_sfile_t to determine how to store the files
    - issue when downloading a whole piece containing data which are not 'storage'
- TODO pass piecelen as file_size_t to avoid convertion error and have the is_null ?

*/

/* system include */
#include <iomanip>

/* local include */
#include "neoip_bt_mfile.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


#define COMMAND_FOR_EACH_FIELD( COMMAND )			\
	COMMAND( bt_id_t		, infohash);		\
	COMMAND( size_t			, piecelen);		\
	COMMAND( bt_mfile_subfile_arr_t	, subfile_arr);		\
	COMMAND( http_uri_t		, announce_uri);	\
	COMMAND( file_path_t		, name);		\
	COMMAND( bt_id_arr_t		, piecehash_arr);	\
	COMMAND( bool			, no_external_peersrc);	\
	COMMAND( bool			, do_piecehash);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
bt_mfile_t::bt_mfile_t()		throw()
{
	// set the default values
	do_piecehash		(true);
	no_external_peersrc	(false);
	// just a set to increase the error detection in the is_null()
	piecelen( 0 );
	// clear the precomputed fields
	// - it helps the sanity check to ensure they are not accessed without being init
	nb_piece_precpu		= std::numeric_limits<size_t>::max();
}

/** \brief Destructor
 */
bt_mfile_t::~bt_mfile_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the object is to be considered null, false otherwise
 * 
 * - check if all the required field are set
 *   - aka the field needed for is_sfile_ok AND is_pfile_ok
 *   - dont check the optionnal field
 */
bool	bt_mfile_t::is_null()	const throw()
{
	// check that piecelen is set
	if( piecelen() == 0 )			return true;
	// check that the infohash is set
	if( infohash().is_null() )		return true;
	// check that there is at least one bt_mfile_subfile_t
	if( subfile_arr().size() == 0 )		return true;
	// check that all bt_mfile_subfile_t are not null
	for(size_t i = 0; i < subfile_arr().size(); i++){
		const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[i];
		// if this mfile_subfile is null, return true
		if( mfile_subfile.is_null() )	return true;
	}	
	// if this point is reached, the bt_mfile_t is to be considered non null
	return false;
}

/** \brief return true if the bt_mfile_t is_full_init
 */
bool	bt_mfile_t::is_fully_init()					const throw()
{
	// if it is null, return false
	if( is_null() )							return false;
	// if the infohash is not computed, return false
	if( infohash().is_null() )					return false;
	// if the totfile_size is not computed, return false
	if( totfile_size_precpu.is_null() )				return false;
	// if the nb_piece is not computed, return false
	if( nb_piece_precpu == std::numeric_limits<size_t>::max() )	return false;
	// check that all bt_mfile_subfile_t are fully_init
	for(size_t i = 0; i < subfile_arr().size(); i++){
		const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[i];
		// if this mfile_subfile is not fully init, return false
		if( !mfile_subfile.is_fully_init() )			return false;
	}	
	// if do_piecehash is true, the piecehash_arr MUST have nb_piece() hash
	if( do_piecehash() && piecehash_arr().size() != nb_piece() )	return false;
	// return true - if all the previous tests passed, it is considered fully initialized
	return true;
}

/** \brief Return true if the bt_mfile_t is ok for a bt_io_sfile_t usage
 * 
 * - TODO this function is likely not to contained all the required tests
 */
bool	bt_mfile_t::is_sfile_ok()			const throw()
{
	// if the bt_mfile_t is not fully init, return false
	if( !is_fully_init() )				return false;
	// check that all bt_mfile_subfile_t have a local_path()
	for(size_t i = 0; i < subfile_arr().size(); i++){
		const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[i];
		// if this mfile_subfile is not disk_ok(), return false
		if( mfile_subfile.is_sfile_ok() )	return false;
	}	
	// if all previous tests passes, return true
	return true;
}

/** \brief Return true if the bt_mfile_t is ok for a bt_io_sfile_t usage
 * 
 * - TODO this function is likely not to contained all the required tests
 */
bool	bt_mfile_t::is_pfile_ok()			const throw()
{
	// if the bt_mfile_t is not fully init, return false
	if( !is_fully_init() )				return false;
	// check that all bt_mfile_subfile_t have a local_path()
	for(size_t i = 0; i < subfile_arr().size(); i++){
		const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[i];
		// if this mfile_subfile is not disk_ok(), return false
		if( mfile_subfile.is_pfile_ok() )	return false;
	}	
	// if all previous tests passes, return true
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Complete the initialization
 * 
 * - it may be done multiple times on a given bt_mfile_t
 *   - this case occurs when a bt_mfile_t is complete_init(), then it is updated
 *     then another complete_init() is done
 */
bt_mfile_t &	bt_mfile_t::complete_init()			throw()
{
	// sanity check - the bt_mfile_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	
	// compute the totfile_size
	totfile_size_precpu	= 0;
	for(size_t i = 0; i < subfile_arr().size(); i++)
		totfile_size_precpu+=  subfile_arr()[i].len();

	// compute the amount of piece needed
	file_size_t	nb_piece = ceil_div(totfile_size(), file_size_t(piecelen()));
	nb_piece_precpu	= nb_piece.to_size_t();

	// compute the infohash IIF it is not yet set
	// - this allows to set the infohash to some custom values aka not directly
	//   derived from the info map. e.g. infohash = hash(http_uri_t)
	if( infohash().is_null() ){
		dvar_t	mfile_dvar	= bencode_t::to_dvar(to_bencode());
		infohash 		(bencode_t::from_dvar(mfile_dvar.map()["info"]));
	}

	// reset the subfile_db - usefull if the subfile_db was already computed
	subfile_db	= std::map<file_size_t, size_t>();

	// compute the totfile_beg for each bt_mfile_subfile_t which a length > 0
	file_size_t	totfile_beg_cur	= 0;
	for(size_t subfile_idx = 0; subfile_idx < subfile_arr().size(); subfile_idx++){
		bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[subfile_idx];
		// if the length is 0, skip it
		if( mfile_subfile.len() == 0 )		continue;
		// set the totfile_beg_precpu in this 
		file_range_t	totfile_range;
		totfile_range	= file_range_t::from_offlen(totfile_beg_cur, mfile_subfile.len());
		mfile_subfile.totfile_range_precpu	= totfile_range;
		// add this mfile_subfile in the subfile_db
		bool	succeed	= subfile_db.insert(std::make_pair(totfile_beg_cur, subfile_idx)).second;
		DBG_ASSERT( succeed );
		// update the totfile_beg_cur with the length of this bt_mfile_subfile_t
		totfile_beg_cur	+= mfile_subfile.len();
	}

	// sanity check - now the bt_mfile_t MUST be fully init
	DBG_ASSERT( is_fully_init() );

	// return the object itself
	return *this;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     get_iov_arr
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the bt_iov_arr_t matching a totfile_range
 * 
 * - the resulting bt_iov_arr_t is reduced and ordered
 *   - ordered = the bt_iov_t for the begining of the totfile are at the begining of the array
 *   - reduced = all bt_iov_t are distinct and not contiguous
 * - it doesnt include any subfile which is of 0length
 */
bt_iov_arr_t	bt_mfile_t::get_iov_arr(const file_range_t &totfile_range)	const throw()
{	
	std::map<file_size_t, size_t>::const_iterator	iter;
	bt_iov_arr_t					iov_arr;
	// sanity check - the bt_mfile_t MUST be fully initialized
	DBG_ASSERT( is_fully_init() );
	// if the subfile_db is empty, return an empty bt_iov_arr_t
	// - this may happen if all the subfile are of 0length
	if( subfile_db.empty() )	return bt_iov_arr_t();
	// get the first element with a totfile_beg >= than the totfile_range.beg
	iter	= subfile_db.lower_bound(totfile_range.beg());
	if( iter != subfile_db.begin() )	iter--;
	// go thru the subfile_db to build a list of all the bt_iov_t which included in the totfile_range
	while( iter != subfile_db.end() ){
		size_t				subfile_idx	= iter->second;
		const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[subfile_idx];
		// update the iterator
		iter++;
		// if this mfile_subfile is strictly greater than the totfile_range, leave the loop
		if( mfile_subfile.totfile_beg() > totfile_range.end() )		break;
		// if this mfile_subfile is strictly less than the totfile_range, goto the next
		if( mfile_subfile.totfile_end() < totfile_range.beg() )		continue;
		// compute the subfile_range to include
		// - i got the iov totfile_range and the wanted totfile_range
		// - it is the iov totfile_range clamped by the wanted file_range
		// - then shifted to be expressed as a subfile_range
		file_range_t	subfile_range	= mfile_subfile.totfile_range();
		subfile_range.clamped_by( totfile_range );
		// sanity check - stuff because it is a rather complex function
		DBG_ASSERT( !subfile_range.is_null() );
		DBG_ASSERT( subfile_range.beg() >= mfile_subfile.totfile_beg() );
		DBG_ASSERT( subfile_range.end() >= mfile_subfile.totfile_beg() );
		// shift the subfile_range to be expressed within the subfile (and not totfile)
		subfile_range.beg	( subfile_range.beg() - mfile_subfile.totfile_beg() );
		subfile_range.end	( subfile_range.end() - mfile_subfile.totfile_beg() );
		// add the bt_iov_t to the iov_arr
		iov_arr	+= bt_iov_t(subfile_idx, subfile_range);
	}
	// sanity check - the result MUST ordered and reduced
	DBG_ASSERT( bt_iov_arr_t(iov_arr).sort() == iov_arr );
	DBG_ASSERT( bt_iov_arr_t(iov_arr).reduce() == iov_arr );
	// return the result
	return iov_arr;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Misc function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a global local directory name to all file in the bt_mfile_subfile_arr_t
 * 
 * - This function is used to set a global dirname for all the local_path
 * - the directory tree of the bt_mfile_t is kept untouched
 */
bt_mfile_t &	bt_mfile_t::set_file_local_dir(const file_path_t &dirname)	throw()
{
	// go thru the whole subfile_arr
	for(size_t i = 0; i < subfile_arr().size(); i++){
		bt_mfile_subfile_t &mfile_subfile	= subfile_arr()[i];
		// sanity check - the local_path MUST be null
		DBG_ASSERT( mfile_subfile.local_path().is_null() );
		// build the local path
		file_path_t	local_path	= dirname;
		// if this is a multi-file bt_mfile_t, the name() is the top most directory name
		if( subfile_arr().size() > 1 )	local_path	/= name();
		// add the mfile_path
		local_path	/= mfile_subfile.mfile_path();
		// set the local_path in the mfile_subfile
		mfile_subfile.local_path( local_path );
	}
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         bencode convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_mfile_t from a bencoded datum_t
 */
bt_mfile_t	bt_mfile_t::from_bencode(const datum_t &bencoded_mfile)	throw()
{
	dvar_t		mfile_dvar	= bencode_t::to_dvar(bencoded_mfile);
	bt_mfile_t	bt_mfile;
	// sanity check - if the mfile_dvar conversion failed, return a null bt_mfile_t
	if( mfile_dvar.is_null() )			return bt_mfile_t();
	// sanity check - mfile_dvar MUST be a dvar_type_t::MAP
	if( mfile_dvar.type() != dvar_type_t::MAP )	return bt_mfile_t();
	// sanity check - check that the bencode convertion for dvar is consistent
	DBG_ASSERT( datum_t(bencode_t::from_dvar(mfile_dvar)) == bencoded_mfile );

	// get the announce_uri
	if( !mfile_dvar.map().contain("announce", dvar_type_t::STRING) )	return bt_mfile_t();
	bt_mfile.announce_uri	( mfile_dvar.map()["announce"].str().get() );

	// extract the info dictionary out of it
	if( !mfile_dvar.map().contain("info", dvar_type_t::MAP) )		return bt_mfile_t();
	const dvar_map_t &info_dvar	= mfile_dvar.map()["info"].map();

	// compute the infohash from the incoming mfile_dvar
	// - NOTE: This allows to get the proper infohash, even if the bt_mfile_t::to_bencode()
	//   doesnt handle all the fields from the original file.
	bt_mfile.infohash	(bt_id_t(datum_t(bencode_t::from_dvar(mfile_dvar.map()["info"]))));


	// get the name
	if( !info_dvar.contain("name", dvar_type_t::STRING) )			return bt_mfile_t();
	bt_mfile.name		( info_dvar["name"].str().get() );
	// get the piece length
	if( !info_dvar.contain("piece length", dvar_type_t::INTEGER) )		return bt_mfile_t();
	bt_mfile.piecelen	( info_dvar["piece length"].integer().get() );

	// get "private" field if it is present, or set it to false
	if( info_dvar.contain("private", dvar_type_t::INTEGER) ){
		bt_mfile.no_external_peersrc( info_dvar["private"].integer().get() );
	}else{
		bt_mfile.no_external_peersrc( false );
	}
	// convert the piecehash_arr
	if( !info_dvar.contain("pieces", dvar_type_t::STRING) )			return bt_mfile_t();
	std::string	pieces_str	= info_dvar["pieces"].str().get();
	for(size_t i = 0; i < pieces_str.size() / bt_id_t::size(); i++){
		std::string	str		= pieces_str.substr(i * bt_id_t::size(), bt_id_t::size());
		bt_id_t		piecehash	= bt_id_t(datum_t(str));
		bt_mfile.piecehash_arr()	+= piecehash;
	}
	DBG_ASSERT( bt_mfile.piecehash_arr().size() == pieces_str.size() / bt_id_t::size() );

	// if the info_dvar contains the key 'length', it is a single file bt_mfile_t
	if( info_dvar.contain("length") ){
		if( !info_dvar.contain("length", dvar_type_t::INTEGER) )	return bt_mfile_t();
		file_size_t	file_len	= info_dvar["length"].integer().get();
		bt_mfile.subfile_arr() += bt_mfile_subfile_t(bt_mfile.name(), file_len);
	}else{
		// NOTE: here it is a multiple file bt_mfile_t
		if( !info_dvar.contain("files", dvar_type_t::ARRAY) )		return bt_mfile_t();
		const dvar_arr_t & files_dvar	= info_dvar["files"].arr();
		for(size_t i = 0; i < files_dvar.size(); i++ ){
			if( files_dvar[i].type() != dvar_type_t::MAP )		return bt_mfile_t();
			const dvar_map_t & item_dvar	= files_dvar[i].map();
			if( !item_dvar.contain("path", dvar_type_t::ARRAY) )	return bt_mfile_t();
			if( !item_dvar.contain("length", dvar_type_t::INTEGER))	return bt_mfile_t();	
			const dvar_arr_t & path_dvar	= item_dvar["path"].arr();
	
			file_size_t	file_len	= item_dvar["length"].integer().get();
			file_path_t	file_path;
			for(size_t j = 0; j < path_dvar.size(); j++){
				if( path_dvar[j].type() != dvar_type_t::STRING)	return bt_mfile_t();
				file_path	/= path_dvar[j].str().get();
			}
			bt_mfile.subfile_arr() += bt_mfile_subfile_t(file_path, file_len);
		}
		DBG_ASSERT( bt_mfile.subfile_arr().size() == files_dvar.size() );
	}
	// return the result
	return bt_mfile;
}

/** \brief Convert this bt_mfile_t into bencode format
 * 
 * - TODO to test this function
 * - TODO check if the fields required for a .torrent are present
 *   - do a is_torrent_ok() function
 */
datum_t	bt_mfile_t::to_bencode()					const throw()
{
	// sanity check - the bt_mfile_t MUST be fully_init
	DBG_ASSERT( is_fully_init() );	
	// create the initial mfile_dvar_t
	dvar_t	mfile_dvar	= dvar_map_t();
	
	// insert the announce field
	mfile_dvar.map().insert("announce", dvar_str_t(announce_uri().to_string()));
	// insert the info field
	mfile_dvar.map().insert("info", dvar_map_t());
	// create an alias on the info field
	dvar_map_t &info_dvar	= mfile_dvar.map()["info"].map();

	// put the 'piece length' in the info
	info_dvar.insert("piece length", dvar_int_t(piecelen()));

	// put the "private" field IFF no_external_peersrc is not false
	if( no_external_peersrc() )	info_dvar.insert("private", dvar_int_t(1));

	// build the pieces string
	std::string	pieces_str;
	for(size_t i = 0; i < piecehash_arr().size(); i++){
		datum_t	datum	= piecehash_arr()[i].to_datum();
		pieces_str	+= std::string((char *)datum.get_ptr(), datum.get_len());
	}
	DBG_ASSERT( piecehash_arr().size() == pieces_str.size() / bt_id_t::size() );
	// insert the piece_str as info.pieces
	info_dvar.insert("pieces", dvar_str_t(pieces_str));

	// put the 'name' in the info
	info_dvar.insert("name", dvar_str_t(name().to_string()));

	// if this is a 'single-file' bt_mfile_t
	if( subfile_arr().size() == 1 ){
		// put the 'length' directly in the info - only for single-file bt_mfile_t
		info_dvar.insert("length", dvar_int_t(subfile_arr()[0].len().to_int64()));
	}else{
		// insert the info.files field
		info_dvar.insert("files", dvar_arr_t());	
		// create an alias on the info.files field	
		dvar_arr_t &	files_dvar	= info_dvar["files"].arr();
		for(size_t i = 0; i < subfile_arr().size(); i++ ){
			const bt_mfile_subfile_t &	mfile_subfile	= subfile_arr()[i];
			// build the path_dvar
			dvar_arr_t	path_dvar;
			for(size_t j = 0; j < mfile_subfile.mfile_path().size(); j++)
				path_dvar += dvar_str_t(mfile_subfile.mfile_path()[i].to_string());
			// build the item_dvar
			dvar_map_t	item_dvar;
			item_dvar.insert("length", dvar_int_t(mfile_subfile.len().to_int64()));
			item_dvar.insert("name", path_dvar);
			// append the item_dvar to the files_dvar
			files_dvar	+= item_dvar;			
		}
	}
	// convert the mfile_dvar into its bencoded format
	return datum_t(bencode_t::from_dvar(mfile_dvar));
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_mfile_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// define the macro
	#define CMD(var_type, var_name)				\
		if( !oss.str().empty() )	oss << " ";	\
		oss << #var_name << "=" << var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return the just built string
	return oss.str();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_mfile_t
 * 
 * - support null bt_mfile_t
 * - a serialized bt_mfile_t stores only the non precomputed value
 */
serial_t& operator << (serial_t& serial, const bt_mfile_t &bt_mfile)		throw()
{
	// serialize each field of the object

	// define the macro
	#define CMD(var_type, var_name)		\
		serial << bt_mfile.var_name();
	// build the string to return
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD
	// return serial
	return serial;
}

/** \brief unserialze a bt_mfile_t
 * 
 * - support null bt_mfile_t
 * - a serialized bt_mfile_t stores only the non precomputed value
 *   - thus it should be passed thru bt_mfile_t::complete_init() after 
 *     unserialization
 */
serial_t& operator >> (serial_t & serial, bt_mfile_t &bt_mfile)		throw(serial_except_t)
{	
	// reset the destination variable
	bt_mfile	= bt_mfile_t();

	// define the macro
	bt_mfile_t & dest_obj	 = bt_mfile;
	#define CMD(var_type, var_name)		\
		var_type	var_name;	\
		serial	>> var_name;		\
		dest_obj.var_name(var_name);
	// unserialize all the field
	COMMAND_FOR_EACH_FIELD( CMD );
	// undefine the macro
	#undef	CMD

	// if resulting bt_mfile is null, throw an exception
	if( bt_mfile.is_null() )	nthrow_serial_plain("Unserialized bt_mfile_t is null!");
	// complete the bt_mfile initialisation
	bt_mfile.complete_init();
	// return serial itself
	return serial;
}

NEOIP_NAMESPACE_END;






