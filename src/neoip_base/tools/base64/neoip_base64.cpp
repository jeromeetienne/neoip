/*! \file
    \brief Definition of the \ref base64_t class

\par Brief Description
This module has 2 functions which performs a base64 encoding as in rfc3548.
It allows to specify the alphabet to use. which is important because the 
normal base64 alphabet contains "+" and "/" which are not safe to use in url or filename.
the safe alphabet is the same except that "+" is replaced by "-" and "/" by "_", this
little modification makes it suitable for url and filename.
    
*/

/* system include */
/* local include */
#include "neoip_base64.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref base64_t constant
const std::string base64_t::norm_alphabet =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz"
						"0123456789+/";
const std::string base64_t::safe_alphabet =	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz"
						"0123456789-_";
// end of constants definition


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			encode in base64
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert binary to a base64 string
 * 
 * - the base64 described here is the rfc3548.5 one
 */
std::string base64_t::encode(void *data_ptr, size_t data_len, const std::string &alphabet)throw()
{
	uint8_t		enc_quantum[3];
	size_t		enc_idx = 0;
	std::string	result;

	// - From rfc3548, "Proceeding from left to right, a 24-bit input group is
	//   formed by concatenating 3 8-bit input groups. These 24 bits are then 
	//   treated as 4 concatenated 6-bit groups, each of which is translated 
	//   into a single digit in the base 64 alphabet."
	
	// go thru all the data - fill the enc_quantum and translate it when full
	for( ; data_len != 0; data_len-- ){
		// fill the encoding quantum with the incoming data
		enc_quantum[enc_idx++]	= *(uint8_t*)data_ptr;
		// goto the next incoming byte
		data_ptr	= (uint8_t*)data_ptr + 1;
		// if the encoding quantum is full, translate it into the alphabet
		if( enc_idx == sizeof(enc_quantum) ){
			// from rfc3548
			// "Each 6-bit group is used as an index into an array of 64 printable
			//  characters.  The character referenced by the index is placed in the
			//  output string."
			uint8_t	alphabet_idx;
			// encode the first 6-bit group
			alphabet_idx = ((enc_quantum[0] & 0xfc) >> 2);
			result += alphabet[alphabet_idx];
			// encode the second 6-bit group
			alphabet_idx = ((enc_quantum[0] & 0x03) << 4) + ((enc_quantum[1] & 0xf0) >> 4);
			result += alphabet[alphabet_idx];
			// encode the third 6-bit group
			alphabet_idx = ((enc_quantum[1] & 0x0f) << 2) + ((enc_quantum[2] & 0xc0) >> 6);
			result += alphabet[alphabet_idx];
			// encode the forth 6-bit group
			alphabet_idx = (enc_quantum[2] & 0x3f);
			result += alphabet[alphabet_idx];

			// reset the enc_idx
			enc_idx = 0;
		}
	}
	
	// if the enc_quantum IS empty, the convetion is completed, return now
	if( enc_idx == 0 )	return result;
	
	// if the enc_quantum isn't empty, code the remaining and add some padding

	// from rfc3548, "When fewer than 24 input bits are available in
	// an input group, zero bits are added (on the right) to form an
	// integral number of 6-bit groups.
	for(size_t j = enc_idx; j < sizeof(enc_quantum); j++)	enc_quantum[j] = '\0';

	uint8_t	dec_chunk[4];
	dec_chunk[0] =  (enc_quantum[0] & 0xfc) >> 2;
	dec_chunk[1] = ((enc_quantum[0] & 0x03) << 4) + ((enc_quantum[1] & 0xf0) >> 4);
	dec_chunk[2] = ((enc_quantum[1] & 0x0f) << 2) + ((enc_quantum[2] & 0xc0) >> 6);
	dec_chunk[3] =   enc_quantum[2] & 0x3f;

	for(size_t j = 0; j < enc_idx + 1; j++)
		result += alphabet[dec_chunk[j]];

	// add some padding as the input didnt full the whole enc_quantum
	if( enc_idx == 1 ){
		// from rfc3548, "(2) the final quantum of encoding input is 
		// exactly 8 bits; here, the final unit of encoded output will
		// be two characters followed by two "=" padding characters
		result += "==";
	}else{
		// from rfc3548, "(3) the final quantum of encoding input is
		// exactly 16 bits; here, the final unit of encoded output will
		// be three characters followed by one "=" padding character.	
		result += "=";
	}
		
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			decode in base64
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a base64 string into binary data
 */
datum_t base64_t::decode(const std::string &str_enc, const std::string &alphabet)throw()
{
	size_t		chunk_idx	= 0;
	uint8_t		dec_chunk[4];
	std::string	tmp_str;
	
	// go thru the whole encoded string
	for( size_t str_idx = 0; str_idx < str_enc.size(); str_idx++ ){
		// if the current charater is padding , leave the loop
		if( is_padding(str_enc[str_idx]) )			break;
		// if the current charater doesnt belong to alphabet, leave the loop
		if( !belong_to_alphabet(str_enc[str_idx], alphabet) )	break;

		// put the current character in the dec_chunk
		dec_chunk[chunk_idx++] = str_enc[str_idx];

		// if the decode chunk is full, decode it
		if( chunk_idx == sizeof(dec_chunk) ){
			// convert the dec_chunk from (char in alphabet) to (idx in the alphabet)
			for(size_t i = 0; i < sizeof(dec_chunk); i++)
				dec_chunk[i] = alphabet.find(dec_chunk[i]);

			// convert the dec_chunk to the tmp_str
			tmp_str += (dec_chunk[0] << 2) 		+ ((dec_chunk[1] & 0x30) >> 4);
			tmp_str += ((dec_chunk[1] & 0xf) << 4)	+ ((dec_chunk[2] & 0x3c) >> 2);
			tmp_str += ((dec_chunk[2] & 0x3) << 6)	+   dec_chunk[3];

			// reset the chunk_idx
			chunk_idx	= 0;
		}
	}
	
	// if the dec_chunk IS empty, the convertion is completed, return now
	if( chunk_idx == 0 )	return datum_t(tmp_str.c_str(), tmp_str.size());
	
	// if the dec_chunk isn't empty, decode the remaining and discard the padding padding

	// zero the rest of the dec_chunk
	for(size_t i = chunk_idx; i < sizeof(dec_chunk); i++)	dec_chunk[i] = 0;

	// convert the dec_chunk from (char in alphabet) to (idx in the alphabet)
	for(size_t i = 0; i < sizeof(dec_chunk); i++)
		dec_chunk[i] = alphabet.find(dec_chunk[i]);

	// convert the dec_chunk to the enc_quantum
	uint8_t		enc_quantum[3];
	enc_quantum[0]	=  (dec_chunk[0] << 2) 		+ ((dec_chunk[1] & 0x30) >> 4);
	enc_quantum[1]	= ((dec_chunk[1] & 0xf) << 4)	+ ((dec_chunk[2] & 0x3c) >> 2);
	enc_quantum[2]	= ((dec_chunk[2] & 0x3) << 6)	+   dec_chunk[3];

	// copy the enc_quantum to the tmp_str
	for(size_t i = 0; i < chunk_idx-1; i++)		tmp_str += enc_quantum[i];

	// convert the tmp_str into a datum
	return datum_t(tmp_str.c_str(), tmp_str.size());
}


NEOIP_NAMESPACE_END


