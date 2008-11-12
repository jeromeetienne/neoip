/*! \file
    \brief Definition of the \ref amf0_build_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_amf0_build.hpp"
#include "neoip_amf0_type.hpp"
#include "neoip_dvar.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief Helper on top of the other to_dvar
 *
 */
void	amf0_build_t::to_amf0(const dvar_t &dvar, bytearray_t &amf0_data)	throw()
{
	switch( dvar.type().get_value() ){
	case dvar_type_t::DOUBLE:
		amf0_data << amf0_type_t(amf0_type_t::NUMBER);
		amf0_data << double(dvar.dbl().get());
		break;
	case dvar_type_t::STRING:
		{std::string	str	= dvar.str().get();
		amf0_data << amf0_type_t(amf0_type_t::STRING);
		amf0_data << uint16_t(str.size());
		amf0_data.append(datum_t(str));
		break;}
	case dvar_type_t::BOOLEAN:
		amf0_data << amf0_type_t(amf0_type_t::BOOLEAN);
		amf0_data << uint8_t(dvar.boolean().get() ? 1 : 0);
		break;
	case dvar_type_t::NIL:
		amf0_data << amf0_type_t(amf0_type_t::NIL);
		break;
	case dvar_type_t::MAP:{
		const std::map<std::string, dvar_t> &		inmap	=  dvar.map().get_inmap();
		std::map<std::string, dvar_t>::const_iterator	iter;
		amf0_data << amf0_type_t(amf0_type_t::OBJECT);
		for(iter = inmap.begin(); iter != inmap.end(); iter++ ){
			const std::string &	key	= iter->first;
			const dvar_t		item	= iter->second;
			amf0_data << uint16_t(key.size());
			amf0_data.append(datum_t(key));
			to_amf0(item, amf0_data);
		}
		amf0_data << uint16_t(0);
		amf0_data << amf0_type_t(amf0_type_t::ENDOFOBJECT);
		break;}
	default:	KLOG_ERR("dvar_type_t:" << dvar.type().get_value() << " cant be converter to amf0");
			DBG_ASSERT( 0 );
	}
}

/** \brief Helper on top of the other to_dvar
 */
bytearray_t	amf0_build_t::to_amf0(const dvar_t &dvar)	throw()
{
	bytearray_t	amf0_data;
	// convert the dvar_t to amf0 and put it in amf0_data
	to_amf0(dvar, amf0_data);
	// return the result
	return amf0_data;
}

NEOIP_NAMESPACE_END


