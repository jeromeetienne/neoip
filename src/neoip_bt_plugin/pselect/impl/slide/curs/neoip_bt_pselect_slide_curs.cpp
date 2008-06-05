/*! \file
    \brief Definition of the \ref bt_pselect_slide_curs_t

\par IMPORTANT
- every time a bt_pselect_slide_curs_t is contructed/deleted or modified
  the caller MUST call bt_swarm_t::notify_pieceprec_change()
  - thus the scheduler knows about it
  - TODO what about doing this automatically from the bt_pselect_vapi_t
    - this would avoid to forget about the change
    - this would link the bt_swarm_t with the bt_pselect_vapi_t
    - so i dunno but i note the possibility as i already got bugs due to forgeting
      to update the bt_swarm_t
    - maybe some helpers doing the 2 simultenaously
\par TODO
- TODO make a real start() instead of the current start in ctor

\par About has_circularidx integration
- this mean that
  - when the offset is set, it is first modularized
    - this it is always < pselect_slide->size()
  - pieceprec(pieceidx)
    - accept only pieceidx < pselect_slide->size()
    - return the proper bt_pieceprec_t in case of circular
  - cursidx_to_pieceidx return std::numeric_limits<size_t>::max()
    if the cursidx is out of range
    - so it should be tested in the caller
*/

/* system include */
/* local include */
#include "neoip_bt_pselect_slide_curs.hpp"
#include "neoip_bt_pselect_slide.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_pselect_slide_curs_t::bt_pselect_slide_curs_t(bt_pselect_slide_t *m_pselect_slide
				, const bt_pselect_slide_curs_arg_t &arg)	throw()
{
	// copy the parameter
	this->m_pselect_slide	= m_pselect_slide;
	this->m_pieceprec_arr	= arg.pieceprec_arr();
	this->m_offset		= arg.offset();
	this->m_has_circularidx	= arg.has_circularidx();
	// link this object to the attached bt_selector2_buff_t
	pselect_slide()->curs_dolink(this);
}

/** \brief Destructor
 */
bt_pselect_slide_curs_t::~bt_pselect_slide_curs_t()	throw()
{
	// unlink this object from the attached bt_selector2_buff_t
	pselect_slide()->curs_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert a relative piece_idx within this curs to a absolute piece_idx
 * 
 * - it handle has_circularidx when needed
 * - if the resulting piece_idx is outofrange, return std::numeric_limits<size_t>::max() 
 */
size_t	bt_pselect_slide_curs_t::cursidx_to_pieceidx(size_t curs_idx)	const throw()
{
	size_t	piece_idx	= curs_idx + offset();	 
	// if has_circularidx(), return 'modularized' piece_idx 
	if( has_circularidx() )	return piece_idx % pselect_slide()->size();
	
	// sanity check - at this point, has_circularidx() is not set
	DBG_ASSERT( !has_circularidx() );
	// if the piece_idx is outofrange, return std::numeric_limits<size_t>::max()
	if( piece_idx >= pselect_slide()->size() ) return std::numeric_limits<size_t>::max();
	// return the piece_idx
	return piece_idx;
}

/** \brief return the bt_pieceprec_t for a given pieceidx according to this slide_curs
 */
bt_pieceprec_t	bt_pselect_slide_curs_t::pieceprec(size_t piece_idx)		const throw()
{
	// sanity check - the piece_idx MUST be < pselect_slide()->nb_piece()
	DBG_ASSERT( piece_idx < pselect_slide()->nb_piece() );
	
	// if has_circular_idx and piece_idx is 'before' the offset, try add the
	// full size in a attempts to see if the wrapped around pieceprec_arr can
	// catch it  
	if( has_circularidx() && piece_idx < offset())	piece_idx += pselect_slide()->size();
	
	// if the pieceidx is not included in this curs, return bt_pieceprec_t::NOTNEEDED;
	if( piece_idx <  offset() )				return bt_pieceprec_t::NOTNEEDED;
	if( piece_idx >= offset() + pieceprec_arr().size() )	return bt_pieceprec_t::NOTNEEDED;
	// else return the bt_pieceprec_t for this pieceidx
	return pieceprec_arr()[piece_idx - offset()];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the new offset of this bt_pselect_slide_curs_t
 * 
 * - if has_circularidx() is set, new_offset may be greated than bt_pselect_slide_t::size()
 */
void	bt_pselect_slide_curs_t::offset(size_t new_offset)	throw()
{
	// if has_circularidx(), the new_offset is 'modularized' 
	if( has_circularidx() )	new_offset	%= pselect_slide()->size();
	
#if 1	//TODO neoip-oload trigger the following assert when doing
	// wget -O - http://localhost:4500/http/jmehost2/~jerome/go\ open.mp4
	if( new_offset >= pselect_slide()->size() ){
		KLOG_ERR("new_offset="<< new_offset);
		KLOG_ERR("pselect_slide()->size()"<< pselect_slide()->size());
	}
#endif
	
	// sanity check - the new_offset MUST be less than pselect_slide()->size() 
	DBG_ASSERT(new_offset < pselect_slide()->size());
	// set the new_offset as the current_one
	m_offset = new_offset;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_pselect_slide_curs_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )		return "null";
	// build the string
	oss << "[";
	oss << "offset="	<< offset();
	oss << " ";
	oss << "pieceprec_prec="<< pieceprec_arr();
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






