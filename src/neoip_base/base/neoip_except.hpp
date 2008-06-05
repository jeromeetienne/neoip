/*! \file
    \brief Header of the neoip_except

\par possible improvement
- the location things is ugly in the code
  - another possibility would be to do my own throw using the same trick as
    the one done in nipmem_new.
  - thus all my exception would be able to get the location stuff
- in anycase a exception strategy must be defined
  - programation error are report thru ASSERT
  - runtime error are report thru error code or exception
  - exception are dangerous so use it with moderation
    
*/


#ifndef __NEOIP_EXCEPT_HPP__ 
#define __NEOIP_EXCEPT_HPP__ 
/* system include */
#include <string>
#include <sstream>
/* local include */
#include "neoip_errno.hpp"	// added as a convenience mechanism because exception often needs
				// the errno

// pseudo language to make exception handler more beautifull in the source
// - do not change the exception semantic
// - TODO not sure it is a good thing
// - after a while i decided that in most cases, exception wasnt a good thing 
//   - the only case when it is usefull is the serialization
#define NEXPT_S			try {
#define NEXPT_C( val )		}catch( val ){
#define NEXPT_C_1( val, inst )	}catch( val ){	inst;
#define NEXPT_E			}	
#define NEXPT_CE_1( val, inst )	NEXPT_C_1(val,inst)	NEXPT_E

#define NTHROW			throw
#define nthrow_rethrow		throw
//#define throw		throw

#define neoip_except_l(reason)		neoip_except(reason, __FILE__, __LINE__, __func__)
#define nthrow_errno(str)		NTHROW neoip_except_l(std::string(str) \
							+ ". errno=" + neoip_strerror(errno) )
#define nthrow_plain(str)		NTHROW neoip_except_l(std::string(str))

class neoip_except {
private:
	std::string	reason;

	std::string	filename;
	int		line;
	std::string	fctname;
public:	
	neoip_except(std::string reason) : reason(reason), line(-1){}
	neoip_except(std::string reason, std::string filename, int line, std::string fctname )
			: reason(reason), filename(filename), line(line), fctname(fctname)
			{}

	const std::string &	get_reason()	const { return reason;	}
	const std::string &	get_filename()	const { return filename;}
	const int &		get_line()	const { return line;	}
	const std::string &	get_fctname()	const { return fctname;	}

	std::string what() const {
		std::stringstream	sstream;
		sstream << reason;
		if( line != -1 )
			sstream << " [thrown at " << filename << ":" << fctname << "():" << line << "]";
		return sstream.str();
	}
};

/** \brief template to return a string from a ostream operator on a type
 * 
 * - typical use: "hello people " + OSTREAMSTR(blabla)
 *   - with blabla an instance with a ostream << redirection to "foo blabla bar"
 *     for example
 *   - and the result being a std::string "hello people foo blabla bar"
 * - i think it is possible to have a more generic template which would do
 *   the same result with "hello people " + blabla
 *   - but im too lame in c++ to know how
 *   - TODO ask to more experienced people
 */
template <class T> std::string OSTREAMSTR ( const T &val )
{
	std::stringstream	sstream;
	sstream << val;
	return sstream.str();
}


#endif	/* __NEOIP_EXCEPT_HPP__  */



