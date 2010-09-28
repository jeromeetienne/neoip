/*! \file
    \brief Definition of the \ref router_apps_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_router_apps_helper.hpp"
#include "neoip_router.hpp"
#include "neoip_file.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_base64.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        router_lident_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the router_lident_t and save it in config_path
 * 
 * @auth_cert	the x509_cert_t of the authority. if is_null(), generate a self signed x509_cert_t
 */
router_err_t	router_apps_helper_t::lident_register(const file_path_t &config_path
				, const router_name_t &dnsname
				, size_t key_len, const x509_cert_t &ca_cert
				, const x509_privkey_t &ca_privkey)		throw()
{
	router_lident_t	router_lident;
	router_err_t	router_err;
	file_err_t	file_err;
	// sanity check - dnsname is either is_fully_qualified or is_host_only
	DBG_ASSERT( dnsname.is_fully_qualified() || dnsname.is_host_only() );
	// sanity check - ca_cert and ca_privkey are either both is_null() or none are 
	DBG_ASSERT( ca_cert.is_null() == ca_privkey.is_null() );

	// determine the local_peerid based on the type of dnsname
	router_peerid_t	local_peerid;
#if 0	// TODO to remove - obsolete stuff from the pre-nonesigned era
	if( dnsname.is_fully_qualified() )	local_peerid = dnsname.to_string();
	else					local_peerid = router_peerid_t::build_random();
#else
	if( dnsname.is_selfsigned_ok() ){
		local_peerid = router_peerid_t::build_random();
	}else if( dnsname.is_authsigned_ok() ){
		local_peerid = dnsname.to_string();
	}else if( dnsname.is_nonesigned_ok() ){
		local_peerid = router_peerid_t::from_canonical_string(dnsname.host());
	}else{
		return router_err_t(router_err_t::ERROR, "invalid router name " + dnsname.to_string());
	}
#endif

	// get the local identity for router
	router_lident	= router_lident_t::generate(local_peerid, dnsname, key_len, ca_cert, ca_privkey);
	DBG_ASSERT( !router_lident.is_null() );

	// if router_lident.is_authsigned() or is_nonesigned(), write only *_priv and return
	if( router_lident.is_authsigned() ){
		DBG_ASSERT( dnsname.is_fully_qualified() );
		// generate the filename_priv
		file_path_t	filename_priv;
		filename_priv	= dnsname.to_string() + ".authsigned_priv";
		filename_priv	= lib_session_get()->temp_rootdir() / filename_priv;
		// write the file
		file_err	= file_sio_t::writeall(filename_priv, datum_t(router_lident.to_canonical_string()));
		if( file_err.failed() )	return router_err_from_file(file_err);
		// return no error
		return router_err_t::OK;
	}

	// NOTE: at this point, it MUST be a selfsigned or is_nonesigned identity
	DBG_ASSERT( ca_cert.is_null() );
	DBG_ASSERT( router_lident.is_selfsigned() || router_lident.is_nonesigned() );
	
	// if router_lident.is_selfsigned, save the "selfsigned_cert"
	if( router_lident.is_selfsigned() ){
		// compute the filename of the public identity
		file_path_t	filename_cert;
		filename_cert	= config_path / "local_identity" / (dnsname.host() + ".selfsigned_cert");
		// write the .selfsigned_cert file
		router_err	= rident_save(filename_cert, router_lident.to_rident());
		if( router_err.failed() )	return router_err;
	}

	// determine the basename for the *_priv file 
	std::string	basename_priv;
	if( router_lident.is_selfsigned() )	basename_priv	= dnsname.host() + ".selfsigned_priv";
	else 					basename_priv	= dnsname.to_string() + ".nonesigned_priv";	
	// write the *_priv file
	file_path_t	filename_priv	=  config_path / "local_identity" / basename_priv;
	file_err	= file_sio_t::writeall(filename_priv, datum_t(router_lident.to_canonical_string()));
	if( file_err.failed() )		return router_err_from_file(file_err);

	// if there is already a current_identity.priv, remove it, to overwrite it with the new
	if( !file_stat_t(config_path / "current_identity.priv").is_null() ){
		file_utils_t::remove_file(config_path / "current_identity.priv");
	}

	// create a symlink between the .selfsigned_priv identity and the current_identity.priv	
	file_err	= file_utils_t::create_symlink( file_path_t("local_identity") / basename_priv
						, config_path / "current_identity.priv");
	if( file_err.failed() )		return router_err_from_file(file_err);

	// return no error
	return router_err_t::OK;
}


/** \brief Load the router_lident_t from the config_path
 */
router_err_t	router_apps_helper_t::lident_load(const file_path_t &config_path
					, router_lident_t &router_lident)	throw()
{
	file_path_t	file_path	= config_path / "current_identity.priv";	
	datum_t		file_datum;
	file_err_t	file_err;
	// load the file
	file_err	= file_sio_t::readall(file_path, file_datum);
	if( file_err.failed() )	return router_err_from_file(file_err);
	// build a string from the content of the file
	std::string	str	= file_datum.to_stdstring();
	// parse the string to produce a router_lident_t 
	router_lident	= router_lident_t::from_canonical_string(str);
	// test if the router_lident_t has been successfully imported
	if( router_lident.is_null() )
		return router_err_t(router_err_t::ERROR, file_path.to_string() + " has an invalid format");
	// return noerror
	return router_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           router_rident_t load/save
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Save a router_rident_t to a given file_path
 */
router_err_t router_apps_helper_t::rident_save(const file_path_t &file_path
					, const router_rident_t &router_rident) throw()
{
	file_err_t	file_err;	
	// save the file for the router_rident_t
	file_err	= file_sio_t::writeall(file_path, datum_t(router_rident.to_canonical_string()));
	if( file_err.failed() )	return router_err_from_file(file_err);
	// return no error
	return router_err_t::OK;
}

/** \brief Load a router_rident_t from a file
 */
router_err_t router_apps_helper_t::rident_load(const file_path_t &file_path
					, router_rident_t &router_rident)	throw()
{
	datum_t		file_datum;
	file_err_t	file_err;
	// load the file
	file_err	= file_sio_t::readall(file_path, file_datum);
	if( file_err.failed() )	return router_err_from_file(file_err);
	// parse the string to produce a router_rident_t 
	router_rident	= router_rident_t::from_canonical_string( file_datum.to_stdstring() );
	// test if the router_rident_t has been successfully imported
	if( router_rident.is_null() )
		return router_err_t(router_err_t::ERROR, file_path.to_string() + " has an invalid format");
	// return no error
	return router_err_t::OK;
}

/** \brief Load the router_rident_t from the config_path
 */
router_err_t	router_apps_helper_t::rident_arr_load(const file_path_t &config_path
					, router_rident_arr_t &rident_arr)	throw()
{
	file_path_t		dirname	= config_path / "authorized_peer.d";	
	file_dir_t		file_dir;
	router_err_t		router_err;
	file_err_t		file_err;
	// open the directory
	file_err	= file_dir.open(dirname);
	if( file_err.failed() )	return router_err_from_file(file_err);
	// keep only the file_path matching *.public
	file_dir.filter( file_dir_t::filter_glob_nomatch("*.selfsigned_cert") );
	// go thru all the matching file_path_t
	for(size_t i = 0; i < file_dir.size(); i++){
		router_rident_t	router_rident;
		// load the file itself
		router_err	= rident_load(file_dir[i], router_rident);
		// if the loading failed, report an error
		if( router_err.failed() )	return router_err;
		// add this router_rident_t to the array
		rident_arr	+= router_rident;
	}
	// return no error
	return router_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        router_rootca_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the router_rootca_t and save it in config_path
 * 
 * @auth_cert	the x509_cert_t of the authority. if is_null(), generate a self signed x509_cert_t
 */
router_err_t	router_apps_helper_t::rootca_create(const file_path_t &config_path
				,const router_name_t &domain_name, size_t key_len)	throw()
{
	file_path_t	dirname	= config_path / "rootca";	
	file_path_t	file_path;
	file_err_t	file_err;
	crypto_err_t	crypto_err;
	// log to debug
	KLOG_DBG("enter dnsname=" << domain_name);
	// sanity check - if the domain_name MUST be is_domain_only
	DBG_ASSERT( domain_name.is_domain_only() );

	// generate the private key
	x509_privkey_t	rootca_privkey;
	crypto_err	= rootca_privkey.generate(key_len);
	if( crypto_err.failed() )	return router_err_from_crypto(crypto_err);
	// generate the certificate request needed to build the certificate 
	x509_request_t	rootca_request;
	crypto_err	= rootca_request.generate(domain_name.to_string(), rootca_privkey);
	if( crypto_err.failed() )	return router_err_from_crypto(crypto_err);
	// generate the selfsigned certificate for the just built request
	x509_cert_t	rootca_cert;
	crypto_err	= rootca_cert.generate_selfsigned(rootca_request, rootca_privkey);
	if( crypto_err.failed() )	return router_err_from_crypto(crypto_err);

	// build the router_rootca_t
	router_rootca_t	router_rootca(rootca_cert, item_arr_t<router_name_t>().append(domain_name));
	DBG_ASSERT( !router_rootca.is_null() );

	// remove the heading "*." from the domain name to build the file_path_t basename
	DBG_ASSERT( domain_name.to_string().substr(0, 2) == "*.");
	std::string	basename	= domain_name.to_string().substr(2);

	// save the router_rootca_t private key in a file - as a to_der_datum
	file_path	= dirname / file_path_t(basename + ".rootca_priv");
	file_err	= file_sio_t::writeall(file_path
				, datum_t(base64_t::encode(rootca_privkey.to_der_datum()))
				, file_perm_t::USR_RW_, file_mode_t::WPLUS);
	if( file_err.failed() )	return router_err_from_file(file_err);

	// save the router_rootca_t in a file as a canonical string
	file_path	= dirname / file_path_t(basename + ".rootca_cert");
	file_err	= file_sio_t::writeall(file_path
				, datum_t(router_rootca.to_canonical_string())
				, file_perm_t::USR_RW_, file_mode_t::WPLUS);
	if( file_err.failed() )	return router_err_from_file(file_err);

	// return no error
	return router_err_t::OK;
}

/** \brief Load a router_rootca_t from a file
 */
router_err_t	router_apps_helper_t::rootca_load(const file_path_t &file_path
					, router_rootca_t &router_rootca)	throw()
{
	datum_t		file_datum;
	file_err_t	file_err;
	// load the file
	file_err	= file_sio_t::readall(file_path, file_datum);
	if( file_err.failed() )	return router_err_from_file(file_err);
	// parse the string to produce a router_rootca_t 
	router_rootca	= router_rootca_t::from_canonical_string( file_datum.to_stdstring() );
	// test if the router_rootca_t has been successfully imported
	if( router_rootca.is_null() )
		return router_err_t(router_err_t::ERROR, file_path.to_string() + " has an invalid format");
	// return no error
	return router_err_t::OK;
}

/** \brief Load the router_rootca_t from the config_path/rootca
 * 
 * - NOTE: should be used ONLY to read the rootca to check validity 
 *   - not to generate new identity. (for this use ::rootca_load_for_authsign())
 */
router_err_t	router_apps_helper_t::rootca_arr_load(const file_path_t &config_path
					, router_rootca_arr_t &rootca_arr)	throw()
{
	file_path_t	dirname	= config_path / "rootca";	
	file_dir_t	file_dir;
	file_err_t	file_err;
	router_err_t	router_err;
	// open the directory
	file_err	= file_dir.open(dirname);
	if( file_err.failed() )	return router_err_from_file(file_err);
	// keep only the filename matching *.public
	file_dir.filter( file_dir_t::filter_glob_nomatch("*.rootca_cert") );
	// go thru all the matching file_path_t
	for(size_t i = 0; i < file_dir.size(); i++){
		router_rootca_t	router_rootca;
		// load the file itself
		router_err	= rootca_load(file_dir[i], router_rootca);
		// if the loading failed, report an error
		if( router_err.failed() )	return router_err;
		// add this router_rootca_t to the array
		rootca_arr	+= router_rootca;
	}
	// return no error
	return router_err_t::OK;
}

/** \brief load the x509_cert_t/x509_privkey_t for the dnsname to authsign with them
 * 
 * - NOTE: the .rootca_priv/.rootca_cert are taken from the current directory
 * - the resulting .authsigned_cert is to be stored in lib_session->temp_rootdir
 */
router_err_t	router_apps_helper_t::rootca_load_for_authsign(const router_name_t &dnsname
			, x509_cert_t &ca_cert, x509_privkey_t &ca_privkey)	throw()
{
	const router_rootca_t *	router_rootca;
	router_rootca_arr_t	rootca_arr;
	file_path_t	dirname	= ".";	
	file_dir_t	file_dir;
	file_err_t	file_err;
	router_err_t	router_err;
	
	/*************** read all the .rootca_cert of current dir	*******/
	// open the directory
	file_err	= file_dir.open(dirname);
	if( file_err.failed() )	return router_err_from_file(file_err);

	// keep only the filename matching *.rootca_cert
	file_dir.filter( file_dir_t::filter_glob_nomatch("*.rootca_cert") );
	// go thru all the matching file_path_t
	for(size_t i = 0; i < file_dir.size(); i++){
		router_rootca_t	router_rootca;
		// load the file itself
		router_err	= rootca_load(file_dir[i], router_rootca);
		// if the loading failed, report an error
		if( router_err.failed() )	return router_err;
		// add this router_rootca_t to the array
		rootca_arr	+= router_rootca;
	}
	
	// find the router_rootca_t matching this dnsname 
	router_rootca	= rootca_arr.find_by_dnsname(dnsname);
	if( !router_rootca ){
		std::string	reason = "Unable to find a router_rootca_t handling " + dnsname.to_string();
		return router_err_t(router_err_t::ERROR, reason);
	}

	// copy the router_rootca_t x509_cert_t into ca_cert
	ca_cert		= router_rootca->cert();

	// determine the file_path_t for the ca_privkey
	DBG_ASSERT( router_rootca->domain_db()[0].to_string().substr(0, 2) == "*.");
	std::string	basename	= router_rootca->domain_db()[0].to_string().substr(2);
	file_path_t	file_path	= basename + ".rootca_priv";

	// load the file containing the ca_privkey for this router_rootca_t
	datum_t		privkey_der_datum;
	file_err	= file_sio_t::readall(file_path, privkey_der_datum);
	if( file_err.failed() ){
		std::string	reason = "Unable to find a router_rootca_t privkey for " + router_rootca->to_string();
		return router_err_t(router_err_t::ERROR, reason);
	}

	// convert the privakey_der_datum into a ca_privkey
	ca_privkey	= x509_privkey_t::from_der_datum(base64_t::decode(privkey_der_datum));
	if( ca_privkey.is_null() ){
		std::string	reason = "Unable to convert the content of " + file_path.to_string() + " into a x509_privkey_t";
		return router_err_t(router_err_t::ERROR, reason);
	}

	// return no error
	return router_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           router_acache_t load/save
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Load the router_acache_t from the cache_rootpath and load it in router_peer
 */
router_err_t router_apps_helper_t::acache_load(const file_path_t &cache_rootpath
					, router_acache_t &router_acache)	throw()
{
	file_path_t	file_path	= cache_rootpath / "router_acache.db";
	datum_t		file_datum;
	file_err_t	file_err;
	// load the file
	file_err	= file_sio_t::readall(file_path, file_datum);
	if( file_err.failed() )	return router_err_t::OK;

	// unserialize the router_acache from the read data
	bytearray_t	bytearray	= bytearray_t(file_datum);
	try {
		bytearray >> router_acache;
	}catch(serial_except_t &e){
		router_acache	= router_acache_t();
	}
	// test if the router_acache_t has been successfully imported
	if( router_acache.is_null() )
		return router_err_t(router_err_t::ERROR, file_path.to_string() + " has an invalid format");
	// return no error
	return router_err_t::OK;
}

/** \brief Save the router_acache_t from router_peer_t to the config_path
 */
router_err_t	router_apps_helper_t::acache_save(const file_path_t &cache_rootpath
					, const router_acache_t &router_acache)	throw()
{
	file_path_t	file_path	= cache_rootpath / "router_acache.db";	
	bytearray_t	bytearray;
	// build the bytearray_t containing the router_acache_t serialized
	bytearray	= bytearray_t().serialize( router_acache );
	// save the file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, bytearray.to_datum());
	if( file_err.failed() )	return router_err_from_file(file_err);
	// return no error
	return router_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        disp_info_* function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Display the information about router_lident_t
 */
router_err_t	router_apps_helper_t::disp_info_lident()		throw()
{
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	bool			verbose_on	= arg_option.contain_key("verbose");
	router_lident_t		router_lident;
	router_profile_t	profile;
	router_err_t		router_err;
	// try to load the current router_lident_t
	router_err	= lident_load(config_path, router_lident);
	if( router_err.failed() ){
		KLOG_STDOUT("Local Identity: unable to load it due to " << router_err << "\n");
		return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	}
	
	// display the information about the router_lident_t
	KLOG_STDOUT("Local Identity: " << router_lident.dnsfqname(profile));
	if( router_lident.is_selfsigned() )		KLOG_STDOUT(" (selfsigned)");
	else if( router_lident.is_authsigned() )	KLOG_STDOUT(" (authsigned)");
	else if( router_lident.is_nonesigned() )	KLOG_STDOUT(" (nonesigned)");
	else	DBG_ASSERT( 0 );
	KLOG_STDOUT("\n");
	
	// if the output IS NOT verbose, return now
	if( !verbose_on )	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	
	// display information about the x509_cert_t
	KLOG_STDOUT("\tCertificate:\t" << router_lident.cert().algo_name()
				<< " key of "	<< router_lident.cert().key_length()	<< "-bit"
				<< " with "	<< router_lident.cert().subject_name()	<< " subject"
				<< " and "	<< router_lident.cert().issuer_name()	<< " issuer.");
	KLOG_STDOUT("\n");
	// display information about the router_peerid_t
	KLOG_STDOUT("\tPeerID:\t\t" << router_lident.peerid());
	KLOG_STDOUT("\n");

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}

/** \brief Display the information about router_rident_arr_t
 */
router_err_t	router_apps_helper_t::disp_info_rident_arr()		throw()
{
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	bool			verbose_on	= arg_option.contain_key("verbose");
	router_rident_arr_t	rident_arr;
	router_profile_t	profile;
	router_err_t		router_err;
	// try to load the current router_rident_arr_t
	router_err	= rident_arr_load(config_path, rident_arr);
	if( router_err.failed() ){
		KLOG_STDOUT("Remote selfsigned identity: unable to load it due to " << router_err << "\n");
		return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	}
	
	// display the information about the router_lident_t
	KLOG_STDOUT("Remote selfsigned identity: " << rident_arr.size() << " of them.");
	KLOG_STDOUT("\n");
	
	// if the output IS NOT verbose, return now
	if( !verbose_on )	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	
	// go thru the whole router_rident_arr_t
	for(size_t i = 0; i < rident_arr.size(); i++){
		const router_rident_t &		router_rident	= rident_arr[i];
		KLOG_STDOUT("\tIdentity " << i << ": " 
						<< router_rident.dnsfqname(profile)	<< "\n");
		KLOG_STDOUT("\t\t" << router_rident.cert().algo_name()
				<< " key of "	<< router_rident.cert().key_length()	<< "-bit\n");
		KLOG_STDOUT("\t\tPeerID:\t"	<< router_rident.peerid()		<< "\n");
	}

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}


/** \brief Display the information about router_rootca_arr_t
 */
router_err_t	router_apps_helper_t::disp_info_rootca_arr()		throw()
{
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	bool			verbose_on	= arg_option.contain_key("verbose");
	router_rootca_arr_t	rootca_arr;
	router_err_t		router_err;
	// try to load the current router_rootca_arr_t
	router_err	= rootca_arr_load(config_path, rootca_arr);
	if( router_err.failed() ){
		KLOG_STDOUT("Certificate Authority: unable to load it due to " << router_err << "\n");
		return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	}
	
	// display the information about the router_lident_t
	KLOG_STDOUT("Certificate Authority: " << rootca_arr.size() << " of them.");
	KLOG_STDOUT("\n");
	
	// if the output IS NOT verbose, return now
	if( !verbose_on )	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	
	// go thru the whole router_rootca_arr_t
	for(size_t i = 0; i < rootca_arr.size(); i++){
		const router_rootca_t &		router_rootca	= rootca_arr[i];
		const item_arr_t<router_name_t> &	domain_db	= router_rootca.domain_db();
		KLOG_STDOUT("\tAuthority " << i << ": " << router_rootca.cert().algo_name()
					<< " key of "	<< router_rootca.cert().key_length()	<< "-bit");
		for(size_t j = 0; j < domain_db.size(); j++){
			const router_name_t & domain	= domain_db[j];
			if( (j % 4) == 0 )	KLOG_STDOUT("\n\t\t");
			KLOG_STDOUT(domain.to_string());
		}
		KLOG_STDOUT("\n");
	}

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}


/** \brief Display the information about router_acl_t
 */
router_err_t	router_apps_helper_t::disp_info_acl()		throw()
{
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	bool			verbose_on	= arg_option.contain_key("verbose");
	router_acl_t		router_acl;
	router_err_t		router_err;
	// load the router_acl_t 
	router_err	= router_acl.load_file(config_path / "hostname_acl.conf");

	// handle in case of error
	if( router_err.failed() ){
		// TODO to code
		DBG_ASSERT( 0 );		
		return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	}
	
	// display the information about the router_lident_t
	KLOG_STDOUT("Identity ACL: " << router_acl.size() << " of them.");
	KLOG_STDOUT("\n");
	
	// if the output IS NOT verbose, return now
	if( !verbose_on )	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	
	// go thru the whole router_rootca_arr_t
	for(size_t i = 0; i < router_acl.size(); i++){
		const router_acl_item_t & acl_item	= router_acl[i];
		KLOG_STDOUT("\t" << acl_item.type()	<< " "	<< acl_item.pattern() << "\n");
	}

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}


/** \brief Display the information about router_acache_t
 */
router_err_t	router_apps_helper_t::disp_info_acache()		throw()
{
	file_path_t		cache_rootpath	= lib_session_get()->cache_rootdir();
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	bool			verbose_on	= arg_option.contain_key("verbose");
	router_acache_t		router_acache;
	router_err_t		router_err;
	// try to load the current router_acache_t
	router_err	= acache_load(cache_rootpath, router_acache);
	if( router_err.failed() ){
		KLOG_STDOUT("IP address cache: unable to load it due to " << router_err << "\n");
		return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	}
		
	// display the information about the router_lident_t
	KLOG_STDOUT("IP address cache: " << router_acache.size() << " of them.");
	KLOG_STDOUT("\n");
	
	// if the output IS NOT verbose, return now
	if( !verbose_on )	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
	
	// go thru the whole router_rootca_arr_t
	for(size_t i = 0; i < router_acache.size(); i++){
		const router_acache_item_t & acache_item	= router_acache[i];
		KLOG_STDOUT("\tWith " << acache_item.remote_dnsname()		<< "\t"
				<< "local:" << acache_item.local_iaddr()	<< "\t"
				<< "remote:"<< acache_item.remote_iaddr()	<< "\n");
	}

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}

NEOIP_NAMESPACE_END


