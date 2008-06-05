/*! \file
    \brief Definition of the \ref mimediag_t class
    
*/

/* system include */
/* local include */
#include "neoip_mimediag.hpp"
#include "neoip_file_path.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of the static database indexing the mimetype by their file extension
mimediag_t::mimetype_db_t *	mimediag_t::mimetype_db	= NULL;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Build the mimetype_db
 */
void	mimediag_t::ctor_mimetype_db()	throw()
{
	// sanity check - mimetype_db MUST NOT be initialized
	DBG_ASSERT( !mimetype_db );
	
	// allocate the mimetype_db
	// - NOTE: it use new directly (instead of nipmem_new to avoid the memory tracker)
	mimetype_db	= new mimetype_db_t();

#define POPULATE_MIMETYPE_DB(file_ext, mimetype)						\
	{ bool succeed	= mimetype_db->insert(std::make_pair(file_ext, mimetype)).second;	\
	  DBG_ASSERT( succeed );								\
	}
	// populate the mimetype_db
	POPULATE_MIMETYPE_DB("ez", "application/andrew-inset");
	POPULATE_MIMETYPE_DB("cu", "application/cu-seeme");
	POPULATE_MIMETYPE_DB("tsp", "application/dsptype");
	POPULATE_MIMETYPE_DB("spl", "application/futuresplash");
	POPULATE_MIMETYPE_DB("hta", "application/hta");
	POPULATE_MIMETYPE_DB("jar", "application/java-archive");
	POPULATE_MIMETYPE_DB("ser", "application/java-serialized-object");
	POPULATE_MIMETYPE_DB("class", "application/java-vm");
	POPULATE_MIMETYPE_DB("hqx", "application/mac-binhex40");
	POPULATE_MIMETYPE_DB("cpt", "application/mac-compactpro");
	POPULATE_MIMETYPE_DB("nb", "application/mathematica");
	POPULATE_MIMETYPE_DB("mdb", "application/msaccess");
	POPULATE_MIMETYPE_DB("doc", "application/msword");
	POPULATE_MIMETYPE_DB("dot", "application/msword");
	POPULATE_MIMETYPE_DB("bin", "application/octet-stream");
	POPULATE_MIMETYPE_DB("oda", "application/oda");
	POPULATE_MIMETYPE_DB("ogg", "application/ogg");
	POPULATE_MIMETYPE_DB("pdf", "application/pdf");
	POPULATE_MIMETYPE_DB("key", "application/pgp-keys");
	POPULATE_MIMETYPE_DB("pgp", "application/pgp-signature");
	POPULATE_MIMETYPE_DB("prf", "application/pics-rules");
	POPULATE_MIMETYPE_DB("ps", "application/postscript");
	POPULATE_MIMETYPE_DB("ai", "application/postscript");
	POPULATE_MIMETYPE_DB("eps", "application/postscript");
	POPULATE_MIMETYPE_DB("rar", "application/rar");
	POPULATE_MIMETYPE_DB("rdf", "application/rdf+xml");
	POPULATE_MIMETYPE_DB("rss", "application/rss+xml");
	POPULATE_MIMETYPE_DB("smi", "application/smil");
	POPULATE_MIMETYPE_DB("smil", "application/smil");
	POPULATE_MIMETYPE_DB("wpd", "application/wordperfect");
	POPULATE_MIMETYPE_DB("wp5", "application/wordperfect5.1");
	POPULATE_MIMETYPE_DB("xhtml", "application/xhtml+xml");
	POPULATE_MIMETYPE_DB("xht", "application/xhtml+xml");
	POPULATE_MIMETYPE_DB("xml", "application/xml");
	POPULATE_MIMETYPE_DB("xsl", "application/xml");
	POPULATE_MIMETYPE_DB("zip", "application/zip");
	POPULATE_MIMETYPE_DB("cdy", "application/vnd.cinderella");
	POPULATE_MIMETYPE_DB("xul", "application/vnd.mozilla.xul+xml");
	POPULATE_MIMETYPE_DB("xls", "application/vnd.ms-excel");
	POPULATE_MIMETYPE_DB("xlb", "application/vnd.ms-excel");
	POPULATE_MIMETYPE_DB("xlt", "application/vnd.ms-excel");
	POPULATE_MIMETYPE_DB("cat", "application/vnd.ms-pki.seccat");
	POPULATE_MIMETYPE_DB("stl", "application/vnd.ms-pki.stl");
	POPULATE_MIMETYPE_DB("ppt", "application/vnd.ms-powerpoint");
	POPULATE_MIMETYPE_DB("pps", "application/vnd.ms-powerpoint");
	POPULATE_MIMETYPE_DB("odc", "application/vnd.oasis.opendocument.chart");
	POPULATE_MIMETYPE_DB("odb", "application/vnd.oasis.opendocument.database");
	POPULATE_MIMETYPE_DB("odf", "application/vnd.oasis.opendocument.formula");
	POPULATE_MIMETYPE_DB("odg", "application/vnd.oasis.opendocument.graphics");
	POPULATE_MIMETYPE_DB("otg", "application/vnd.oasis.opendocument.graphics-template");
	POPULATE_MIMETYPE_DB("odi", "application/vnd.oasis.opendocument.image");
	POPULATE_MIMETYPE_DB("odp", "application/vnd.oasis.opendocument.presentation");
	POPULATE_MIMETYPE_DB("otp", "application/vnd.oasis.opendocument.presentation-template");
	POPULATE_MIMETYPE_DB("ods", "application/vnd.oasis.opendocument.spreadsheet");
	POPULATE_MIMETYPE_DB("ots", "application/vnd.oasis.opendocument.spreadsheet-template");
	POPULATE_MIMETYPE_DB("odt", "application/vnd.oasis.opendocument.text");
	POPULATE_MIMETYPE_DB("odm", "application/vnd.oasis.opendocument.text-master");
	POPULATE_MIMETYPE_DB("ott", "application/vnd.oasis.opendocument.text-template");
	POPULATE_MIMETYPE_DB("oth", "application/vnd.oasis.opendocument.text-web");
	POPULATE_MIMETYPE_DB("cod", "application/vnd.rim.cod");
	POPULATE_MIMETYPE_DB("mmf", "application/vnd.smaf");
	POPULATE_MIMETYPE_DB("sdc", "application/vnd.stardivision.calc");
	POPULATE_MIMETYPE_DB("sda", "application/vnd.stardivision.draw");
	POPULATE_MIMETYPE_DB("sdd", "application/vnd.stardivision.impress");
	POPULATE_MIMETYPE_DB("sdp", "application/vnd.stardivision.impress");
	POPULATE_MIMETYPE_DB("smf", "application/vnd.stardivision.math");
	POPULATE_MIMETYPE_DB("sdw", "application/vnd.stardivision.writer");
	POPULATE_MIMETYPE_DB("vor", "application/vnd.stardivision.writer");
	POPULATE_MIMETYPE_DB("sgl", "application/vnd.stardivision.writer-global");
	POPULATE_MIMETYPE_DB("sxc", "application/vnd.sun.xml.calc");
	POPULATE_MIMETYPE_DB("stc", "application/vnd.sun.xml.calc.template");
	POPULATE_MIMETYPE_DB("sxd", "application/vnd.sun.xml.draw");
	POPULATE_MIMETYPE_DB("std", "application/vnd.sun.xml.draw.template");
	POPULATE_MIMETYPE_DB("sxi", "application/vnd.sun.xml.impress");
	POPULATE_MIMETYPE_DB("sti", "application/vnd.sun.xml.impress.template");
	POPULATE_MIMETYPE_DB("sxm", "application/vnd.sun.xml.math");
	POPULATE_MIMETYPE_DB("sxw", "application/vnd.sun.xml.writer");
	POPULATE_MIMETYPE_DB("sxg", "application/vnd.sun.xml.writer.global");
	POPULATE_MIMETYPE_DB("stw", "application/vnd.sun.xml.writer.template");
	POPULATE_MIMETYPE_DB("sis", "application/vnd.symbian.install");
	POPULATE_MIMETYPE_DB("vsd", "application/vnd.visio");
	POPULATE_MIMETYPE_DB("wbxml", "application/vnd.wap.wbxml");
	POPULATE_MIMETYPE_DB("wmlc", "application/vnd.wap.wmlc");
	POPULATE_MIMETYPE_DB("wmlsc", "application/vnd.wap.wmlscriptc");
	POPULATE_MIMETYPE_DB("wk", "application/x-123");
	POPULATE_MIMETYPE_DB("abw", "application/x-abiword");
	POPULATE_MIMETYPE_DB("dmg", "application/x-apple-diskimage");
	POPULATE_MIMETYPE_DB("bcpio", "application/x-bcpio");
	POPULATE_MIMETYPE_DB("torrent", "application/x-bittorrent");
	POPULATE_MIMETYPE_DB("cdf", "application/x-cdf");
	POPULATE_MIMETYPE_DB("vcd", "application/x-cdlink");
	POPULATE_MIMETYPE_DB("pgn", "application/x-chess-pgn");
	POPULATE_MIMETYPE_DB("cpio", "application/x-cpio");
	POPULATE_MIMETYPE_DB("csh", "application/x-csh");
	POPULATE_MIMETYPE_DB("deb", "application/x-debian-package");
	POPULATE_MIMETYPE_DB("udeb", "application/x-debian-package");
	POPULATE_MIMETYPE_DB("dcr", "application/x-director");
	POPULATE_MIMETYPE_DB("dir", "application/x-director");
	POPULATE_MIMETYPE_DB("dxr", "application/x-director");
	POPULATE_MIMETYPE_DB("dms", "application/x-dms");
	POPULATE_MIMETYPE_DB("wad", "application/x-doom");
	POPULATE_MIMETYPE_DB("dvi", "application/x-dvi");
	POPULATE_MIMETYPE_DB("flac", "application/x-flac");
	POPULATE_MIMETYPE_DB("pfa", "application/x-font");
	POPULATE_MIMETYPE_DB("pfb", "application/x-font");
	POPULATE_MIMETYPE_DB("gsf", "application/x-font");
	POPULATE_MIMETYPE_DB("pcf", "application/x-font");
	POPULATE_MIMETYPE_DB("pcf.Z", "application/x-font");
	POPULATE_MIMETYPE_DB("mm", "application/x-freemind");
// duplicate	POPULATE_MIMETYPE_DB("spl", "application/x-futuresplash");
	POPULATE_MIMETYPE_DB("gnumeric", "application/x-gnumeric");
	POPULATE_MIMETYPE_DB("sgf", "application/x-go-sgf");
	POPULATE_MIMETYPE_DB("gcf", "application/x-graphing-calculator");
	POPULATE_MIMETYPE_DB("gtar", "application/x-gtar");
	POPULATE_MIMETYPE_DB("tgz", "application/x-gtar");
	POPULATE_MIMETYPE_DB("taz", "application/x-gtar");
	POPULATE_MIMETYPE_DB("hdf", "application/x-hdf");
	POPULATE_MIMETYPE_DB("phtml", "application/x-httpd-php");
	POPULATE_MIMETYPE_DB("pht", "application/x-httpd-php");
	POPULATE_MIMETYPE_DB("php", "application/x-httpd-php");
	POPULATE_MIMETYPE_DB("phps", "application/x-httpd-php-source");
	POPULATE_MIMETYPE_DB("php3", "application/x-httpd-php3");
	POPULATE_MIMETYPE_DB("php3p", "application/x-httpd-php3-preprocessed");
	POPULATE_MIMETYPE_DB("php4", "application/x-httpd-php4");
	POPULATE_MIMETYPE_DB("rhtml", "application/x-httpd-eruby");
	POPULATE_MIMETYPE_DB("ica", "application/x-ica");
	POPULATE_MIMETYPE_DB("ins", "application/x-internet-signup");
	POPULATE_MIMETYPE_DB("isp", "application/x-internet-signup");
	POPULATE_MIMETYPE_DB("iii", "application/x-iphone");
	POPULATE_MIMETYPE_DB("iso", "application/x-iso9660-image");
	POPULATE_MIMETYPE_DB("jnlp", "application/x-java-jnlp-file");
	POPULATE_MIMETYPE_DB("js", "application/x-javascript");
	POPULATE_MIMETYPE_DB("jmz", "application/x-jmol");
	POPULATE_MIMETYPE_DB("chrt", "application/x-kchart");
	POPULATE_MIMETYPE_DB("kil", "application/x-killustrator");
	POPULATE_MIMETYPE_DB("skp", "application/x-koan");
	POPULATE_MIMETYPE_DB("skd", "application/x-koan");
	POPULATE_MIMETYPE_DB("skt", "application/x-koan");
	POPULATE_MIMETYPE_DB("skm", "application/x-koan");
	POPULATE_MIMETYPE_DB("kpr", "application/x-kpresenter");
	POPULATE_MIMETYPE_DB("kpt", "application/x-kpresenter");
	POPULATE_MIMETYPE_DB("ksp", "application/x-kspread");
	POPULATE_MIMETYPE_DB("kwd", "application/x-kword");
	POPULATE_MIMETYPE_DB("kwt", "application/x-kword");
	POPULATE_MIMETYPE_DB("latex", "application/x-latex");
	POPULATE_MIMETYPE_DB("lha", "application/x-lha");
	POPULATE_MIMETYPE_DB("lzh", "application/x-lzh");
	POPULATE_MIMETYPE_DB("lzx", "application/x-lzx");
	POPULATE_MIMETYPE_DB("frm", "application/x-maker");
	POPULATE_MIMETYPE_DB("maker", "application/x-maker");
	POPULATE_MIMETYPE_DB("frame", "application/x-maker");
	POPULATE_MIMETYPE_DB("fm", "application/x-maker");
	POPULATE_MIMETYPE_DB("fb", "application/x-maker");
	POPULATE_MIMETYPE_DB("book", "application/x-maker");
	POPULATE_MIMETYPE_DB("fbdoc", "application/x-maker");
	POPULATE_MIMETYPE_DB("mif", "application/x-mif");
	POPULATE_MIMETYPE_DB("wmd", "application/x-ms-wmd");
	POPULATE_MIMETYPE_DB("wmz", "application/x-ms-wmz");
	POPULATE_MIMETYPE_DB("com", "application/x-msdos-program");
	POPULATE_MIMETYPE_DB("exe", "application/x-msdos-program");
	POPULATE_MIMETYPE_DB("bat", "application/x-msdos-program");
	POPULATE_MIMETYPE_DB("dll", "application/x-msdos-program");
	POPULATE_MIMETYPE_DB("msi", "application/x-msi");
	POPULATE_MIMETYPE_DB("nc", "application/x-netcdf");
	POPULATE_MIMETYPE_DB("pac", "application/x-ns-proxy-autoconfig");
	POPULATE_MIMETYPE_DB("nwc", "application/x-nwc");
	POPULATE_MIMETYPE_DB("o", "application/x-object");
	POPULATE_MIMETYPE_DB("oza", "application/x-oz-application");
	POPULATE_MIMETYPE_DB("p7r", "application/x-pkcs7-certreqresp");
	POPULATE_MIMETYPE_DB("crl", "application/x-pkcs7-crl");
	POPULATE_MIMETYPE_DB("pyc", "application/x-python-code");
	POPULATE_MIMETYPE_DB("pyo", "application/x-python-code");
	POPULATE_MIMETYPE_DB("qtl", "application/x-quicktimeplayer");
	POPULATE_MIMETYPE_DB("rpm", "application/x-redhat-package-manager");
	POPULATE_MIMETYPE_DB("sh", "application/x-sh");
	POPULATE_MIMETYPE_DB("shar", "application/x-shar");
	POPULATE_MIMETYPE_DB("swf", "application/x-shockwave-flash");
	POPULATE_MIMETYPE_DB("swfl", "application/x-shockwave-flash");
	POPULATE_MIMETYPE_DB("sit", "application/x-stuffit");
	POPULATE_MIMETYPE_DB("sv4cpio", "application/x-sv4cpio");
	POPULATE_MIMETYPE_DB("sv4crc", "application/x-sv4crc");
	POPULATE_MIMETYPE_DB("tar", "application/x-tar");
	POPULATE_MIMETYPE_DB("tcl", "application/x-tcl");
	POPULATE_MIMETYPE_DB("gf", "application/x-tex-gf");
	POPULATE_MIMETYPE_DB("pk", "application/x-tex-pk");
	POPULATE_MIMETYPE_DB("texinfo", "application/x-texinfo");
	POPULATE_MIMETYPE_DB("texi", "application/x-texinfo");
	POPULATE_MIMETYPE_DB("~", "application/x-trash");
	POPULATE_MIMETYPE_DB("%", "application/x-trash");
	POPULATE_MIMETYPE_DB("bak", "application/x-trash");
	POPULATE_MIMETYPE_DB("old", "application/x-trash");
	POPULATE_MIMETYPE_DB("sik", "application/x-trash");
	POPULATE_MIMETYPE_DB("t", "application/x-troff");
	POPULATE_MIMETYPE_DB("tr", "application/x-troff");
	POPULATE_MIMETYPE_DB("roff", "application/x-troff");
	POPULATE_MIMETYPE_DB("man", "application/x-troff-man");
	POPULATE_MIMETYPE_DB("me", "application/x-troff-me");
	POPULATE_MIMETYPE_DB("ms", "application/x-troff-ms");
	POPULATE_MIMETYPE_DB("ustar", "application/x-ustar");
	POPULATE_MIMETYPE_DB("src", "application/x-wais-source");
	POPULATE_MIMETYPE_DB("wz", "application/x-wingz");
	POPULATE_MIMETYPE_DB("crt", "application/x-x509-ca-cert");
	POPULATE_MIMETYPE_DB("xcf", "application/x-xcf");
	POPULATE_MIMETYPE_DB("fig", "application/x-xfig");
	POPULATE_MIMETYPE_DB("xpi", "application/x-xpinstall");
	POPULATE_MIMETYPE_DB("au", "audio/basic");
	POPULATE_MIMETYPE_DB("snd", "audio/basic");
	POPULATE_MIMETYPE_DB("mid", "audio/midi");
	POPULATE_MIMETYPE_DB("midi", "audio/midi");
	POPULATE_MIMETYPE_DB("kar", "audio/midi");
	POPULATE_MIMETYPE_DB("mpga", "audio/mpeg");
	POPULATE_MIMETYPE_DB("mpega", "audio/mpeg");
	POPULATE_MIMETYPE_DB("mp2", "audio/mpeg");
	POPULATE_MIMETYPE_DB("mp3", "audio/mpeg");
	POPULATE_MIMETYPE_DB("m4a", "audio/mpeg");
	POPULATE_MIMETYPE_DB("m3u", "audio/mpegurl");
	POPULATE_MIMETYPE_DB("sid", "audio/prs.sid");
	POPULATE_MIMETYPE_DB("aif", "audio/x-aiff");
	POPULATE_MIMETYPE_DB("aiff", "audio/x-aiff");
	POPULATE_MIMETYPE_DB("aifc", "audio/x-aiff");
	POPULATE_MIMETYPE_DB("gsm", "audio/x-gsm");
// duplicate	POPULATE_MIMETYPE_DB("m3u", "audio/x-mpegurl");
	POPULATE_MIMETYPE_DB("wma", "audio/x-ms-wma");
	POPULATE_MIMETYPE_DB("wax", "audio/x-ms-wax");
	POPULATE_MIMETYPE_DB("ra", "audio/x-pn-realaudio");
	POPULATE_MIMETYPE_DB("rm", "audio/x-pn-realaudio");
	POPULATE_MIMETYPE_DB("ram", "audio/x-pn-realaudio");
// duplicate 	POPULATE_MIMETYPE_DB("ra", "audio/x-realaudio");
	POPULATE_MIMETYPE_DB("pls", "audio/x-scpls");
	POPULATE_MIMETYPE_DB("sd2", "audio/x-sd2");
	POPULATE_MIMETYPE_DB("wav", "audio/x-wav");
	POPULATE_MIMETYPE_DB("alc", "chemical/x-alchemy");
	POPULATE_MIMETYPE_DB("cac", "chemical/x-cache");
	POPULATE_MIMETYPE_DB("cache", "chemical/x-cache");
	POPULATE_MIMETYPE_DB("csf", "chemical/x-cache-csf");
	POPULATE_MIMETYPE_DB("cbin", "chemical/x-cactvs-binary");
	POPULATE_MIMETYPE_DB("cascii", "chemical/x-cactvs-binary");
	POPULATE_MIMETYPE_DB("ctab", "chemical/x-cactvs-binary");
	POPULATE_MIMETYPE_DB("cdx", "chemical/x-cdx");
	POPULATE_MIMETYPE_DB("cer", "chemical/x-cerius");
	POPULATE_MIMETYPE_DB("c3d", "chemical/x-chem3d");
	POPULATE_MIMETYPE_DB("chm", "chemical/x-chemdraw");
	POPULATE_MIMETYPE_DB("cif", "chemical/x-cif");
	POPULATE_MIMETYPE_DB("cmdf", "chemical/x-cmdf");
	POPULATE_MIMETYPE_DB("cml", "chemical/x-cml");
	POPULATE_MIMETYPE_DB("cpa", "chemical/x-compass");
	POPULATE_MIMETYPE_DB("bsd", "chemical/x-crossfire");
	POPULATE_MIMETYPE_DB("csml", "chemical/x-csml");
	POPULATE_MIMETYPE_DB("csm", "chemical/x-csml");
	POPULATE_MIMETYPE_DB("ctx", "chemical/x-ctx");
	POPULATE_MIMETYPE_DB("cxf", "chemical/x-cxf");
	POPULATE_MIMETYPE_DB("cef", "chemical/x-cxf");
	POPULATE_MIMETYPE_DB("emb", "chemical/x-embl-dl-nucleotide");
	POPULATE_MIMETYPE_DB("embl", "chemical/x-embl-dl-nucleotide");
	POPULATE_MIMETYPE_DB("spc", "chemical/x-galactic-spc");
	POPULATE_MIMETYPE_DB("inp", "chemical/x-gamess-input");
	POPULATE_MIMETYPE_DB("gam", "chemical/x-gamess-input");
	POPULATE_MIMETYPE_DB("gamin", "chemical/x-gamess-input");
	POPULATE_MIMETYPE_DB("fch", "chemical/x-gaussian-checkpoint");
	POPULATE_MIMETYPE_DB("fchk", "chemical/x-gaussian-checkpoint");
	POPULATE_MIMETYPE_DB("cub", "chemical/x-gaussian-cube");
	POPULATE_MIMETYPE_DB("gau", "chemical/x-gaussian-input");
	POPULATE_MIMETYPE_DB("gjc", "chemical/x-gaussian-input");
	POPULATE_MIMETYPE_DB("gjf", "chemical/x-gaussian-input");
	POPULATE_MIMETYPE_DB("gal", "chemical/x-gaussian-log");
	POPULATE_MIMETYPE_DB("gcg", "chemical/x-gcg8-sequence");
	POPULATE_MIMETYPE_DB("gen", "chemical/x-genbank");
	POPULATE_MIMETYPE_DB("hin", "chemical/x-hin");
	POPULATE_MIMETYPE_DB("istr", "chemical/x-isostar");
	POPULATE_MIMETYPE_DB("ist", "chemical/x-isostar");
	POPULATE_MIMETYPE_DB("jdx", "chemical/x-jcamp-dx");
	POPULATE_MIMETYPE_DB("dx", "chemical/x-jcamp-dx");
	POPULATE_MIMETYPE_DB("kin", "chemical/x-kinemage");
	POPULATE_MIMETYPE_DB("mcm", "chemical/x-macmolecule");
	POPULATE_MIMETYPE_DB("mmd", "chemical/x-macromodel-input");
	POPULATE_MIMETYPE_DB("mmod", "chemical/x-macromodel-input");
	POPULATE_MIMETYPE_DB("mol", "chemical/x-mdl-molfile");
	POPULATE_MIMETYPE_DB("rd", "chemical/x-mdl-rdfile");
	POPULATE_MIMETYPE_DB("rxn", "chemical/x-mdl-rxnfile");
	POPULATE_MIMETYPE_DB("sd", "chemical/x-mdl-sdfile");
	POPULATE_MIMETYPE_DB("sdf", "chemical/x-mdl-sdfile");
	POPULATE_MIMETYPE_DB("tgf", "chemical/x-mdl-tgf");
	POPULATE_MIMETYPE_DB("mcif", "chemical/x-mmcif");
	POPULATE_MIMETYPE_DB("mol2", "chemical/x-mol2");
	POPULATE_MIMETYPE_DB("b", "chemical/x-molconn-Z");
	POPULATE_MIMETYPE_DB("gpt", "chemical/x-mopac-graph");
	POPULATE_MIMETYPE_DB("mop", "chemical/x-mopac-input");
	POPULATE_MIMETYPE_DB("mopcrt", "chemical/x-mopac-input");
	POPULATE_MIMETYPE_DB("mpc", "chemical/x-mopac-input");
	POPULATE_MIMETYPE_DB("dat", "chemical/x-mopac-input");
	POPULATE_MIMETYPE_DB("zmt", "chemical/x-mopac-input");
	POPULATE_MIMETYPE_DB("moo", "chemical/x-mopac-out");
	POPULATE_MIMETYPE_DB("mvb", "chemical/x-mopac-vib");
	POPULATE_MIMETYPE_DB("asn", "chemical/x-ncbi-asn1");
	POPULATE_MIMETYPE_DB("prt", "chemical/x-ncbi-asn1-ascii");
	POPULATE_MIMETYPE_DB("ent", "chemical/x-ncbi-asn1-ascii");
	POPULATE_MIMETYPE_DB("val", "chemical/x-ncbi-asn1-binary");
	POPULATE_MIMETYPE_DB("aso", "chemical/x-ncbi-asn1-binary");
// duplicate 	POPULATE_MIMETYPE_DB("asn", "chemical/x-ncbi-asn1-spec");
	POPULATE_MIMETYPE_DB("pdb", "chemical/x-pdb");
// duplicate	POPULATE_MIMETYPE_DB("ent", "chemical/x-pdb");
	POPULATE_MIMETYPE_DB("ros", "chemical/x-rosdal");
	POPULATE_MIMETYPE_DB("sw", "chemical/x-swissprot");
	POPULATE_MIMETYPE_DB("vms", "chemical/x-vamas-iso14976");
	POPULATE_MIMETYPE_DB("vmd", "chemical/x-vmd");
	POPULATE_MIMETYPE_DB("xtel", "chemical/x-xtel");
	POPULATE_MIMETYPE_DB("xyz", "chemical/x-xyz");
	POPULATE_MIMETYPE_DB("gif", "image/gif");
	POPULATE_MIMETYPE_DB("ief", "image/ief");
	POPULATE_MIMETYPE_DB("jpeg", "image/jpeg");
	POPULATE_MIMETYPE_DB("jpg", "image/jpeg");
	POPULATE_MIMETYPE_DB("jpe", "image/jpeg");
	POPULATE_MIMETYPE_DB("pcx", "image/pcx");
	POPULATE_MIMETYPE_DB("png", "image/png");
	POPULATE_MIMETYPE_DB("svg", "image/svg+xml");
	POPULATE_MIMETYPE_DB("svgz", "image/svg+xml");
	POPULATE_MIMETYPE_DB("tiff", "image/tiff");
	POPULATE_MIMETYPE_DB("tif", "image/tiff");
	POPULATE_MIMETYPE_DB("djvu", "image/vnd.djvu");
	POPULATE_MIMETYPE_DB("djv", "image/vnd.djvu");
	POPULATE_MIMETYPE_DB("wbmp", "image/vnd.wap.wbmp");
	POPULATE_MIMETYPE_DB("ras", "image/x-cmu-raster");
	POPULATE_MIMETYPE_DB("cdr", "image/x-coreldraw");
	POPULATE_MIMETYPE_DB("pat", "image/x-coreldrawpattern");
	POPULATE_MIMETYPE_DB("cdt", "image/x-coreldrawtemplate");
// duplicate	POPULATE_MIMETYPE_DB("cpt", "image/x-corelphotopaint");
	POPULATE_MIMETYPE_DB("ico", "image/x-icon");
	POPULATE_MIMETYPE_DB("art", "image/x-jg");
	POPULATE_MIMETYPE_DB("jng", "image/x-jng");
	POPULATE_MIMETYPE_DB("bmp", "image/x-ms-bmp");
	POPULATE_MIMETYPE_DB("psd", "image/x-photoshop");
	POPULATE_MIMETYPE_DB("pnm", "image/x-portable-anymap");
	POPULATE_MIMETYPE_DB("pbm", "image/x-portable-bitmap");
	POPULATE_MIMETYPE_DB("pgm", "image/x-portable-graymap");
	POPULATE_MIMETYPE_DB("ppm", "image/x-portable-pixmap");
	POPULATE_MIMETYPE_DB("rgb", "image/x-rgb");
	POPULATE_MIMETYPE_DB("xbm", "image/x-xbitmap");
	POPULATE_MIMETYPE_DB("xpm", "image/x-xpixmap");
	POPULATE_MIMETYPE_DB("xwd", "image/x-xwindowdump");
	POPULATE_MIMETYPE_DB("igs", "model/iges");
	POPULATE_MIMETYPE_DB("iges", "model/iges");
	POPULATE_MIMETYPE_DB("msh", "model/mesh");
	POPULATE_MIMETYPE_DB("mesh", "model/mesh");
	POPULATE_MIMETYPE_DB("silo", "model/mesh");
	POPULATE_MIMETYPE_DB("wrl", "model/vrml");
	POPULATE_MIMETYPE_DB("vrml", "model/vrml");
	POPULATE_MIMETYPE_DB("ics", "text/calendar");
	POPULATE_MIMETYPE_DB("icz", "text/calendar");
	POPULATE_MIMETYPE_DB("csv", "text/comma-separated-values");
	POPULATE_MIMETYPE_DB("css", "text/css");
	POPULATE_MIMETYPE_DB("323", "text/h323");
	POPULATE_MIMETYPE_DB("html", "text/html");
	POPULATE_MIMETYPE_DB("htm", "text/html");
	POPULATE_MIMETYPE_DB("shtml", "text/html");
	POPULATE_MIMETYPE_DB("uls", "text/iuls");
	POPULATE_MIMETYPE_DB("mml", "text/mathml");
	POPULATE_MIMETYPE_DB("asc", "text/plain");
	POPULATE_MIMETYPE_DB("txt", "text/plain");
	POPULATE_MIMETYPE_DB("text", "text/plain");
	POPULATE_MIMETYPE_DB("diff", "text/plain");
	POPULATE_MIMETYPE_DB("pot", "text/plain");
	POPULATE_MIMETYPE_DB("rtx", "text/richtext");
	POPULATE_MIMETYPE_DB("rtf", "text/rtf");
	POPULATE_MIMETYPE_DB("sct", "text/scriptlet");
	POPULATE_MIMETYPE_DB("wsc", "text/scriptlet");
	POPULATE_MIMETYPE_DB("tm", "text/texmacs");
	POPULATE_MIMETYPE_DB("ts", "text/texmacs");
	POPULATE_MIMETYPE_DB("tsv", "text/tab-separated-values");
	POPULATE_MIMETYPE_DB("jad", "text/vnd.sun.j2me.app-descriptor");
	POPULATE_MIMETYPE_DB("wml", "text/vnd.wap.wml");
	POPULATE_MIMETYPE_DB("wmls", "text/vnd.wap.wmlscript");
	POPULATE_MIMETYPE_DB("bib", "text/x-bibtex");
	POPULATE_MIMETYPE_DB("boo", "text/x-boo");
	POPULATE_MIMETYPE_DB("h++", "text/x-c++hdr");
	POPULATE_MIMETYPE_DB("hpp", "text/x-c++hdr");
	POPULATE_MIMETYPE_DB("hxx", "text/x-c++hdr");
	POPULATE_MIMETYPE_DB("hh", "text/x-c++hdr");
	POPULATE_MIMETYPE_DB("c++", "text/x-c++src");
	POPULATE_MIMETYPE_DB("cpp", "text/x-c++src");
	POPULATE_MIMETYPE_DB("cxx", "text/x-c++src");
	POPULATE_MIMETYPE_DB("cc", "text/x-c++src");
	POPULATE_MIMETYPE_DB("h", "text/x-chdr");
	POPULATE_MIMETYPE_DB("htc", "text/x-component");
// duplicate	POPULATE_MIMETYPE_DB("csh", "text/x-csh");
	POPULATE_MIMETYPE_DB("c", "text/x-csrc");
	POPULATE_MIMETYPE_DB("d", "text/x-dsrc");
	POPULATE_MIMETYPE_DB("hs", "text/x-haskell");
	POPULATE_MIMETYPE_DB("java", "text/x-java");
	POPULATE_MIMETYPE_DB("lhs", "text/x-literate-haskell");
	POPULATE_MIMETYPE_DB("moc", "text/x-moc");
	POPULATE_MIMETYPE_DB("p", "text/x-pascal");
	POPULATE_MIMETYPE_DB("pas", "text/x-pascal");
	POPULATE_MIMETYPE_DB("gcd", "text/x-pcs-gcd");
	POPULATE_MIMETYPE_DB("pl", "text/x-perl");
	POPULATE_MIMETYPE_DB("pm", "text/x-perl");
	POPULATE_MIMETYPE_DB("py", "text/x-python");
	POPULATE_MIMETYPE_DB("etx", "text/x-setext");
// duplicate	POPULATE_MIMETYPE_DB("sh", "text/x-sh");
// duplicate	POPULATE_MIMETYPE_DB("tcl", "text/x-tcl");
	POPULATE_MIMETYPE_DB("tk", "text/x-tcl");
	POPULATE_MIMETYPE_DB("tex", "text/x-tex");
	POPULATE_MIMETYPE_DB("ltx", "text/x-tex");
	POPULATE_MIMETYPE_DB("sty", "text/x-tex");
	POPULATE_MIMETYPE_DB("cls", "text/x-tex");
	POPULATE_MIMETYPE_DB("vcs", "text/x-vcalendar");
	POPULATE_MIMETYPE_DB("vcf", "text/x-vcard");
	POPULATE_MIMETYPE_DB("dl", "video/dl");
	POPULATE_MIMETYPE_DB("dif", "video/dv");
	POPULATE_MIMETYPE_DB("dv", "video/dv");
	POPULATE_MIMETYPE_DB("fli", "video/fli");
	POPULATE_MIMETYPE_DB("gl", "video/gl");
	POPULATE_MIMETYPE_DB("mpeg", "video/mpeg");
	POPULATE_MIMETYPE_DB("mpg", "video/mpeg");
	POPULATE_MIMETYPE_DB("mpe", "video/mpeg");
	POPULATE_MIMETYPE_DB("mp4", "video/mp4");
	POPULATE_MIMETYPE_DB("qt", "video/quicktime");
	POPULATE_MIMETYPE_DB("mov", "video/quicktime");
	POPULATE_MIMETYPE_DB("mxu", "video/vnd.mpegurl");
	POPULATE_MIMETYPE_DB("lsf", "video/x-la-asf");
	POPULATE_MIMETYPE_DB("lsx", "video/x-la-asf");
	POPULATE_MIMETYPE_DB("mng", "video/x-mng");
	POPULATE_MIMETYPE_DB("asf", "video/x-ms-asf");
	POPULATE_MIMETYPE_DB("asx", "video/x-ms-asf");
	POPULATE_MIMETYPE_DB("wm", "video/x-ms-wm");
	POPULATE_MIMETYPE_DB("wmv", "video/x-ms-wmv");
	POPULATE_MIMETYPE_DB("wmx", "video/x-ms-wmx");
	POPULATE_MIMETYPE_DB("wvx", "video/x-ms-wvx");
	POPULATE_MIMETYPE_DB("avi", "video/x-msvideo");
	POPULATE_MIMETYPE_DB("movie", "video/x-sgi-movie");
	POPULATE_MIMETYPE_DB("ice", "x-conference/x-cooltalk");
	POPULATE_MIMETYPE_DB("vrm", "x-world/x-vrml");
// duplicate	POPULATE_MIMETYPE_DB("vrml", "x-world/x-vrml");
// duplicate	POPULATE_MIMETYPE_DB("wrl", "x-world/x-vrml");

	// NOTE: added by hand
	POPULATE_MIMETYPE_DB("flv", "video/x-flv");
	
#undef POPULATE_MIMETYPE_DB
	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the mimetype matching this file extension, or default_str if none matches
 */
const std::string &	mimediag_t::from_file_ext(const std::string &file_ext
					, const std::string &default_str)	throw()
{
	mimetype_db_t::iterator	iter;
	// build the mimetype_db if not yet done
	if( !mimetype_db )	ctor_mimetype_db();
	// try to find this file_ext in the mimetype_db
	iter = mimetype_db->find(file_ext);
	// if not found, return the default_str
	if( iter == mimetype_db->end() )	return default_str;
	// if found, return the mimetype
	return iter->second;
}

/** \brief Return the mimetype matching this file extension, or default_str if none matches
 */
const std::string &	mimediag_t::from_file_path(const file_path_t &file_path
					, const std::string &default_str)	throw()
{
	// extract the file extension from the mimetype
	std::string	file_ext	= file_path.basename_ext();
	// return the mimetype of this file extension
	return from_file_ext(file_ext, default_str);;
}

NEOIP_NAMESPACE_END


