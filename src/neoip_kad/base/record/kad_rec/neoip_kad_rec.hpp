/*! \file
    \brief Header of the \ref kad_rec_t
    
*/


#ifndef __NEOIP_KAD_REC_HPP__ 
#define __NEOIP_KAD_REC_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_id.hpp"
#include "neoip_datum.hpp"
#include "neoip_delay.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for kad's record
 */
class kad_rec_t : NEOIP_COPY_CTOR_ALLOW {
private:
	kad_recid_t	recid;		//!< globally statistically unique id of this record
	kad_keyid_t	keyid;		//!< the record key
	datum_t		payload;		//!< the data associated with this record id
	delay_t		ttl;		//!< the time to live of this record
public:
	/*************** ctor/dtor	***************************************/
	kad_rec_t()	throw()	{}
	kad_rec_t(const kad_recid_t &recid, const kad_keyid_t &keyid, const delay_t &ttl
						, const datum_t &payload)	throw();
	
	/*************** Query Function	***************************************/
	bool		is_null()	const throw()		{ return keyid.is_null();	}
	bool		is_tie_winner_against(const kad_rec_t &other)	const throw();
	
	/*************** get/set function	*******************************/
	const kad_recid_t &	get_recid()	const throw()	{ return recid;	}
	kad_rec_t &		set_recid(const kad_recid_t &recid)	throw()
							 	{ this->recid = recid; return *this;	}
	const kad_keyid_t &	get_keyid()	const throw()	{ return keyid;	}
	kad_rec_t &		set_keyid(const kad_keyid_t &keyid)	throw()
							 	{ this->keyid = keyid; return *this;	}
	const datum_t &		get_payload()	const throw()	{ return payload;	}
	kad_rec_t &		set_payload(const datum_t &payload)	throw()
							 	{ this->payload = payload; return *this;}
	const delay_t &		get_ttl()	const throw()	{ return ttl;	}
	kad_rec_t &		set_ttl(const delay_t &ttl)	throw()
							 	{ this->ttl = ttl; return *this;	}

	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const kad_rec_t &kad_rec)	throw()
				{ return os << kad_rec.to_string();	}
	/*************** serialization function	*******************************/
	friend	serial_t &	operator << (serial_t& serial, const kad_rec_t &kad_rec)	throw();
	friend	serial_t &	operator >> (serial_t& serial, kad_rec_t &kad_rec)	 	throw(serial_except_t);
	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_rec_t &kad_rec)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_rec_t &kad_rec)	throw(xml_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_REC_HPP__  */










