/*! \file
    \brief Header of the \ref nslan_rec_t
    
*/


#ifndef __NEOIP_NSLAN_REC_HPP__ 
#define __NEOIP_NSLAN_REC_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_nslan_id.hpp"
#include "neoip_datum.hpp"
#include "neoip_delay.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for nslan_rec_t
 */
class nslan_rec_t : NEOIP_COPY_CTOR_ALLOW {
private:
	nslan_keyid_t	keyid;		//!< the record key
	datum_t		payload;	//!< the data associated with this record id
	delay_t		ttl;		//!< the time to live of this record
public:
	/*************** ctor/dtor	***************************************/
	nslan_rec_t()	throw();
	nslan_rec_t(const nslan_keyid_t &keyid, const datum_t &payload, const delay_t &ttl)	throw();
	
	/*************** query function	***************************************/
	bool			is_null()	const throw();	
	const nslan_keyid_t &	get_keyid()	const throw()	{ return keyid;		}
	const datum_t &		get_payload()	const throw()	{ return payload;	}
	const delay_t &		get_ttl()	const throw()	{ return ttl;		}

	/*************** comparison operator	*******************************/
	int	compare(const nslan_rec_t &other)	const throw();
	bool 	operator == (const nslan_rec_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const nslan_rec_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const nslan_rec_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const nslan_rec_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const nslan_rec_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const nslan_rec_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const nslan_rec_t &nslan_rec)throw()
					{ return os << nslan_rec.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &	operator << (serial_t& serial, const nslan_rec_t &nslan_rec)	throw();
	friend	serial_t &	operator >> (serial_t& serial, nslan_rec_t &nslan_rec) throw(serial_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_REC_HPP__  */










