/*! \file
    \brief Implementation of \ref file_read_t to read in files in a syncronous mode

- this class is a KLUDGE for several reasons.
  - the main one is: the whole file is read in the constructor in order
    to support the serial_t.
  - serial_t serial in the class is public to be accessed easily by the 
    template 
    
\par TODO
- NO ERROR HANDLING in this function !?!?!? am i a loser or something ?
- to reimplement when time allow
 
*/

/* system include */
/* local include */
#include "neoip_file_read.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor for the class
 */
file_read_t::file_read_t()	throw()
{	
	serial	= NULL;
}

/** \brief Destructor for the class
 * 
 * - close the file if not already done
 */
file_read_t::~file_read_t()	throw()
{
	// close the file if needed
	if( file_in.is_open() )	file_in.close();
	if( serial )		nipmem_delete serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       open
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor for the class
 */
file_err_t	file_read_t::open(const std::string pathname, std::_Ios_Openmode mode)	throw()
{	
	serial_t	tmp_serial;
	// open the file
	file_in.open( pathname.c_str(), std::ios::in | mode );
	// Check if the open succeed
	if( !file_in.is_open() )
		return file_err_t(file_err_t::ERROR, "Cant open the file " + pathname);

	// read the whole file into the serial_t
	// - prevent from using large file
	// - this is lame but works for now, anyway this whole class has to be
	//   reimplementated
	while( !file_in.eof() ){
		char buffer[10*1024];
		int len_read	= file_in.readsome( buffer, sizeof(buffer) );
		DBG_ASSERT( len_read >= 0 );
		KLOG_DBG(" len_read=" << len_read);
		if( len_read == 0 )	break;
		if( len_read < 0 )	return file_err_t::ERROR;
		tmp_serial.append( buffer, len_read );
	}
	serial = nipmem_new serial_t( tmp_serial.get_data(), tmp_serial.get_len() );
	// return no error
	return file_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief explicitly close the file
 */
void file_read_t::close(void)							throw()
{
	// sanity check - the file MUST NOT be already closed
	DBG_ASSERT( file_in.is_open() );
	// close the file
	file_in.close();
}


NEOIP_NAMESPACE_END



