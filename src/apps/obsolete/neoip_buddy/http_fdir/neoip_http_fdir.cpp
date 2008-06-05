/*! \file
    \brief Definition of the \ref http_fdir_t class

\par Brief Description
This module implements a directory browsing thru http

*/

/* system include */
/* local include */
#include "neoip_http_fdir.hpp"
#include "neoip_http_fdir_uri_var.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_httpd_handler.hpp"
#include "neoip_httpd_auth.hpp"
#include "neoip_file_dir.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_path.hpp"
#include "neoip_mimediag.hpp"
#include "neoip_thumb_factory.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_date.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_tcp_full.hpp"

NEOIP_NAMESPACE_BEGIN;

static const std::string	URL_DIR_SEPARATOR	= "/";
static const std::string	URL_ESCAPE_CHARSET	= 
				"<>#%\""	// the delims from rfc2396.2.4.3
				"{}|\\^[]`"	// the unwise from rfc2396.2.4.3
				" "		// the space  from rfc2396.2.4.3
				;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_fdir_t::http_fdir_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// init thumb_prefix...
	// - this stuff must allow to identify a filename as icon...
	thumb_prefix	= ".fdir_icon";
	// allocate the mimediag
	mimediag	= nipmem_new mimediag_t();
	// allocate the thumb_factory
	thumb_factory	= nipmem_new thumb_factory_t();
	// zero the httpd_auth_t
	httpd_auth	= NULL;
}

/** \brief Destructor
 */
http_fdir_t::~http_fdir_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// free the httpd_auth if needed
	if( httpd_auth )	nipmem_delete	httpd_auth;
	// free the mimediag
	nipmem_delete	mimediag;
	// free the thumb_factory
	nipmem_delete	thumb_factory;
	// remove the handler from httpd
	if( !url_rootpath.is_null() )
		lib_session_get()->get_httpd()->handler_del("/fdir", this, NULL, httpd_handler_t::DEEPER_PATH_OK);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the user database
 */
void	http_fdir_t::set_user_db(const std::map<std::string, std::string> &user_db)	throw()
{
	this->user_db	= user_db;
}

/** \brief start the operation
 * 
 * @return true on error, false otherwise
 */
bool	http_fdir_t::start(const std::string &url_rootpath, const file_path_t &file_rootpath
					, const std::string &root_alias)	throw()
{
	// copy the parameter
	this->url_rootpath	= url_rootpath;
	this->file_rootpath	= file_rootpath;
	// determine the root_alias
	if( !root_alias.empty() )	this->root_alias = root_alias;
	else				this->root_alias = "[root]";
	
	// set the httpd_auth_t with the root_alias as auth realm
	httpd_auth	= nipmem_new httpd_auth_t("File Browsing " + this->root_alias + " Directory"
							, "basic");
	
	// add the basic handler
	lib_session_get()->get_httpd()->handler_add(url_rootpath, this, NULL
							, httpd_handler_t::DEEPER_PATH_OK);	
	// return no error
	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief The httpd_auth_t functor which return true if the username/password is allowed, false otherwise
 */
struct http_fdir_t::fdir_auth_get_pass_ftor_t : public httpd_auth_t::get_pass_ftor_t {
private:http_fdir_t *	http_fdir;
	std::string	loggedin_username, loggedin_password;
public: fdir_auth_get_pass_ftor_t(http_fdir_t *http_fdir)	throw() : http_fdir(http_fdir) {}
	const std::string &	get_username()	const throw()	{ return loggedin_username;	}
	const std::string &	get_passwd()	const throw()	{ return loggedin_password;	}
	bool operator()(const std::string &username, const httpd_auth_t &cb_httpd_auth
					, const httpd_request_t &cb_httpd_request
					, std::string &passwd_out) throw() {
		std::map<std::string, std::string>::const_iterator	iter;
		// log to debug
		KLOG_DBG("test username=" << username << " password=" << passwd_out);
		// try to find the username in the user_db
		iter = http_fdir->user_db.find(username);
		// if the username is not in the database, return false
		if( iter == http_fdir->user_db.end() )	return false;
		// copy the password to the passwd_out
		passwd_out	= iter->second;
		// copy the username/passwd to the local var 
		// - used to determine who is logged to this httpd_fdir_t
		loggedin_username	= username;
		loggedin_password	= passwd_out;
		// return true
		return true;
	}
};

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t http_fdir_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw()
{
	// log to debug
	KLOG_ERR("request method=" << request.get_method() << " path=" << request.get_path() );

	// sanity check - the request path MUST be the url_rootpath or deepeer
	DBG_ASSERT( request.get_path().substr(0, url_rootpath.to_string().size()) == url_rootpath.to_string() );
	
	// check if this request is allowed via the httpd authentication
	if( user_db.size() ){
		fdir_auth_get_pass_ftor_t	ftor	= fdir_auth_get_pass_ftor_t(this);
		httpd_err_t	httpd_err = httpd_auth->is_allowed(request, ftor);
		if( httpd_err != httpd_err_t::OK )	return httpd_err;
	}

	if( request.get_method() == "POST" && request.get_path() == "/buddy/share/info/upload.cgi"){
		KLOG_ERR("request header=" << request.get_header_property() );
		KLOG_ERR("posted data=" << string_t::from_datum(request.get_posted_data()) );
		return httpd_err_t::NOT_FOUND;
	}
	
	std::string	url_relpath_str	= "." + request.get_path().substr(url_rootpath.to_string().size());
	file_path_t	url_relpath	= string_t::unescape(url_relpath_str);
	file_path_t	file_relpath	= url_relpath;

	// test if it is an thumbnail
	if( relpath_is_thumb(file_relpath) )	return handle_thumb(request, file_relpath);

	// build the file_fullpath
	file_path_t	file_fullpath	= file_rootpath / file_relpath;

	KLOG_ERR("file_fullpath=" << file_fullpath);
	// test if the file exist
	file_stat_t	file_stat = file_stat_t(file_fullpath);
	// if the file is not found, report it as NOT_FOUND
	if( file_stat.is_null() )		return httpd_err_t::NOT_FOUND;

	
	// if the file_fullpath points to a directory, display it
	if( file_stat.is_dir() ){
		httpd_err_t	httpd_err;
		// get the uri_var from the httpd_request_t
		uri_var_t	uri_var(request);
		// put the page dependsing on the dir_view_str
		if( uri_var.dir_view() == "plainlist" || uri_var.dir_view() == "thumblist" ){
			httpd_err = handle_view_list(request, file_relpath);
		}else if( uri_var.dir_view() == "thumbnail" ){
			return handle_view_thumbnail(request, file_relpath);
		}else if( uri_var.dir_view() == "imageshow" ){
			return handle_view_imageshow(request, file_relpath);
		}else if( uri_var.dir_view() == "getplaylist" ){
			return handle_view_getplaylist(request, file_relpath);
		}else{
			return httpd_err_t::NOT_FOUND;
		}
		// if no error occured, put the html_tail
		if( httpd_err == httpd_err_t::OK )	put_html_tail(request, file_relpath);
		// return the error
		return httpd_err;
	}
	// if the file_fullpath points to a regular file, reply it
	if( file_stat.is_regular_file() )	return handle_file_direct(request, file_relpath);

	// if this point is reached, return NOT_FOUND
	return httpd_err_t::NOT_FOUND;
}

/** \brief Add non-space-break
 * 
 * - for long string which doesnt contains any space
 */
std::string	http_fdir_t::add_nonspacebreak(const std::string &str, size_t max_len)	 const throw()
{
//	std::string	nsb_str	= "&#x200B;";
	std::string	nsb_str = "<img width=\"0\" border=\"0\">";
	std::string	result;
	for( size_t i = 0; i < str.size(); i += max_len ){
		// copy the string
		result	+= str.substr(i, max_len);
		// add the nonspacebreak
		result	+= nsb_str;
	}
	// return the result
	return result;
}

/** \brief Return true if the request comes from a text browser, false otherwise
 * 
 * - NOTE: this is an estimation due to heuristic parsing User-Agent
 */
bool	http_fdir_t::is_text_browser(const httpd_request_t &request)		const throw()
{
	// display to debug
	KLOG_DBG("header=" << request.get_header_property() );
	// get the User-Agent request header
	std::string	useragent_str	= request.get_header("User-Agent");
	// if there no such header, return false to default to the common case aka graphical browser
	if( useragent_str.empty() )	return false;

	// lowerize the useragent_str
	useragent_str	= string_t::to_lower(useragent_str);
	// if the useragent_str contains "elinks", it is estimated as text browser 
	if( string_t::glob_match("*elinks*", useragent_str ) )	return true;
	// if the useragent_str contains "lynx", it is estimated as text browser 
	if( string_t::glob_match("*lynx*", useragent_str ) )	return true;
	// else it is default as a graphical browser
	return false;
}


/** \brief Add the html header
 */
void	http_fdir_t::put_html_head(httpd_request_t &request
					, const file_path_t &dir_relpath) 	throw()
{
	std::ostringstream	&oss	= request.get_reply();
	
	oss << "<html>";
	oss << "<head>";
	oss << "<style type=\"text/css\">";
	oss << "div.navigation_header";
	oss << "{";
	oss << "background: #bbccff;";
	oss << "border: solid 1px #9999ff;";
	oss << "margin-top: 0;";
	oss << "margin-bottom: 0;";
	oss << "position: fixed;";
	oss << "width: 100%;";
	oss << "top: 0;";
	oss << "left: 0;";
	oss << "height: 5em;";
	oss << "z-index: 1;";
	oss << "}";
	oss << "div.navigation_dir";
	oss << "{";
	oss << "position: relative;";
	oss << "top: 6em;";
	oss << "z-index: 0;";
	oss << "}";
	oss << "body {background-color: #ddddee}";
	oss << "</style>";
	oss << "</head>";
	oss << "<body>";
	
	
	// display the path header
	oss << "<div class=\"navigation_header\">";

	oss << h.s_table("width=\"100%\" cellpadding=\"0\" cellspacing=\"0\"");
	oss << h.s_tr();
	oss << h.s_td("align=\"left\"") << dir_view_html(request, dir_relpath)		<< h.e_td();
	oss << h.s_td("align=\"left\"")	<< show_hidden_html(request, dir_relpath)	<< h.e_td();
	oss << h.e_tr();
	oss << h.s_tr();
	oss << h.s_td();
	oss <<	"<form action=\"" << url_rootpath / "upload.cgi" << "\" method=\"post\" enctype=\"multipart/form-data\">";
	oss <<  "<label for=\"file\"><b>Upload: </b></label>";
	oss <<  "<input type=\"file\" name=\"file\" id=\"file\" />";
	oss <<  "<input type=\"submit\" name=\"submit\" value=\"Upload\" />";
	oss <<  "</form>";
	
	oss << h.e_td();
	oss << h.s_td("align=\"left\"")	<< dir_order_html(request, dir_relpath)		<< h.e_td();
	oss << h.s_tr();
	oss << h.s_td("align=\"left\"")	<< path_selector_html(request, dir_relpath)	<< h.e_td();
	oss << h.s_td("align=\"left\"");
	// TODO to refactor to avoid thehardcoded url :)
	oss 		<< "<form action=\"/search\" method=\"get\"> "
				<< "Search: "
				<< "<input type=\"text\" name=\"search_keyword\" style=\"background:#ddddee;\">"
				<< "</form>";
	oss	 << h.e_td();
	
	oss << h.e_tr();
	oss << h.e_table();

	oss << "</div>";	
}

/** \brief Add the html tail
 */
void	http_fdir_t::put_html_tail(httpd_request_t &request
					, const file_path_t &dir_relpath) 	throw()
{
	std::ostringstream	&oss	= request.get_reply();
	oss << "</body>";
	oss << "</html>";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   Some local file_dir_t::filter_ftor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Functor to filter out all files which mimetype is not an image
 */
struct filter_non_image : public file_dir_t::filter_ftor {
private:mimediag_t *	mimediag;
public:	filter_non_image(mimediag_t *mimediag) 		throw() : mimediag(mimediag) {}
	bool operator()(const file_path_t &file_path)	const throw() {
		std::string	mimetype	= mimediag->from_file(file_path);
		// if the filter match, return false
		if( string_t::glob_match("image/*", mimetype) )	return false;
		return true;
	}
};

/** \brief Functor to filter out all files which mimetype is not an audio
 */
struct filter_non_audio : public file_dir_t::filter_ftor {
private:mimediag_t *	mimediag;
public:	filter_non_audio(mimediag_t *mimediag) 		throw() : mimediag(mimediag) {}
	bool operator()(const file_path_t &file_path) 	const throw() {
		std::string	mimetype	= mimediag->from_file(file_path);
		// if the filter match, return false
		if( string_t::glob_match("audio/*", mimetype) )		return false;
		if( string_t::glob_match("application/ogg", mimetype) )	return false;
		return true;
	}
};

/** \brief Functor to filter out all files which mimetype is not an video
 */
struct filter_non_video : public file_dir_t::filter_ftor {
private:mimediag_t *	mimediag;
public:	filter_non_video(mimediag_t *mimediag)		throw() : mimediag(mimediag) {}	
	bool operator()(const file_path_t &file_path) 	const throw() {
		std::string	mimetype	= mimediag->from_file(file_path);
		// if the filter match, return false
		if( string_t::glob_match("video/*", mimetype) )		return false;
		return true;
	}
};

/** \brief Functor to filter out all files which mimetype is not an audio/video
 */
struct filter_non_audiovideo : public file_dir_t::filter_ftor {
private:mimediag_t *	mimediag;
public:	filter_non_audiovideo(mimediag_t *mimediag)		throw() : mimediag(mimediag) {}	
	bool operator()(const file_path_t &file_path) 	const throw() {
		std::string	mimetype	= mimediag->from_file(file_path);
		// if the filter match, return false
		if( string_t::glob_match("video/*", mimetype) )		return false;
		if( string_t::glob_match("audio/*", mimetype) )		return false;
		if( string_t::glob_match("application/ogg", mimetype) )	return false;		
		return true;
	}
};



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  handle_view_list
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



/** \brief handle a directory via text
 */
httpd_err_t	http_fdir_t::handle_view_list(httpd_request_t &request
					, const file_path_t &dir_relpath) 	throw()
{
	std::ostringstream	&oss		= request.get_reply();
	file_path_t		dir_fullpath	= file_rootpath / dir_relpath;	
	uri_var_t		uri_var(request);
	file_dir_t		file_dir;
	bool			w_nonspacebreak	= is_text_browser(request) ? false : true;

	// sanity check - the dir_view_str MUST be plainlist or thumblist
	DBG_ASSERT( uri_var.dir_view() == "plainlist" || uri_var.dir_view() == "thumblist" );

	// get all the name in this dirname
	file_err_t		file_err	= file_dir.open(dir_fullpath);
	if( file_err.failed() )	return httpd_err_t::NOT_FOUND;
	// sort the result 
	file_dir.sort("d" + uri_var.dir_order());
	
	// filter the hidden file if needed
	if( uri_var.show_hidden() == "n" )	file_dir.filter(file_dir_t::filter_hidden());
	
	// put the html head
	put_html_head(request, dir_relpath);
	// start the navigation_dir
	oss << "<div class=\"navigation_dir\">";
	oss << h.s_table("width=\"100%\"");

	// display the directories
	for( size_t i = 0; i < file_dir.size(); i++ ){
		const file_stat_t &file_stat	= file_dir.get_file_stat(i);
		std::string	url_path	= get_url_path(request, dir_relpath / file_dir.basename(i) );
		std::string	row_style;
		if( i % 2 )	row_style	= "style=\"background:#ddddee;\"";
		else		row_style	= "style=\"background:#d4d4ee;\"";

		// compute the displayed filename
		std::string	disp_filename	= file_dir.basename(i).to_string() + (file_stat.is_dir() ? URL_DIR_SEPARATOR : "");
		if( w_nonspacebreak )	disp_filename = add_nonspacebreak(disp_filename, 5);

		
		if( uri_var.dir_view() == "thumblist" )	oss << h.s_tr(row_style + " height=\"48\"");
		else					oss << h.s_tr(row_style);
		oss << h.s_td("width=\"60%\"");
		// add the link with the anchor definition

		if( uri_var.dir_view() == "thumblist" ){
			file_path_t	file_fullpath	= dir_fullpath / file_dir.basename(i);
			file_path_t	thumb_path	= thumb_factory->get_thumb_name(file_fullpath);
			oss << h.s_table("") << h.s_tr();
			oss << h.s_td("align=\"right\"");
			// TODO do a s_img in the html_builder_t
			oss	 << h.s_link(url_path);
			oss	 	<< "<img border=\"0\" width=\"48\" src=\"";
			oss	 		<< get_url_path(request, thumb_prefix / thumb_path);
			oss	 		<< "\">";
			oss		<< h.e_link();
			oss 	<< h.e_td();
			oss << h.s_td("align=\"left\"");
			oss	<< h.s_link(url_path, "name=\""+ file_dir.basename(i).to_string() + "\"");
			oss 		<< disp_filename;
			oss 		<< h.e_link();
			oss 	<< h.e_td();
			oss << h.e_tr() << h.e_table();
		}else{
			oss	<< h.s_link(url_path, "name=\""+ file_dir.basename(i).to_string() + "\"");
			oss 		<< disp_filename;
			oss 		<< h.e_link();			
		}
		oss << h.e_td();

		oss << h.s_td("align=\"right\" width=\"10%\"")
				<< string_t::size_string(file_stat.get_size()) << h.e_td();

		oss << h.s_td("align=\"right\" width=\"20%\"")	
				<< file_stat.get_mtime() << h.e_td();

		oss << h.s_td("width=\"10%\"")
				<< mimediag->from_file(file_path_t(file_dir[i])) << h.e_td();

		oss << h.e_tr();
	}
	oss << h.e_table();

	oss << "</div>";

	// return no error
	return httpd_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  handle_view_icon
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle a directory via thumbmail
 */
httpd_err_t	http_fdir_t::handle_view_thumbnail(httpd_request_t &request
					, const file_path_t &dir_relpath)	throw()
{
	std::ostringstream	&oss		= request.get_reply();
	file_path_t		dir_fullpath	= file_rootpath / dir_relpath;
	size_t			row_len		= 4;
	uri_var_t		uri_var(request);
	file_dir_t		file_dir;	
	bool			w_nonspacebreak	= is_text_browser(request) ? false : true;

	// sanity check - the dir_view_str MUST be thumbnail
	DBG_ASSERT( uri_var.dir_view() == "thumbnail" );

	// get all the name in this dirname
	file_err_t		file_err	= file_dir.open(dir_fullpath);
	if( file_err.failed() )	return httpd_err_t::NOT_FOUND;

	// sort the result 
	file_dir.sort("d" + uri_var.dir_order());

	// filter the hidden file if needed
	if( uri_var.show_hidden() == "n" )	file_dir.filter(file_dir_t::filter_hidden());

	// put the html head
	put_html_head(request, dir_relpath);
	// start the navigation_dir
	oss << "<div class=\"navigation_dir\">";
	oss << h.s_table("width=\"100\%\"");
	// display the directories
for( size_t k = 0; k < file_dir.size(); k += row_len){
	// start the row for the thumbnails
	oss << h.s_tr();
	// fill the row
	for( size_t j = 0; j < row_len; j++ ){
		size_t	i = k+j;
		// Start the cells
		oss << h.s_td("align=\"middle\" valign=\"bottom\" width=\"25%\" height=\"150\"");;
		if( i < file_dir.size() ){
			std::string	url_path	= get_url_path(request, dir_relpath / file_dir.basename(i) );
			file_path_t	file_fullpath	= dir_fullpath / file_dir.basename(i);
			file_path_t	thumb_path	= thumb_factory->get_thumb_name(file_fullpath);
			// put the thumbnail
			oss	<< h.s_link(url_path, "name=\""+ file_dir.basename(i).to_string() + "\"")
					<< "<img border=\"0\" width=\"128\" src=\""
						<< get_url_path(request, thumb_prefix / thumb_path)
						<< "\">"
					<< h.e_link();
		}
		// end the cells
		oss << h.e_td();
	}
	oss << h.e_tr();
	// start the row for the names 
	oss << h.s_tr();
	// fill the row
	for( size_t j = 0; j < row_len; j++ ){
		size_t	i = k+j;

		// Start the cells
		oss << h.s_td("align=\"middle\" valign=\"top\" width=\"25%\"");;
		if( i >= file_dir.size() ){
			oss << h.e_td();
			continue;
		}
		
		// compute url_path
		std::string	url_path	= get_url_path(request, dir_relpath / file_dir.basename(i) );
		// compute the displayed filename
		const file_stat_t &file_stat	= file_dir.get_file_stat(i);
		std::string	disp_filename	= file_dir.basename(i).to_string() + (file_stat.is_dir() ? URL_DIR_SEPARATOR : "");
		if( w_nonspacebreak )	disp_filename = add_nonspacebreak(disp_filename, 5);
		
		// put the text below the thumbnail
		oss	<< h.s_link(url_path, "name=\""+ file_dir.basename(i).to_string() + "\"")
				<< disp_filename
				<< h.e_link();
		// end the cells
		oss << h.e_td();
	}
	oss << h.e_tr();
}

	oss << h.e_table();	

	oss << "</div>";

	// return no error
	return httpd_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  handle_view_imageshow
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief View a directory as a image show
 */
httpd_err_t	http_fdir_t::handle_view_imageshow(httpd_request_t &request
					, const file_path_t &dir_relpath) 	throw()
{
	std::ostringstream	&oss		= request.get_reply();
	file_path_t		dir_fullpath	= file_rootpath / dir_relpath;	
	uri_var_t		uri_var(request);
	file_dir_t		file_dir;	
	bool			w_nonspacebreak	= is_text_browser(request) ? false : true;

	// sanity check - the dir_view_str MUST be imageshow
	DBG_ASSERT( uri_var.dir_view() == "imageshow" );

	// get all the name in this dirname
	file_err_t		file_err	= file_dir.open(dir_fullpath);
	if( file_err.failed() )	return httpd_err_t::NOT_FOUND;
	// sort the result 
	file_dir.sort("d" + uri_var.dir_order());

	// remove all file_dir_t elements which not an image
	file_dir.filter(filter_non_image(mimediag));

	// put the html head
	put_html_head(request, dir_relpath);
	// Start the navigation_dir division
	oss << "<div class=\"navigation_dir\">";

	// if there are no image in this directory, notify the user and exist
	if( file_dir.size() == 0 ){
		oss << h.s_b() << "No image in this directory" << h.e_b();
		oss << "</div>";
		return httpd_err_t::OK;
	}

	// get the image_idx from the request variable
	std::string	image_idx_str	= request.get_variable("image_idx", "0");
	size_t		image_idx	= atoi(image_idx_str.c_str());

	// if the image index is >= that the number of file in file_dir, set it to file_dir.size()
	if( image_idx >= file_dir.size() )	image_idx = file_dir.size() - 1;

	// compute the url_dirpath
	std::string	url_dirpath	= get_url_path(request, dir_relpath);
	// compute the url_filepath
	std::string	url_filepath	= get_url_path(request, dir_relpath / file_dir.basename(image_idx));
	
	
// put the cursors
	oss << h.s_table("width=\"100%\"") << h.s_tr();
	// put the "First" cursor
	oss << h.s_td("align=\"left\"") << h.s_link(url_dirpath + "&image_idx=0")
						<< "First" << h.e_link() << h.e_td();
	// put the "Previous / Next" cursor
	oss << h.s_td("align=\"center\"");
	if( image_idx )				oss << h.s_link(url_dirpath + "&image_idx="+OSTREAMSTR(image_idx-1));
	oss << "Previous";
	if( image_idx )				oss << h.e_link();
	oss << " / ";
	if( image_idx+1 < file_dir.size())	oss << h.s_link(url_dirpath + "&image_idx="+OSTREAMSTR(image_idx+1));
	oss << "Next";
	if( image_idx+1 < file_dir.size())	oss << h.e_link();
	
	oss << h.e_td();
	// put the "Last" cursor
	oss << h.s_td("align=\"right\"") << h.s_link(url_dirpath + "&image_idx="+OSTREAMSTR(file_dir.size()))
						<< "Last" << h.e_link() << h.e_td();
	oss << h.e_tr() << h.e_table();

	std::string	disp_filename	= file_dir.basename(image_idx).to_string();
	if( w_nonspacebreak )	disp_filename = add_nonspacebreak(disp_filename, 5);
	
// put the image
	oss << h.s_table("width=\"100%\"") << h.s_tr() << h.s_td("align=\"center\"");
	oss	<< h.s_link(url_filepath);
	oss 		<< "<img border=\"0\" width=\"55%\" src=\""
				<< get_url_path(request, dir_relpath / file_dir.basename(image_idx))
				<< "\">";	
	oss 	<< h.e_link();
	oss << h.e_td() << h.e_tr() << h.e_table();
	// put the Image filename
	oss << h.s_table("width=\"100%\"") << h.s_tr() << h.s_td("align=\"center\"");
	oss	<< h.s_link(url_filepath);
	oss 	<< disp_filename;
	oss 	<< h.e_link();
	oss << h.e_td() << h.e_tr() << h.e_table();

	// end of the navigation_dir division
	oss << "</div>";

	// return no error
	return httpd_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  handle_view_getplaylist
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the player list of a directory
 */
httpd_err_t	http_fdir_t::handle_view_getplaylist(httpd_request_t &request
					, const file_path_t &dir_relpath) 	throw()
{
	std::ostringstream	&oss		= request.get_reply();
	file_path_t		dir_fullpath	= file_rootpath / dir_relpath;	
	uri_var_t		uri_var(request);
	file_dir_t		file_dir;	

	// sanity check - the dir_view_str MUST be imageshow
	DBG_ASSERT( uri_var.dir_view() == "getplaylist" );

	// get all the name in this dirname
	file_err_t		file_err	= file_dir.open(dir_fullpath);
	if( file_err.failed() )	return httpd_err_t::NOT_FOUND;
	// sort the result 
	file_dir.sort("d" + uri_var.dir_order());

	// remove all file_dir_t elements which not an image
//	file_dir.filter(filter_non_audio(mimediag));
	file_dir.filter(filter_non_audiovideo(mimediag));


	// if there are no image in this directory, notify the user and exist
	if( file_dir.size() == 0 ){
		// put the html head
		put_html_head(request, dir_relpath);
		oss << "<div class=\"navigation_dir\">";
		oss << h.s_b() << "No audio in this directory" << h.e_b();
		oss << "</div>";
		return httpd_err_t::OK;
	}

	std::string	url_addr	= "http://";


/* NOTE:
 * - from here, there is a lot of kludge to build the playlist
 *   - moreover it is poorely coded
 * - the main issue is the non loopback nat
 *   - due to that, the destination address to put in the playlist is different
 *     depending from where the request come from.
 *   - if it comes from another host behind the same nat, the destination address
 *     is the local view of it
 *   - else it is the public view
 * - the estimation to determine if the requester host is behind the same nat
 *   is done with the source address of the http request
 *   - if it is a localhost or a private address, it comes from the same nat
 *   - else it doesnt
 */
	url_addr	+= "jerome:bonjour@";

	// build the url root of the link in the playlist
	ip_addr_t	remote_ip_addr = request.get_tcp_full()->get_remote_addr().get_ipaddr();
	KLOG_ERR("remote addr="<< remote_ip_addr);
	const property_t *	lib_property= lib_session_get()->get_property();
	std::string	local_listen_addr = lib_property->find_string("lib_httpd_listen");	
	std::string	public_listen_addr = lib_property->find_string("lib_httpd_public_listen");
	DBG_ASSERT( local_listen_addr.empty() );	
	if( public_listen_addr.empty() )	public_listen_addr = local_listen_addr;

	if( remote_ip_addr.is_localhost() || remote_ip_addr.is_private() ){
		// TODO here put the local_view of the httpd listen_addr
		KLOG_ERR("request from the private realm = "<< remote_ip_addr);
		url_addr	+= local_listen_addr;
	}else{
		// TODO here put the public_view of the httpd listen_addr
		KLOG_ERR("request from outside the private realm = "<< remote_ip_addr);
		ipport_addr_t	listen_addr = public_listen_addr;
		if( listen_addr.get_ipaddr().is_any() ){
			DBG_ASSERT(lib_property->find_string("public_ip_addr") != "");
			public_listen_addr	= lib_property->find_string("public_ip_addr") + ":" + OSTREAMSTR(listen_addr.get_port());
		}
		url_addr	+= public_listen_addr;
	}

	// display the directories
	for( size_t i = 0; i < file_dir.size(); i++ ){
		file_path_t	url_filepath	= url_rootpath / dir_relpath / file_dir.basename(i);
		std::string	url_str		= string_t::escape(url_filepath.to_string(), URL_ESCAPE_CHARSET);
		std::string	url_full	= url_addr + url_str;
		/** TODO here put the public view of the buddy http listen address
		 * - add the potential jerome:bonjour authentication
		 * - maybe to change the authentication to provide the username:passwd
		 *   passed in the http request
		 *   - instead of the functor
		 */
//		KLOG_ERR("audio file " << i << " is " << url_full);
		oss << url_full << std::endl;
	}
	
	// set the mime type of the file
	request.get_reply_mimetype() = "audio/x-mpegurl";
//	request.get_reply_mimetype() = "video/x-mpegurl";

	// return no error
	return httpd_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  ???
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle file requested by direct url
 */
httpd_err_t	http_fdir_t::handle_file_direct(httpd_request_t &request, const file_path_t &file_relpath) throw()
{
	// just forward it to the general file_download
	return download_file_fullpath(request, file_rootpath / file_relpath);
}

/** \brief Generic funtion to provide a file to an httpd_request_t
 */
httpd_err_t	http_fdir_t::download_file_fullpath(httpd_request_t &request
						, const file_path_t &file_path) throw()
{
	// set the mime type of the file
	request.get_reply_mimetype() = mimediag->from_file(file_path);
#if 1
	// return no error
	return httpd_err_t(httpd_err_t::SEND_FILE, file_path.to_string() );	
#else
/** LAME STANDALONE VERSION
 * 
 * - it read the WHOLE file in RAM
 * - and return it all in once
 */
	std::ostringstream	&oss	= request.get_reply();
	FILE *			fIn;
	int			read_len;
	char			tmp[1024];
	// open the file 
	fIn = fopen( file_path.to_string().c_str(), "rb" );
	if( fIn == NULL )	return httpd_err_t::INTERNAL_ERROR;
	// load the file
	while( (read_len = fread( &tmp, 1, sizeof(tmp), fIn )) ){
		oss << std::string(tmp, read_len);
	}
	// close the file
	fclose( fIn );
	
	// return no error
	return httpd_err_t::OK;	
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    url variable/state
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return a url_path pointing to file_relpath with the anchor_str
 */
std::string	http_fdir_t::get_url_path(httpd_request_t &request,const file_path_t &file_relpath
					, const std::string &anchor_str)	const throw()
{
	file_path_t	url_filepath	= url_rootpath / file_relpath;
	std::string	url_str		= string_t::escape(url_filepath.to_string(), URL_ESCAPE_CHARSET);
	// add the url_var
	url_str += "?" + uri_var_t(request).to_url();
	// add the anchor if present
	if( !anchor_str.empty() )
		url_str += "#" + anchor_str;
	// return the url_str
	return url_str;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  header
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put the path header html into the request
 */
std::string	http_fdir_t::path_selector_html(httpd_request_t &request
					, const file_path_t &file_relpath)	throw()
{
	std::ostringstream	oss;
	std::string		url_str;

	oss << h.s_table("width=\"100\%\"");

	// Start the "Path: " stuff
	oss << h.s_td("align=\"left\"")	<< h.s_b() << "Path: " << h.e_b();
	for( size_t i = 0; i < file_relpath.size(); i++ ){
		// build the anchor
		std::string	anchor;
		if(i != file_relpath.size()-1)	anchor = file_relpath.get_name_level(i+1).to_string();
		// build url_path
		url_str	= get_url_path(request, file_relpath[i], anchor);

		// add the inter name separator
		if( i != 0 )	oss << " / ";
		
		// add the link reference
		oss << h.s_link(url_str);
		if( i != 0 )	oss << file_relpath.get_name_level(i);
		else		oss << root_alias;
		oss << h.e_link();
	}
	oss << h.e_td();

	// Start the "Back" Stuff
	oss << h.s_td("align=\"right\"");
	// build url_path
	if(file_relpath.size() > 1){
		std::string	anchor = file_relpath.get_name_level(file_relpath.size()-1).to_string();
	 	url_str	=  get_url_path(request, file_relpath[file_relpath.size()-2], anchor);
	}else{
		url_str	=  get_url_path(request, file_path_t());
	}
	// add the link
	oss << h.s_link(url_str) << "Back" << h.e_link();
	oss << h.e_td() << h.e_table();
	
	// return the result
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  	dir_order variable
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put the dir order header html into the request
 */
std::string	http_fdir_t::dir_order_html(httpd_request_t &request, const file_path_t &file_relpath)	throw()
{
	std::ostringstream	oss;
	file_path_t		url_filepath	= url_rootpath / file_relpath;
	std::string		url_path	= string_t::escape(url_filepath.to_string(), URL_ESCAPE_CHARSET);
	uri_var_t		orig_uri_var(request);
	char *			order_list[][2]	= {
						 {"n", "name"},
						 {"s", "size"},
						 {"m", "time"},
						 };
	// display the title
	oss 	<< h.s_b() << "Order: " << h.e_b();
	// display an option for each possible view
	for( size_t i = 0; i < sizeof(order_list)/sizeof(order_list[0]); i++ ){
		std::string	order_str = order_list[i][0];
		// put the separator
		if( i != 0 )	oss	<< " | ";

		// if the original dir_order is this criteria, offer the negative		
		if( orig_uri_var.dir_order() == order_str )
			order_str	= string_t::to_upper(order_str);
		
		// put the link
		uri_var_t	copy_uri_var(orig_uri_var);
		copy_uri_var.dir_order()	= order_str;
		oss	<< h.s_link(url_path + "?" + copy_uri_var.to_url()) 
				<< order_list[i][1]
				<< h.e_link();
	}

	// return the result
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  	dir_view variable
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the html for the dir_view variable
 */
std::string	http_fdir_t::dir_view_html(httpd_request_t &request, const file_path_t &file_relpath)	throw()
{
	std::ostringstream	oss;
	file_path_t		url_filepath	= url_rootpath / file_relpath;
	std::string		url_path	= string_t::escape(url_filepath.to_string(), URL_ESCAPE_CHARSET);
	uri_var_t		orig_uri_var(request);
	char *			view_list[]	= { "plainlist"
							, "thumblist"
							, "thumbnail"
							, "imageshow"
							, "getplaylist" };
	
	// display the title
	oss 	<< h.s_b() << "View: " << h.e_b();
	// display an option for each possible view
	for( size_t i = 0; i < sizeof(view_list)/sizeof(view_list[0]); i++ ){
		std::string	cur_view	= view_list[i];
		// put the separator
		if( i != 0 )	oss	<< " | ";
		
		if( orig_uri_var.dir_view() != cur_view ){
			uri_var_t	copy_uri_var(orig_uri_var);
			copy_uri_var.dir_view()	= cur_view;
			oss	<< h.s_link(url_path + "?" + copy_uri_var.to_url()) 
					<< cur_view 
					<< h.e_link();
		}else{
			oss	<< cur_view;
		}
	}
	
	// return the result
	return oss.str();
}	


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  	show_hidden variable
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the html for the show_hidden variable
 */
std::string	http_fdir_t::show_hidden_html(httpd_request_t &request, const file_path_t &file_relpath)	throw()
{
	std::ostringstream	oss;
	file_path_t		url_filepath	= url_rootpath / file_relpath;
	std::string		url_path	= string_t::escape(url_filepath.to_string(), URL_ESCAPE_CHARSET);
	uri_var_t		orig_uri_var(request);

	// display the title
	oss 	<< h.s_b() << "Hidden file: " << h.e_b();
	if( orig_uri_var.show_hidden() == "n" ){
		uri_var_t	copy_uri_var(orig_uri_var);
		copy_uri_var.show_hidden()	= "y";
		oss	<< h.s_link(url_path + "?" + copy_uri_var.to_url()) 
				<< "show"
				<< h.e_link();
	}else{
		uri_var_t	copy_uri_var(orig_uri_var);
		copy_uri_var.show_hidden()	= "n";
		oss	<< h.s_link(url_path + "?" + copy_uri_var.to_url()) 
				<< "hide"
				<< h.e_link();
	}

	// return the result
	return oss.str();
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  	thumbnail management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true if the relpath is an thumbnail file or not
 */
bool	http_fdir_t::relpath_is_thumb(const file_path_t &file_relpath) const throw()
{
	// if the file_relpath isnt deep enougth to be a thumbnail, return false
	if( file_relpath.size() < 3 )				return false;
	// if the thumb_prefix cant be found at the proper place, return false
	if( file_relpath.get_name_level(1) != thumb_prefix )	return false;
	// in all other cases, it is a thumbnail
	return true;
}


/** \brief handle url for thumbnail
 */
httpd_err_t	http_fdir_t::handle_thumb(httpd_request_t &request
					, const file_path_t &file_relpath)	throw()
{
	// log to debug	
	KLOG_DBG("enter for relpath=" << file_relpath);
	// sanity check - the icon_relpath MUST be relpath_is_cfgfile()
	DBG_ASSERT( relpath_is_thumb(file_relpath) );
	// build the file_fullpath
	file_path_t	file_fullpath	= "/";
	for(size_t i = 2; i < file_relpath.size(); i++ )
		file_fullpath /= file_relpath.get_name_level(i);
	// log to debug
	KLOG_DBG("file_fullpath=" << file_fullpath);
// TODO here there is a security bug!!! 
// - a manually crafted url, car read anytfile the httpd can read
	// launch the download
	return download_file_fullpath( request, file_fullpath );
}

NEOIP_NAMESPACE_END





