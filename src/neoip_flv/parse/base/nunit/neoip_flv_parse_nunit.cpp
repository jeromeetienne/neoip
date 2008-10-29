/*! \file
    \brief Definition of the unit test for the \ref flv_parse_t

*/

/* system include */
/* local include */
#include "neoip_flv_parse_nunit.hpp"
#include "neoip_flv.hpp"
#include "neoip_dvar.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
flv_parse_testclass_t::flv_parse_testclass_t()	throw()
{
	// zero some field
	flv_parse	= NULL;
	file_aio	= NULL;
	file_aread	= NULL;
}

/** \brief Destructor
 */
flv_parse_testclass_t::~flv_parse_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	flv_parse_testclass_t::neoip_nunit_testclass_init()	throw()
{
	file_path_t	file_path;
	file_err_t	file_err;
	// get the file_path_t for the FLV file to read
//	file_path	= "/home/jerome/public_html/golfers.flv";
//	file_path	= "/home/jerome/public_html/hypo_from_vgoogle.flv";
//	file_path	= "/tmp/novell.flv";
	file_path	= "/home/jerome/public_html/output.flv";
//	file_path	= "/home/jerome/public_html/output2.flv";

	// get the size of the file
	file_size	= file_stat_t(file_path).get_size();
	// init the read_size
	read_size	= 0;

	// open the flv_parse_t
	flv_err_t	flv_err;
	flv_parse	= nipmem_new flv_parse_t();
	flv_err		= flv_parse->start(this, NULL);
	if( flv_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, flv_err.to_string() );
	
	// start a file_aio_t
	file_aio	= nipmem_new file_aio_t();
	file_err	= file_aio->start(file_path, file_mode_t::READ);
	if( file_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, file_err.to_string() );
	
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	flv_parse_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete variables if needed
	nipmem_zdelete	flv_parse;
	nipmem_zdelete	file_aread;
	nipmem_zdelete	file_aio;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	flv_parse_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// launch the first read
	flv_err_t	flv_err;
	flv_err		= launch_next_read();
	NUNIT_ASSERT( flv_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

/** \brief Launch the next file_aread_t (or nothing if the whole file has been read)
 */
flv_err_t	flv_parse_testclass_t::launch_next_read()	throw()
{
	file_err_t	file_err;
	// sanity check - file_aread MUST be NULL
	DBG_ASSERT( !file_aread );
	
	// if all the file has been read, do nothing
	if( file_size == read_size )	return flv_err_t::OK;
	
	// determine the chunk_size
	file_size_t	chunk_size	= 5120;
	chunk_size	= std::min(chunk_size, file_size - read_size);
	// start a file_aread_t
	file_aread	= nipmem_new file_aread_t(file_aio);
	file_err	= file_aread->start(read_size, chunk_size, this, NULL);
	if( file_err.failed() )		return flv_err_t(flv_err_t::ERROR,file_err.to_string());

	// return no error
	return flv_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			file_aread_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	flv_parse_testclass_t::neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
			, const file_err_t &file_err, const datum_t &read_data)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err);

	// notify nunit depending on the result
	if( file_err.failed() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;	
	}

	// update read_size
	read_size	+= read_data.size();

	// feed the file_chunk to the flv_parse_t
	flv_parse->notify_data(read_data);

	// delete the file_aread_t
	nipmem_zdelete	file_aread;

	// launch the next read
	flv_err_t	flv_err;
	flv_err		= launch_next_read();
	if( flv_err.failed() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_parse_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref flv_parse_t when to notify an event
 */
bool	flv_parse_testclass_t::neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << parse_event);
	
	// if flv_parse_event_t is fatal, report an error
	if( parse_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// compute the amount of unparsed data in flv_parse->buffer()
	DBG_ASSERT( flv_parse->buffer().size() >= parse_event.byte_length() );
	size_t	unparsed_len	= flv_parse->buffer().size() - parse_event.byte_length();
	// if unparsed_len == 0 and no more data to read, leave asap
	// - do a -4 because it should point to the flv_taghd_t::type and 
	//   not the flv_taghd_t::prevtag_size which is an uint32_t
	//   - this flv_taghd_t::prevtag_size crap is as well in 
	//     flv_parse_helper_t::kframe_boffset_from(), in the flv_parse_t nunit
	//     and neoip-flvfilter 
	//   - i dont like it, seems like a bug in the flv_parse_t architechture
	//   - it is spreaded everywhere, likely a misunderstanding of the flv format
	if( unparsed_len <= sizeof(uint32_t) && file_size == read_size ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// if this parse_event_t is a tag flv_tagtype_t::META, display the AMF it contains
	if( parse_event.is_tag() ){
		datum_t		tag_data;
		flv_taghd_t	flv_taghd	= parse_event.get_tag(&tag_data);
		if( flv_taghd.type() == flv_tagtype_t::META ){
			flv_err_t	flv_err;
			bytearray_t	bytearray(tag_data);
			dvar_t		event_name_dvar;
			dvar_t		event_data_dvar;
			// log to debug
			KLOG_ERR("META DATA:");
			// parse the tag_data as AMF data
			flv_err	= amf0_parse_t::amf_to_dvar(bytearray, event_name_dvar);
			DBG_ASSERT( flv_err.succeed() ); 
			KLOG_ERR("event_name="	<< event_name_dvar);
			flv_err	= amf0_parse_t::amf_to_dvar(bytearray, event_data_dvar);
			DBG_ASSERT( flv_err.succeed() ); 
			KLOG_ERR("event_data="	<< event_data_dvar);
		}
	}
	
	
	// display the kframe_boffset if any
	file_size_t	kframe_boffset;
	kframe_boffset	= flv_parse_helper_t::kframe_boffset_from(flv_parse, parse_event);
	if( !kframe_boffset.is_null() )	KLOG_ERR("KEY frame at " << kframe_boffset);

	
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END

