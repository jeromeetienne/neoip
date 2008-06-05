/*! \file
    \brief Implementation of \ref file_write_t to write in files in a syncronous mode
    
\par TODO
- NO ERROR HANDLING in this function !?!?!? am i a loser or something ?
- moreover the API is real shitty as well
  - the same for file_read_t. im definitly a big smelly shit abandoned by a street
    dog who didnt want me anymore.
    
*/

/* system include */
/* local include */
#include "neoip_file_write.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor for the class
 */
file_write_t::file_write_t()	throw()
{	
}

/** \brief Destructor for the class
 * 
 * - close the file if not already done
 */
file_write_t::~file_write_t()	throw()
{
	// close the file if needed
	if( file_out.is_open() )	file_out.close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       open
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Open a file
 * 
 * - TODO why this function get the filename as a string and not a file_path_t ???
 *   - because i am a shit, that's why.
 *   - to change
 */
file_err_t	file_write_t::open(const std::string pathname, std::_Ios_Openmode mode)	throw()
{	
	// open the file
	file_out.open( pathname.c_str(), std::ios::out | mode );
	// Check if the open succeed
	if( !file_out.is_open() )
		return file_err_t(file_err_t::ERROR, "Cant open the file " + pathname);	
	// return no error
	return file_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief write data in the file
 */
int file_write_t::write(void * const buffer, int len)	throw()
{
	file_out.write( (char *)buffer, len );
	return len;
}

/** \brief explicitly close the file
 */
void file_write_t::close()				throw()
{
	// sanity check - the file MUST NOT be already closed
	DBG_ASSERT( file_out.is_open() );
	// close the file
	file_out.close();
}


NEOIP_NAMESPACE_END


