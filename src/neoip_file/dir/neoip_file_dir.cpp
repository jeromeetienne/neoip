/*! \file
    \brief Implementation of \ref file_dir_t to read directories
 
*/

/* system include */
#include <glib.h>
#include <algorithm>
/* local include */
#include "neoip_file_dir.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       open/close
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Open a directory dir_path (MUST NOT be null)
 * 
 * - this is valid to open an already opened file_dir_t
 *   - the content obtained from the previous open(), will be overwritten by the new one
 */
file_err_t	file_dir_t::open(const file_path_t &dir_path)			throw()
{	
	GError *	gerror	= NULL;
	const gchar *	name;
	// sanity check - the dir_path MUST NOT be null
	DBG_ASSERT( !dir_path.is_null() );

	// zero the file database
	name_db	= std::vector<name_t>();	
	// open the directory
	GDir *	g_dir	= g_dir_open(dir_path.to_os_path_string().c_str(), 0, &gerror );
	// if the open failed, report the error now
	if( gerror )	return file_err_t(file_err_t::ERROR, gerror->message );

	// scan the directory
	while( (name = g_dir_read_name(g_dir)) ){
		file_stat_t	file_stat( dir_path.fullpath() / name );
		// if this file can't be stated, skip it
		if( file_stat.is_null() )	continue;
		// put the found name in the name_db
		name_db.push_back( name_t(name, file_stat) );
	}
	
	// close the directory
	g_dir_close(g_dir);

	// copy the dir_path
	this->dir_path	= dir_path;
	
	// return no error
	return file_err_t::OK;
}

/** \brief close the file_dir_t
 */
void file_dir_t::close()		throw()
{
	// zero the file database
	name_db = std::vector<name_t>();
}

/** \brief Remove the name at idx
 * 
 * - PRECONDITION: idx MUST be < .size()
 */
void	file_dir_t::remove(size_t idx)		throw()
{
	// sanity check - the idx be in the database
	DBG_ASSERT( idx < name_db.size() );
	// remove the name at idx 
	name_db.erase( name_db.begin() + idx );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          Query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the fullpath of the name at idx
 * 
 * - PRECONDITION: idx MUST be < .size()
 */
file_path_t	file_dir_t::fullpath(size_t idx)	const throw()
{
	// sanity check - the idx be in the database
	DBG_ASSERT( idx < name_db.size() );
	// build the fullpath and return it
	return dir_path.fullpath() / name_db[idx].get_basename();
}


/** \brief Return the relative path of the name at idx
 * 
 * - PRECONDITION: idx MUST be < .size()
 */
file_path_t	file_dir_t::relpath(size_t idx)	const throw()
{
	// sanity check - the idx be in the database
	DBG_ASSERT( idx < name_db.size() );
	// build the relative path and return it
	return dir_path / name_db[idx].get_basename();
}

/** \brief Return the basename of the name at idx
 * 
 * - PRECONDITION: idx MUST be < .size()
 */
file_path_t	file_dir_t::basename(size_t idx)	const throw()
{
	// sanity check - the idx be in the database
	DBG_ASSERT( idx < name_db.size() );
	// return the basename
	return name_db[idx].get_basename();
}

/** \brief Return the file_stat_t of the name at idx
 * 
 * - PRECONDITION: idx MUST be < .size()
 */
const file_stat_t &file_dir_t::get_file_stat(size_t idx)	const throw()
{
	// sanity check - the idx be in the database
	DBG_ASSERT( idx < name_db.size() );
	// return the basename
	return name_db[idx].get_file_stat();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          sort
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Functor for sort_by_dist()
 */
struct cmp_name_t : public std::binary_function<file_dir_t::name_t, file_dir_t::name_t, bool> {
	private:std::string	criteria_str;
	public:	cmp_name_t(const std::string &criteria_str) : criteria_str(criteria_str) {}
	
	bool operator()(const file_dir_t::name_t &item1, const file_dir_t::name_t &item2){
		const file_stat_t &	stat1		= item1.get_file_stat();
		const file_stat_t &	stat2		= item2.get_file_stat();
		const std::string &	basename1	= item1.get_basename();
		const std::string &	basename2	= item2.get_basename();
		DBG_ASSERT( !stat1.is_null() );
		DBG_ASSERT( !stat2.is_null() );
#if 0
		return item1.get_basename() < item2.get_basename();
#else
		// log to debug
		KLOG_DBG("compare "
				<< "item1=" << item1.get_basename() << (stat1.is_dir() ? "/":"")
				<< " and "
				<< "item2=" << item2.get_basename() << (stat2.is_dir() ? "/":"")
				);
		// go thru all the criteria in order
		// - it return true for the first criteria which IS item1<item2
		for( size_t i = 0; i < criteria_str.size(); i++ ){
//			KLOG_ERR("Test criteria=" <<criteria_str[i]);
			switch(criteria_str[i]){
			case 'N': if( basename1         >  basename2)		return true;
				  if( basename1         <  basename2)		return false;	break;
			case 'n': if( basename1         <  basename2)		return true;
				  if( basename1         >  basename2)		return false;	break;
			case 'S': if( stat1.get_size()  > stat2.get_size())	return true;
				  if( stat1.get_size()  < stat2.get_size())	return false;	break;
			case 's': if( stat1.get_size()  < stat2.get_size())	return true;
				  if( stat1.get_size()  > stat2.get_size())	return false;	break;
			case 'M': if( stat1.get_mtime() > stat2.get_mtime())	return true;
				  if( stat1.get_mtime() < stat2.get_mtime())	return false;	break;
			case 'm': if( stat1.get_mtime() < stat2.get_mtime())	return true;
				  if( stat1.get_mtime() > stat2.get_mtime())	return false;	break;
			case 'd': if( stat1.is_dir()  && !stat2.is_dir() )	return true;	
				  if(!stat1.is_dir()  &&  stat2.is_dir() )	return false;	break;
			case 'D': if(!stat1.is_dir()  &&  stat2.is_dir() )	return true;	
				  if( stat1.is_dir()  && !stat2.is_dir() )	return false;	break;
			}	
		}
		KLOG_DBG("item1 is NOT < to item2 by " << criteria_str);
		// if this point is reached, item1 is NOT < than item2, so return false
		return false;
#endif
	}
};

/** \brief Sort the file_dir_t in alphabatical order
 */
file_dir_t &	file_dir_t::sort(const std::string &criteria_str)		throw()
{
	// sort the basename_db
	std::sort(name_db.begin(), name_db.end(), cmp_name_t(criteria_str));
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          filter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Filter the name in the file_dir_t
 */
file_dir_t &	file_dir_t::filter(const file_dir_t::filter_ftor &filter_ftor)	throw()
{
	// for thru each entry of the file_dir_t
	for( size_t i = 0; i < name_db.size(); ){
		file_path_t	file_path	= fullpath(i);
		// if the filter doesnt match, skip this entry
		if( filter_ftor(file_path) == false ){
			i++;
			continue;
		}
		// if the filter matches, remove this entry
		remove(i);
	}
	// return the object itself
	return *this;
}

NEOIP_NAMESPACE_END



