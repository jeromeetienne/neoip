/*! \file
    \brief Header of the \ref serial_stat_t

*/


#ifndef __NEOIP_SERIAL_STAT_HPP__ 
#define __NEOIP_SERIAL_STAT_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class serial_stat_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint64_t	m_len_copied;		//!< the number of byte copied
	uint64_t	m_obj_copied;		//!< the number of object copied
	uint64_t	m_head_realloc;
	uint64_t	m_head_memshift;
	uint64_t	m_tail_realloc;
	uint64_t	m_tail_memshift;
public:
	/**************	ctor/dtor	***************************************/
	serial_stat_t()	throw();
	
	/************** action/query function	*******************************/
	uint64_t &	len_copied()	throw()		{ return m_len_copied;		}
	const uint64_t&	len_copied()	const throw()	{ return m_len_copied;		}
	uint64_t &	obj_copied()	throw()		{ return m_obj_copied;		}
	const uint64_t&	obj_copied()	const throw()	{ return m_obj_copied;		}
	uint64_t &	head_realloc()	throw()		{ return m_head_realloc;	}
	const uint64_t&	head_realloc()	const throw()	{ return m_head_realloc;	}
	uint64_t &	head_memshift()	throw()		{ return m_head_memshift;	}
	const uint64_t&	head_memshift()	const throw()	{ return m_head_memshift;	}
	uint64_t &	tail_realloc()	throw()		{ return m_tail_realloc;	}
	const uint64_t&	tail_realloc()	const throw()	{ return m_tail_realloc;	}
	uint64_t &	tail_memshift()	throw()		{ return m_tail_memshift;	}
	const uint64_t&	tail_memshift()	const throw()	{ return m_tail_memshift;	}

	/*************** arithmetic operator	*******************************/
	serial_stat_t &operator +=(const serial_stat_t &other)	throw();
	serial_stat_t	operator + (const serial_stat_t &other)	const throw()
						{ serial_stat_t tmp(*this); tmp += other; return tmp;	}

	/************** display function	*******************************/
	std::string		to_string()	const throw();
	friend			std::ostream & operator << (std::ostream & os, const serial_stat_t &serial_stat)	throw()
								{ return os << serial_stat.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SERIAL_STAT_HPP__  */



