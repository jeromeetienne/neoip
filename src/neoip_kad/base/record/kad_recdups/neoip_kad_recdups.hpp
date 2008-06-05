/*! \file
    \brief Header of the \ref kad_recdups_t
    
*/


#ifndef __NEOIP_KAD_RECDUPS_HPP__ 
#define __NEOIP_KAD_RECDUPS_HPP__ 
/* system include */
#include <vector>
/* local include */
#include "neoip_kad_rec.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for id
 */
class kad_recdups_t : NEOIP_COPY_CTOR_ALLOW {
private:
	kad_keyid_t		keyid;
	
	class			recval_t;
	std::vector<recval_t>	recval_db;

	size_t			get_serial_hd_main()	const throw();
	size_t			get_serial_hd_per_rec()	const throw();
	

	std::vector<recval_t>::iterator	get_by_recid(const kad_recid_t &recid)	throw();
public:
	kad_recdups_t()					throw();

	bool			empty()			const throw()	{ return recval_db.empty();	}
	//! return the number of node in the list
	size_t			size()			const throw()	{ return recval_db.size();	}
	const kad_keyid_t &	get_keyid()		const throw()	{ return keyid;			}
	
	kad_recdups_t &	update(const kad_rec_t &kad_rec)	throw();
	kad_recdups_t &	update(const kad_recdups_t &other)	throw();

	kad_rec_t 	operator[](int idx)			const throw();
	kad_recdups_t &	sort_by_recid()				throw();
	
	size_t		truncate_to_serial_len(size_t max_len)	throw();

	void		randomize_order()			throw();
	void		truncate_at_tail(size_t nb_rec_del)	throw();

	size_t		nb_serialized_rec_in_size(size_t size)	const throw();

	/*************** display function	*******************************/
	std::string	to_string()			const throw();
friend	std::ostream &	operator << (std::ostream & os, const kad_recdups_t &kad_recdups)	throw()
		{ return os << kad_recdups.to_string();	}
	/*************** serialization function	*******************************/
friend	serial_t &operator << (serial_t& serial, const kad_recdups_t &kad_recdups)	throw();
friend	serial_t &operator >> (serial_t& serial, kad_recdups_t &kad_recdups) 	throw(serial_except_t);
	/*************** xmlrpc function	*******************************/
friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_recdups_t &kad_recdups)throw();
friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_recdups_t &kad_recdups)
										throw(xml_except_t);
};

/** \brief a given record data for this recdups_t
 */
class kad_recdups_t::recval_t {
private:
	kad_recid_t	recid;		//!< globally statistically unique id of this record
	datum_t		payload;	//!< the data associated with this record id
	delay_t		ttl;		//!< the time to live of this record
public:
	/*************** ctor/dtor	***************************************/
	recval_t(const kad_rec_t &kad_rec)	throw(){
		this->recid	= kad_rec.get_recid();
		this->payload	= kad_rec.get_payload();
		this->ttl	= kad_rec.get_ttl();
	}

	/*************** Query function	***************************************/
	const kad_recid_t &	get_recid()	const throw()	{ return recid;		}
	const datum_t &		get_payload()	const throw()	{ return payload;	}
	const delay_t &		get_ttl()	const throw()	{ return ttl;		}
	
	/*************** comparison operator (based ONLY on the recid)	*******/
	int	compare(const recval_t &other)	const throw()	{ return recid.compare(other.recid);	}
	bool 	operator == (const recval_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const recval_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const recval_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const recval_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const recval_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const recval_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** List of friend function	***********************/	
	friend	class kad_recdups_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RECDUPS_HPP__  */










