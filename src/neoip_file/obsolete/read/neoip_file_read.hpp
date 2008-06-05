/*! \file
    \brief Header of the \ref file_read_t.cpp

*/


#ifndef __NEOIP_FILE_READ_HPP__ 
#define __NEOIP_FILE_READ_HPP__ 

/* system include */
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

/* local include */
#include "neoip_file_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to read in files in a syncronous mode
 * 
 */
class file_read_t {
private:
	std::ifstream	file_in;	
public:
	serial_t *	serial;

	file_read_t()	throw();
	~file_read_t()	throw();

	file_err_t	open(const std::string pathname, std::_Ios_Openmode mode = std::ios::binary)
										throw();
	void		close()							throw();
};

/** \brief template to have all the \ref serial_t stuff automaticaly in \ref file_read_t
 */
template <class T> file_read_t & operator >> (file_read_t &file_read, T &val) throw(serial_except_t)
{
	*(file_read.serial) >> val;
	return file_read;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_READ_HPP__  */



