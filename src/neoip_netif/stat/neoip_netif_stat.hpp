/*! \file
    \brief Header of the \ref netif_stat_t

*/


#ifndef __NEOIP_NETIF_STAT_HPP__ 
#define __NEOIP_NETIF_STAT_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_date.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class netif_stat_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	m_name;			//!< the netif name
	date_t		m_capture_date;		//!< the present date when the stat has been captured
						//!< (usefull if one want to compute rate)
	uint64_t	m_rx_byte;		//!< the number of byte received
	uint64_t	m_tx_byte;		//!< the number of byte transmited
public:
	/**************	ctor/dtor	***************************************/
	netif_stat_t()	throw();
	
	/************** query function	***************************************/
	bool			is_null()	const throw()	{ return name().empty();	}	
	const std::string &	name()		const throw()	{ return m_name;		}
	const date_t &		capture_date()	const throw()	{ return m_capture_date;	}
	const uint64_t &	rx_byte()	const throw()	{ return m_rx_byte;		}
	const uint64_t &	tx_byte()	const throw()	{ return m_tx_byte;		}

	/*************** arithmetic operator	*******************************/
	netif_stat_t &	operator +=(const netif_stat_t &other)	throw();
	netif_stat_t &	operator -=(const netif_stat_t &other)	throw();
	netif_stat_t	operator + (const netif_stat_t &other)	const throw()
						{ netif_stat_t tmp(*this); tmp += other; return tmp;	}
	netif_stat_t	operator - (const netif_stat_t &other)	const throw()
						{ netif_stat_t tmp(*this); tmp -= other; return tmp;	}

	/*************** comparison operator	*******************************/
	int	compare(const netif_stat_t &other)	const throw();
	bool 	operator == (const netif_stat_t & other)const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const netif_stat_t & other)const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const netif_stat_t & other)const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const netif_stat_t & other)const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const netif_stat_t & other)const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const netif_stat_t & other)const throw()	{ return compare(other) >= 0;	}
	
	/************** display function	*******************************/
	std::string		to_string()	const throw();
	friend			std::ostream & operator << (std::ostream & os, const netif_stat_t &netif_stat)	throw()
								{ return os << netif_stat.to_string();	}
	/************** List of friend class	*******************************/
	friend class	netif_stat_helper_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_STAT_HPP__  */



