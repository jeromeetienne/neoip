/*! \file
    \brief Header of the \ref file_write_t.cpp

*/


#ifndef __NEOIP_FILE_WRITE_HPP__ 
#define __NEOIP_FILE_WRITE_HPP__ 

/* system include */
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

/* local include */
#include "neoip_file_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_assert.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to write in files in a syncronous mode
 */
class file_write_t {
private:
	std::ofstream		file_out;	
public:
	/*************** ctor/dtor	***************************************/
	file_write_t()	throw();
	~file_write_t()	throw();

	/*************** operation 	***************************************/
	file_err_t	open(const std::string pathname, std::_Ios_Openmode mode = std::ios::binary)
										throw();
	void		close()							throw();
	int		write(void * const buffer, int len)			throw();
	int		write(const std::string &str)				throw()
				{ return write((void *)str.c_str(), str.size());	}
};

/** \brief template to have all the \ref serial_t stuff automaticaly in \ref file_write_t
 */
template <class T> file_write_t & operator << (file_write_t &fwrite, const T &val)	throw()
{
	serial_t	serial;
	serial << val;
	size_t len = fwrite.write( serial.get_data(), serial.get_len() );
	DBG_ASSERT( len == serial.get_len() );
	return fwrite;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_WRITE_HPP__  */



