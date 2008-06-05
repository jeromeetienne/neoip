/*! \file
    \brief Header of the \ref bitfield_t
    
*/


#ifndef __NEOIP_BITFIELD_RAWSERIAL_HPP__ 
#define __NEOIP_BITFIELD_RAWSERIAL_HPP__ 
/* system include */
/* local include */
#include "neoip_bitfield.hpp"
#include "neoip_serial.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief bitfield_rawserial_t is a custom way to serialize bitfield_t without
 *         including the length of the bitfield
 */
class	bitfield_rawserial_t : NEOIP_COPY_CTOR_DENY {
private:
	bitfield_t *	m_bitfield;	//!< pointer on the bitfield_t to work on
	size_t		nb_bit;		//!< the number of bit from ctor IIF for unserial
public:
	/*************** ctor/dtor	***************************************/
	bitfield_rawserial_t(bitfield_t *bitfield, size_t nb_bit)	throw();
	bitfield_rawserial_t(bitfield_t *bitfield)			throw();

	/*************** query function	***************************************/
	size_t	nb_byte()	const throw()	{ return ceil_div(m_bitfield->size(), size_t(8));	}
	size_t	size()		const throw()	{ return m_bitfield->size();				}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bitfield_rawserial_t &rawserial)	throw();
	friend	serial_t& operator >> (serial_t & serial, const bitfield_rawserial_t &rawserial)throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BITFIELD_RAWSERIAL_HPP__  */










