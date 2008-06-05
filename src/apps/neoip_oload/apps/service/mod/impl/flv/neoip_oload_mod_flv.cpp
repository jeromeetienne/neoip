/*! \file
    \brief Class to handle the oload_mod_flv_t
  
\par Brief Description
This module implement a special trick for flash video .flv which allow to 
seek anywhere in a video, even to position not yet downloaded by the player.
The issue is twofold:
1. flash player is unable to perform actual http request for a given range
2. the returned video must be in a correct .flv format aka starting with a header
   and the first frame must be a keyframe
   
\par workaround lack of http range request
- original description of the technique
  - http://www.flashcomguru.com/index.cfm/2005/11/2/Streaming-flv-video-via-PHP-take-two
- the trick is to pass the rangereq_beg/rangereq_end by variable in the http_uri_t 
  - the variables are in the inner_uri.
  - they reuse the "neoip_metavar_*" prefix common to inner_var
  - "neoip_metavar_oload_flv_rangereq_beg" is the rangereq_beg
  - "neoip_metavar_oload_flv_rangereq_end" is the rangereq_end
- several implementation of this trick has been done
  - none support rangereq_end tho
  - the usual stream.php handle rangereq_beg with "pos" or "position"
  - lighttpd handled rangereq_beg with "start" and "pos"
  - rich media project handles rangereq_beg with "position"
  - This implementation support start/pos/position as alias 
    for "neoip_metavar_oload_flv_rangereq_beg"

\par How to deliver valid flv files
- First deliver a valid flv header for the movie and then deliver the data
- Second the first frame MUST be a key frame
  - This is handled by the player itself
  - the .flv video is supposed to have indexed all the keyframe byteoffset and
    time position
  - so when the player seeks it is able to convert the destination time to 
    the byteoffset of the closest keyframe
  - then it produce the url with the "?start=" and send that to the flv server

\par Links
- flv flash format
  - http://www.osflash.org/flv
- tool to index keyframe in .flv
  - flvmdi (window only) http://www.buraks.com/flvmdi/
  - flvtool2 (cross plateform) http://inlet-media.de/flvtool2
- Original description of the technique
  - http://www.flashcomguru.com/index.cfm/2005/11/2/Streaming-flv-video-via-PHP-take-two
- various players implementing this technic
  - http://www.jeroenwijering.com/?item=Flash_Video_Player
- various http server implementing this technic
  - lighttpd see http://blog.lighttpd.net/articles/2006/03/09/flv-streaming-with-lighttpd
  - appache2 see http://journal.paul.querna.org/articles/2006/07/11/mod_flvx
  - nginx    see http://blog.kovyrin.net/2006/10/14/flash-video-flv-streaming-nginx
  - Other uses a different way to point at the .flv files 
  

*/

/* system include */
/* local include */
#include "neoip_oload_mod_flv.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_oload_helper.hpp"

#include "neoip_bt_httpo_listener.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_http_resp_mode.hpp"

#include "neoip_http_uri.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_nested_uri.hpp"

#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
oload_mod_flv_t::oload_mod_flv_t()	throw()
{
	// zero some variable
	m_oload_apps	= NULL;
	m_httpo_resp	= NULL;
	m_type		= oload_mod_type_t::FLV;
}

/** \brief Destructor
 */
oload_mod_flv_t::~oload_mod_flv_t()		throw()
{
	// unlink this object from the attached oload_apps_t
	if( m_oload_apps )	oload_apps()->mod_unlink(this);
	// delete the bt_httpo_resp_t if needed
	nipmem_zdelete	m_httpo_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	oload_mod_flv_t::start(oload_apps_t *m_oload_apps)	throw()
{
	bt_err_t	bt_err;
	// copy the parameters
	this->m_oload_apps	= m_oload_apps;
	// link this object to the attached oload_apps_t
	oload_apps()->mod_dolink(this);

	// start the bt_httpo_resp_t
	m_httpo_resp	= nipmem_new bt_httpo_resp_t();
	bt_err		= m_httpo_resp->start(oload_apps()->httpo_listener(), "http://0.0.0.0/flv"
					, http_resp_mode_t::ACCEPT_SUBPATH, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_httpo_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
			
/** \brief callback notified by \ref bt_httpo_resp_t when to notify an event
 */
bool	oload_mod_flv_t::neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw()
{
	// log to debug
	KLOG_ERR("enter httpo_event=" << httpo_event);

	// sanity check - the bt_httpo_event_t MUST BE is_resp_ok()
	DBG_ASSERT( httpo_event.is_resp_ok() );
	// sanity check - the bt_httpo_event_t MUST BE a CNX_ESTABLISHED
	DBG_ASSERT( httpo_event.is_cnx_established() );

	// get variable from the bt_httpo_full_t
	bt_httpo_full_t *	httpo_full	= httpo_event.get_cnx_established();

	// put the oload_mod_vapi_t pointer in the userptr
	httpo_full->userptr(this);
	
	// call the oload_apps processing for post etablishement
	oload_apps()->httpo_full_post_esta(httpo_full);
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			oload_mod_vapi_t 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement the pre_itor_hook
 * 
 * - if any of the rangereq_beg/rangereq_end variables are present, a range is created
 *   inside the http_reqhd_t to simulate a range http request
 * - nevertheless flash-player expect a http_status_t::OK (200) and not a 
 *   http_status_t::PARTIAL_CONTENT (206) as for normal range http request
 *   - so a outter_var "use_flv_rangereq" is inserted to let oload_swarm_t
 *     handles this special case 
 */
bt_err_t	oload_mod_flv_t::pre_itor_hook(bt_httpo_full_t *httpo_full)	const throw()
{
	http_uri_t	req_uri		= httpo_full->http_reqhd().uri();
	file_size_t	rangereq_beg;
	file_size_t	rangereq_end;
	
	// log to debug
	KLOG_ERR("enter http_reqhd=" << httpo_full->http_reqhd());
	
	// extract the rangereq_beg/end value from the req_uri from neoip_metavar_*
	// - "neoip_metavar_oload_flv_rangereq_beg" to mark rangereq_beg 
	// - "neoip_metavar_oload_flv_rangereq_end" to mark rangereq_end
	if( req_uri.var().contain_key("neoip_metavar_oload_flv_rangereq_beg") ){
		size_t	key_idx	= req_uri.var().first_key_idx("neoip_metavar_oload_flv_rangereq_beg");
		rangereq_beg	= file_size_t::from_str(req_uri.var()[key_idx].val());
		req_uri.var().remove(key_idx);
	}
	if( req_uri.var().contain_key("neoip_metavar_oload_flv_rangereq_end") ){
		size_t	key_idx	= req_uri.var().first_key_idx("neoip_metavar_oload_flv_rangereq_end");
		rangereq_end	= file_size_t::from_str(req_uri.var()[key_idx].val());
		req_uri.var().remove(key_idx);
	}
	
	// handle aliases for "neoip_metavar_oload_flv_rangereq_beg"
	// - other flv servers are using the same trick but with other variables name
	//   - they handle only rangereq_beg, not rangereq_end
	// - "pos" for the usual stream.php to mark the rangereq_beg
	// - "start" for lighttpd which uses 'pos' or 'start' to mark the rangereq_beg
	// - "position" for rich media project to mark the rangereq_beg
	if( req_uri.var().contain_key("start") ){
		size_t	key_idx	= req_uri.var().first_key_idx("start");
		rangereq_beg	= file_size_t::from_str(req_uri.var()[key_idx].val());
		req_uri.var().remove(key_idx);
	}else if( req_uri.var().contain_key("pos") ){
		size_t	key_idx	= req_uri.var().first_key_idx("pos");
		rangereq_beg	= file_size_t::from_str(req_uri.var()[key_idx].val());
		req_uri.var().remove(key_idx);
	}else if( req_uri.var().contain_key("position") ){
		size_t	key_idx	= req_uri.var().first_key_idx("position");
		rangereq_beg	= file_size_t::from_str(req_uri.var()[key_idx].val());
		req_uri.var().remove(key_idx);
	}

	// if this http_reqhd_t doesnt contain any variable for flv_rangereq, return now
	if( rangereq_beg.is_null() && rangereq_end.is_null() )	return bt_err_t::OK;
	
	// if rangereq_beg is no specified, set it to 0
	if( rangereq_beg.is_null() )	rangereq_beg	= 0;
	// if rangereq_end is no specified, set it to file_size_t::MAX
	if( rangereq_end.is_null() )	rangereq_end	= file_size_t::MAX;
	
	// put the use_flv_rangereq variable in the outter_uri
	// - this is used to avoid answering with a http_status_t::PARTIAL_CONTENT (206)
	//   and instead use a http_status_t::OK (200)
	http_uri_t	inner_uri	= oload_helper_t::parse_inner_uri(req_uri);
	http_uri_t	outter_uri	= oload_helper_t::parse_outter_uri(req_uri);
	outter_uri.var().append("use_flv_rangereq", "dummy");
	http_uri_t	new_uri		= http_nested_uri_t::build(outter_uri, inner_uri);

	// build the new_reqhd
	http_reqhd_t	new_reqhd	= httpo_full->http_reqhd();
	new_reqhd.range	(file_range_t(rangereq_beg, rangereq_end));
	new_reqhd.uri	(new_uri);
	// replace bt_httpo_full_t::http_reqhd() with new_reqhd
	httpo_full->http_reqhd	( new_reqhd	);

	// log to debug
	KLOG_ERR("leave http_reqhd=" << new_reqhd);
	
	// return no error
	return bt_err_t::OK;
}

/** \brief return the prefix_header
 */
datum_t	oload_mod_flv_t::prefix_header(const oload_swarm_t *oload_swarm
					, const bt_httpo_full_t *httpo_full)	const throw()
{
	const http_reqhd_t &	http_reqhd	= httpo_full->http_reqhd();
	file_range_t		req_range	= http_reqhd.range();
	// log to debug
	KLOG_ERR("enter req_uri=" << http_reqhd.uri());

	// if there is no req_range, return an empty prefix header
	if( req_range.is_null() )	return datum_t();
	// if req_range.beg() <= 0, return an empty prefix header
	if( req_range.beg() <= 0 )	return datum_t();

	// if there is a req_range with beg() > 0, return a default header for FLV
	// - see http://www.osflash.org/flv for flv file format
	// - TODO should i use the flv_tophd_t object ?
	#define FLV_HEADER "FLV\x1\x1\0\0\0\x9\0\0\0\x9"	// magic header
	return	datum_t(FLV_HEADER, sizeof(FLV_HEADER)-1);
}

NEOIP_NAMESPACE_END





