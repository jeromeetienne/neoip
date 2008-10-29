/*! \file
    \brief Definition of the \ref amf0_parse_t class

\par Brief Description
this module is done to parse Actionscript data types in AMF. The flv_tagtype_t::META
of in flv video are in AMF.
- for more details about the byte format http://osflash.org/documentation/amf/astypes

*/

/* system include */
/* local include */
#include "neoip_amf0_parse.hpp"
#include "neoip_amf0_type.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_dvar.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert AMF data into a dvar_t
 */
flv_err_t amf0_parse_t::amf_to_dvar(bytearray_t &amf0_data, dvar_t &dvar_out)	throw()
{
	// log to debug
	KLOG_DBG("enter amf0_data=" << amf0_data);
	// call the internal parser
	try {
		dvar_out	= parser(amf0_data);
	}catch(const serial_except_t &e){
		std::string	reason	= "failed to parse flv AMF data due to " + e.what();
		return flv_err_t(flv_err_t::ERROR, reason);
	}
	// return no error
	return flv_err_t::OK;
}

/** \brief reccursive AMF data into a dvar_t
 */
dvar_t	amf0_parse_t::parser(bytearray_t &amf0_data)	throw(serial_except_t)
{
	amf0_type_t	amf0_type;
	
	// get the amf0_type_t
	amf0_data	>> amf0_type;
	if( amf0_type == amf0_type_t::NUMBER ){
		double	value;
		amf0_data	>> value;
		// log to debug
		KLOG_DBG(amf0_type.to_string() << "=" << value);
		// return it as dvar_dbl_t
		return	dvar_dbl_t(value);
	}else if( amf0_type == amf0_type_t::BOOLEAN ){
		uint8_t	value;
		amf0_data	>> value;
		// log to debug
		KLOG_DBG(amf0_type.to_string() << "=" << (value ? "true" : "false"));
		// return the boolean as a int
		return	dvar_int_t(value);
	}else if( amf0_type == amf0_type_t::STRING ){
		uint16_t	str_size;
		datum_t		str_data;
		// get the size of the string
		amf0_data	>> str_size;
		// log to debug
		//KLOG_DBG("str_size=" << str_size);
		// get the data of the string
		str_data	= amf0_data.head_consume(str_size);
		// log to debug
		KLOG_DBG(amf0_type.to_string() << "=" << str_data.to_stdstring());
		return	dvar_str_t(str_data.to_stdstring());
	}else if( amf0_type == amf0_type_t::OBJECT ){	
		dvar_t	dvar	= dvar_map_t();
		// log to debug
		KLOG_DBG(amf0_type.to_string() << ": start");
		// parse the array itself
		while(true){
			// if the amf0_data is now empty, return the value now
			// - some .flv dont include the 0x00, 0x00, 0x09 at the end if it is 
			//   the last data of the amf0_data
			// - this is not in the osflash page, but e.g. youtube does it this way
			if( amf0_data.empty() )	return dvar;
			// get the key of this element
			uint16_t	key_size;
			datum_t		key_data;
			amf0_data	>> key_size;
			key_data	= amf0_data.head_consume(key_size);
			// log to debug
			KLOG_DBG("key_data=" << key_data.to_stdstring());
			// if key_size == 0 and the next byte is 0x09, this is the end of this
			if( key_size == 0 && !amf0_data.empty() && *amf0_data.char_ptr() == 0x09){
				uint8_t	dummy;
				// consume the 0x09 terminator
				amf0_data	>> dummy;
				// log to debug
				KLOG_DBG(amf0_type.to_string() << ": end");
				// return the resulting dvar_t
				return	dvar;
			}
			// parse the value
			dvar_t	value	= parser(amf0_data);
			// insert this key/value inside the dva
			dvar.map().insert(key_data.to_stdstring(), value);
		}
		// NOTE: this points MUST never be reached
		DBG_ASSERT( 0 );
	}else if( amf0_type == amf0_type_t::UNDEFINED ){
		// return a null dvar_t
		return	dvar_t();
	}else if( amf0_type == amf0_type_t::MIXEDARRAY ){
		dvar_t	dvar	= dvar_map_t();
		// get the higuest_idx of this array
		// - NOTE: this is unclear what is the purpose of this
		// - "the highest numeric index in the array, or 0 if there are 
		//    none or they are all negative."
		uint32_t	highest_index;
		amf0_data	>> highest_index;
		
		// log to debug
		KLOG_DBG(amf0_type.to_string() << ": start highest_index=" << highest_index);
		// parse the array itself
		while(true){
			// if the amf0_data is now empty, return the value now
			// - some .flv dont include the 0x00, 0x00, 0x09 at the end if it is 
			//   the last data of the amf0_data
			// - this is not in the osflash page, but e.g. youtube does it this way
			if( amf0_data.empty() )	return dvar;
			// get the key of this element
			uint16_t	key_size;
			datum_t		key_data;
			amf0_data	>> key_size;
			key_data	= amf0_data.head_consume(key_size);
			// log to debug
			KLOG_DBG("key_data=" << key_data.to_stdstring());
			
			// if key_size == 0 and the next byte is 0x09, this is the end of this
			if( key_size == 0 && !amf0_data.empty() && *amf0_data.char_ptr() == 0x09){
				uint8_t	dummy;
				// consume the 0x09 terminator
				amf0_data	>> dummy;
				// log to debug
				KLOG_DBG(amf0_type.to_string() << ": end");
				// return the resulting dvar_t
				return	dvar;
			}
			
			// parse the value
			dvar_t	value	= parser(amf0_data);
			// insert this key/value inside the dva
			dvar.map().insert(key_data.to_stdstring(), value);
		}
		// NOTE: this points MUST never be reached
		DBG_ASSERT( 0 );
	}else if( amf0_type == amf0_type_t::ARRAY ){
		dvar_t	dvar	= dvar_arr_t();
		// get the number of item
		uint32_t	nb_item;
		amf0_data	>> nb_item;
		// log to debug
		KLOG_DBG(amf0_type.to_string() << ": start nb_item="<< nb_item);
		// parse every item and put it in the dvar_arr
		for(size_t i = 0; i < nb_item; i++)	dvar.arr() += parser(amf0_data);
		// log to debug
		KLOG_DBG(amf0_type.to_string() << ": end");
		// return the dvar_arr_t
		return	dvar;
	}else if( amf0_type == amf0_type_t::DATE ){
		double		from_epoch_ms;
		uint16_t	gmt_offset_min;
		amf0_data	>> from_epoch_ms;
		amf0_data	>> gmt_offset_min;
		KLOG_DBG("DATE: from_epoch_ms="<< from_epoch_ms << " gmt_offset_min="<< gmt_offset_min);
		// return it as a dvar_dbl_t (ignoring the gmt_offset_min)
		return	dvar_dbl_t(from_epoch_ms);
	}else{
		nthrow_serial_plain("unparse amf0_type " + amf0_type.to_string() + "... given up");	
	}
}

NEOIP_NAMESPACE_END


