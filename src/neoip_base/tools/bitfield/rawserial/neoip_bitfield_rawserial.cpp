/*! \file
    \brief Definition of the \ref bitfield_rawserial_t

\par Brief Description
bitfield_rawserial_t is a custom way to serialize bitfield_t without including 
the length of the bitfield.
- this is used in neoip_bt for example. because the number of bit, is the 
  number of piece which is already included elsewhere.
  - so it is nomore retransmited as an optimisation.

\par USAGE: example
- to unserialize:	serial	>> bitfield_rawserial_t(&bitfield, your_nb_bit);
- to serialize:	serial	<< bitfield_rawserial_t(&bitfield);
- to get the length: bitfield_rawserial_t(&pieceavail).length()

*/

/* system include */
/* local include */
#include "neoip_bitfield_rawserial.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor in case of a unserial
 */
bitfield_rawserial_t::bitfield_rawserial_t(bitfield_t *bitfield, size_t nb_bit) throw()
{
	// copy the parameter
	this->m_bitfield	= bitfield;
	this->nb_bit		= nb_bit;
	// reset the bitfield_t
	*m_bitfield		= bitfield_t(nb_bit);
}

/** \brief Constructor in case of a doserial
 */
bitfield_rawserial_t::bitfield_rawserial_t(bitfield_t *bitfield) throw()
{
	// copy the parameter
	this->m_bitfield	= bitfield;
	this->nb_bit		= std::numeric_limits<size_t>::max();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bitfield_t
 */
serial_t& operator << (serial_t& serial, const bitfield_rawserial_t &rawserial_bf)		throw()
{
	datum_t		datum	= datum_t(rawserial_bf.nb_byte());
	uint8_t*	p	= datum.uint8_ptr();
	// sanity check - nb_bit MUST NOT have been set
	DBG_ASSERT( rawserial_bf.nb_bit == std::numeric_limits<size_t>::max() );
	// set the whole datum_t to 0
	memset(p, 0, datum.size());
	// convert the datum_t to a bitfield_t bit by bit
	for(size_t i = 0; i < rawserial_bf.size(); i++){
		if( rawserial_bf.m_bitfield->get(i) )	p[i/8]	|= 0x80 >> (i%8);
	}	
	// append the datum_t data
	serial.append( datum.void_ptr(), datum.size() );
	// return serial
	return serial;
}

/** \brief unserialze a bitfield_t
 */
serial_t& operator >> (serial_t & serial, const bitfield_rawserial_t &rawserial_bf)
										throw(serial_except_t)
{
	size_t	nb_byte	= rawserial_bf.nb_byte();
	// sanity check - nb_bit MUST have been set
	DBG_ASSERT( rawserial_bf.nb_bit != std::numeric_limits<size_t>::max() );
	DBG_ASSERT( rawserial_bf.size() == rawserial_bf.nb_bit );
	// if the serial is not long enought, throw an exception
	if( serial.length() < nb_byte )	nthrow_serial_plain("bitfield_t too short");
	// consume the bitfield in the serial_t
	datum_t	datum(serial.void_ptr(), nb_byte);
	serial.consume( nb_byte );
	// convert the datum_t to a bitfield_t bit by bit
	const uint8_t *	p	= datum.uint8_ptr();
	for(size_t i = 0; i < rawserial_bf.nb_bit; i++){
		bool	value = p[i/8] & (0x80 >> (i%8));	
		rawserial_bf.m_bitfield->set(i, value);
	}
	// return serial
	return serial;
}


NEOIP_NAMESPACE_END;






