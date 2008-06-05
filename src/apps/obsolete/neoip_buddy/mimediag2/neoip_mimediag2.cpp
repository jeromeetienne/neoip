/*! \file
    \brief Definition of the \ref mimediag_t class

\par Brief Description
This module has 2 functions which performs a mimediag encoding as in rfc3548
    
*/

/* system include */
/* local include */
#include "neoip_mimediag.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_log.hpp"

// it is set to 0 to see how my hardcoded one react
#define	USE_GNOME_VFS_MIME	0	//!< 1 if it is ok to use GNOME_VFS_MIME, 0 otherwise
					//!< - there is some issue with gnome-vfs being double init

/*************** Use gnome_vfs mime	***************************************/
#if USE_GNOME_VFS_MIME
#	include "libgnomevfs/gnome-vfs.h"
#endif	// USE_GNOME_VFS_MIME


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
mimediag_t::mimediag_t()	throw()
{
	// Hardcoded extension to mimetype association
	// - there have been extracted from a /etc/mime.types via a script
	add_type("ez", "application/andrew-inset");
	add_type("hqx", "application/mac-binhex40");
	add_type("cpt", "application/mac-compactpro");
	add_type("doc", "application/msword");
	add_type("bin", "application/octet-stream");
	add_type("dms", "application/octet-stream");
	add_type("lha", "application/octet-stream");
	add_type("lzh", "application/octet-stream");
	add_type("exe", "application/octet-stream");
	add_type("class", "application/octet-stream");
	add_type("so", "application/octet-stream");
	add_type("dll", "application/octet-stream");
	add_type("img", "application/octet-stream");
	add_type("iso", "application/octet-stream");
	add_type("ogg", "application/ogg");
	add_type("pdf", "application/pdf");
	add_type("ai", "application/postscript");
	add_type("eps", "application/postscript");
	add_type("ps", "application/postscript");
	add_type("rtf", "application/rtf");
	add_type("smi", "application/smil");
	add_type("smil", "application/smil");
	add_type("mif", "application/vnd.mif");
	add_type("xls", "application/vnd.ms-excel");
	add_type("ppt", "application/vnd.ms-powerpoint");
	add_type("sxw", "application/vnd.sun.xml.writer");
	add_type("stw", "application/vnd.sun.xml.writer.template");
	add_type("sxc", "application/vnd.sun.xml.calc");
	add_type("stc", "application/vnd.sun.xml.calc.template");
	add_type("sxd", "application/vnd.sun.xml.draw");
	add_type("std", "application/vnd.sun.xml.draw.template");
	add_type("sxi", "application/vnd.sun.xml.impress");
	add_type("sti", "application/vnd.sun.xml.impress.template");
	add_type("sxg", "application/vnd.sun.xml.writer.global");
	add_type("sxm", "application/vnd.sun.xml.math");
	add_type("wbxml", "application/vnd.wap.wbxml");
	add_type("wmlc", "application/vnd.wap.wmlc");
	add_type("wmlsc", "application/vnd.wap.wmlscriptc");
	add_type("bcpio", "application/x-bcpio");
	add_type("torrent", "application/x-bittorrent");
	add_type("bz2", "application/x-bzip2");
	add_type("vcd", "application/x-cdlink");
	add_type("pgn", "application/x-chess-pgn");
	add_type("cpio", "application/x-cpio");
	add_type("csh", "application/x-csh");
	add_type("dcr", "application/x-director");
	add_type("dir", "application/x-director");
	add_type("dxr", "application/x-director");
	add_type("dvi", "application/x-dvi");
	add_type("spl", "application/x-futuresplash");
	add_type("gtar", "application/x-gtar");
	add_type("gz", "application/x-gzip");
	add_type("tgz", "application/x-gzip");
	add_type("hdf", "application/x-hdf");
	add_type("js", "application/x-javascript");
	add_type("kwd", "application/x-kword");
	add_type("kwt", "application/x-kword");
	add_type("ksp", "application/x-kspread");
	add_type("kpr", "application/x-kpresenter");
	add_type("kpt", "application/x-kpresenter");
	add_type("chrt", "application/x-kchart");
	add_type("kil", "application/x-killustrator");
	add_type("skp", "application/x-koan");
	add_type("skd", "application/x-koan");
	add_type("skt", "application/x-koan");
	add_type("skm", "application/x-koan");
	add_type("latex", "application/x-latex");
	add_type("nc", "application/x-netcdf");
	add_type("cdf", "application/x-netcdf");
	add_type("rpm", "application/x-rpm");
	add_type("sh", "application/x-sh");
	add_type("shar", "application/x-shar");
	add_type("swf", "application/x-shockwave-flash");
	add_type("sit", "application/x-stuffit");
	add_type("sv4cpio", "application/x-sv4cpio");
	add_type("sv4crc", "application/x-sv4crc");
	add_type("tar", "application/x-tar");
	add_type("tcl", "application/x-tcl");
	add_type("tex", "application/x-tex");
	add_type("texinfo", "application/x-texinfo");
	add_type("texi", "application/x-texinfo");
	add_type("t", "application/x-troff");
	add_type("tr", "application/x-troff");
	add_type("roff", "application/x-troff");
	add_type("man", "application/x-troff-man");
	add_type("me", "application/x-troff-me");
	add_type("ms", "application/x-troff-ms");
	add_type("ustar", "application/x-ustar");
	add_type("src", "application/x-wais-source");
	add_type("xhtml", "application/xhtml+xml");
	add_type("xht", "application/xhtml+xml");
	add_type("zip", "application/zip");
	add_type("au", "audio/basic");
	add_type("snd", "audio/basic");
	add_type("mid", "audio/midi");
	add_type("midi", "audio/midi");
	add_type("kar", "audio/midi");
	add_type("mpga", "audio/mpeg");
	add_type("mp2", "audio/mpeg");
	add_type("mp3", "audio/mpeg");
	add_type("aif", "audio/x-aiff");
	add_type("aiff", "audio/x-aiff");
	add_type("aifc", "audio/x-aiff");
	add_type("m3u", "audio/x-mpegurl");
	add_type("ram", "audio/x-pn-realaudio");
	add_type("rm", "audio/x-pn-realaudio");
	add_type("ra", "audio/x-realaudio");
	add_type("wav", "audio/x-wav");
	add_type("pdb", "chemical/x-pdb");
	add_type("xyz", "chemical/x-xyz");
	add_type("bmp", "image/bmp");
	add_type("gif", "image/gif");
	add_type("ief", "image/ief");
	add_type("jpeg", "image/jpeg");
	add_type("jpg", "image/jpeg");
	add_type("jpe", "image/jpeg");
	add_type("png", "image/png");
	add_type("tiff", "image/tiff");
	add_type("tif", "image/tiff");
	add_type("djvu", "image/vnd.djvu");
	add_type("djv", "image/vnd.djvu");
	add_type("wbmp", "image/vnd.wap.wbmp");
	add_type("ras", "image/x-cmu-raster");
	add_type("pnm", "image/x-portable-anymap");
	add_type("pbm", "image/x-portable-bitmap");
	add_type("pgm", "image/x-portable-graymap");
	add_type("ppm", "image/x-portable-pixmap");
	add_type("rgb", "image/x-rgb");
	add_type("xbm", "image/x-xbitmap");
	add_type("xpm", "image/x-xpixmap");
	add_type("xwd", "image/x-xwindowdump");
	add_type("igs", "model/iges");
	add_type("iges", "model/iges");
	add_type("msh", "model/mesh");
	add_type("mesh", "model/mesh");
	add_type("silo", "model/mesh");
	add_type("wrl", "model/vrml");
	add_type("vrml", "model/vrml");
	add_type("css", "text/css");
	add_type("html", "text/html");
	add_type("htm", "text/html");
	add_type("asc", "text/plain");
	add_type("txt", "text/plain");
	add_type("rtx", "text/richtext");
// this one is a duplicate - removing this one at random	add_type("rtf", "text/rtf");
	add_type("sgml", "text/sgml");
	add_type("sgm", "text/sgml");
	add_type("tsv", "text/tab-separated-values");
	add_type("wml", "text/vnd.wap.wml");
	add_type("wmls", "text/vnd.wap.wmlscript");
	add_type("etx", "text/x-setext");
	add_type("xml", "text/xml");
	add_type("xsl", "text/xml");
	add_type("mpeg", "video/mpeg");
	add_type("mpg", "video/mpeg");
	add_type("mpe", "video/mpeg");
	add_type("qt", "video/quicktime");
	add_type("mov", "video/quicktime");
	add_type("mxu", "video/vnd.mpegurl");
	add_type("avi", "video/x-msvideo");
	add_type("movie", "video/x-sgi-movie");
	add_type("ice", "x-conference/x-cooltalk");
	
/*************** Use gnome_vfs mime	***************************************/
#if USE_GNOME_VFS_MIME
	// TODO some issue here as i guess the gnoms_vfs_init() cant be open/close multiple time
	gnome_vfs_init();
#endif	// USE_GNOME_VFS_MIME
}

/** \brief Destructor
 */
mimediag_t::~mimediag_t()	throw()
{
/*************** Use gnome_vfs mime	***************************************/
#if USE_GNOME_VFS_MIME
	gnome_vfs_shutdown();
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            database function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Add a mimetype into the database for this file extension
 */
void	mimediag_t::add_type(const std::string &extension, const std::string &mimetype)	throw()
{
	bool	succeed	= type_db.insert(std::make_pair(extension, mimetype)).second;
	DBG_ASSERT( succeed );
}

/** \brief Return the mimetype attached to this file extension, or an empty string is none is found
 */
std::string mimediag_t::get_type(const std::string &extension)			const throw()
{
	std::map<std::string, std::string>::const_iterator	iter;
	// try to find this extension in the database
	iter	= type_db.find(extension);
	// if the extension is not found, return ""
	if( iter == type_db.end() )	return "";
	// else return the associated mimetype
	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                 query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the diagnosed mime type of the local file pointed by file_path
 */
std::string mimediag_t::from_file(const file_path_t &file_path)			const throw()
{
/*************** Use gnome_vfs mime	***************************************/
#if USE_GNOME_VFS_MIME
	// get the mime type for this file from gnome-vfs
	char *	mimetype_c	= gnome_vfs_get_mime_type(file_path.to_string().c_str());
	// if a mimetype is found, return it
	if( mimetype_c	)	return std::string(mimetype_c);
#endif
	
	// stat the file_path
	file_stat_t	file_stat	= file_stat_t(file_path.to_string());
	// if this file_path is a directory, return the appropriate mimetype
	if( !file_stat.is_null() && file_stat.is_dir() )
		return "x-directory/normal";
	
	// get the extension of the file_path 
	std::string	extension	= file_path.get_basename_ext();
	// lower the extension case - it improves the mime matching
	extension	= string_t::to_lower(extension);
	// log to debug
	KLOG_DBG("the extension is [" << extension << "] for the file_path " << file_path);
	// if no extension is found, return the default mimetype
	if( extension.empty() )		return "application/octet-stream";

	// try to get the mimetype associated with this extention
	std::string	mimetype	= get_type(extension);
	// if no mimetype is found, return the default mimetype
	if( mimetype.empty() )		return "application/octet-stream";

	// return the mimetype of the file
	return mimetype;
}
NEOIP_NAMESPACE_END


