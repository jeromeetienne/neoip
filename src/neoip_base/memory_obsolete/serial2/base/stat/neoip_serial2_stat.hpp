/*! \file
    \brief Header of the \ref serial2_stat_t

*/


#ifndef __NEOIP_SERIAL2_STAT_HPP__ 
#define __NEOIP_SERIAL2_STAT_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class serial2_stat_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint64_t	m_len_copied;		//!< the number of byte copied
	uint64_t	m_obj_copied;		//!< the number of object copied
public:
	/**************	ctor/dtor	***************************************/
	serial2_stat_t()	throw();
	
	/************** action/query function	*******************************/
	uint64_t &	len_copied()	throw()		{ return m_len_copied;	}
	const uint64_t&	len_copied()	const throw()	{ return m_len_copied;	}
	uint64_t &	obj_copied()	throw()		{ return m_obj_copied;	}
	const uint64_t&	obj_copied()	const throw()	{ return m_obj_copied;	}

	/*************** arithmetic operator	*******************************/
	serial2_stat_t &operator +=(const serial2_stat_t &other)	throw();
	serial2_stat_t	operator + (const serial2_stat_t &other)	const throw()
						{ serial2_stat_t tmp(*this); tmp += other; return tmp;	}

	/*************** comparison operator	*******************************/
	int	compare(const serial2_stat_t &other)		const throw();
	bool 	operator == (const serial2_stat_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const serial2_stat_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const serial2_stat_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const serial2_stat_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const serial2_stat_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const serial2_stat_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/************** display function	*******************************/
	std::string		to_string()	const throw();
	friend			std::ostream & operator << (std::ostream & os, const serial2_stat_t &serial2_stat)	throw()
								{ return os << serial2_stat.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SERIAL2_STAT_HPP__  */



