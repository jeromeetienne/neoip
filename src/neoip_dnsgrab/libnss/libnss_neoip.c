/*! \file
    \brief This modules implement a gnu nss plugins
    
\par Brief Description
- it intercept all requests and forward them via tcp and a custom protocol to a deamon.
- it is up to the deamon to provide the answer or notfound
- if the deamon is not present, the tcp connection fails and all works without issue
- the daemon is coded in neoip as a dns_grabber_t
  - later to do another daemon which intercept the dns request via IPQUEUE and
    forward them to dns_grabber_t the way it is done in libnss_neoip.c

\par TODO
- the address of the daemon is currently hardcoded here.
  - TODO put it in a configuration file
  - TODO would require to solve who populate this file
  - are multiple listener allowed ? 
    - if so, how to handle them ?
    - with a number in the filename ala init.d ? and all the listener are tried
      in order.
  - how the listener registers in this directory ?
  - how those file are removed ?
- to change the way to address the listener once you got answer for all this :)
    
\par Implementation Notes
- it is all in a single file to avoid exporting symbol
- Some basics stuff    
// gcc -c nss_test.c && gcc -shared -o libnss_neoip.so.2 -Wl,-soname,libnss_neoip.so.2 nss_test.o
// export LD_DEBUG=files
// export -n LD_DEBUG
// export LD_LIBRARY_PATH=/home/jerome/workspace/yavipin/src/apps/neoip_kad_daemon/:$LD_LIBRARY_PATH

    
*/

/* system include */
#include <stdio.h>
#include <nss.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <alloca.h>
#include <stdlib.h>
#include <time.h>
/* local include */

// NOTE: here i include the .c - it seems dirty but prevent to have global name
//       exported in the libnss_neoip and thus avoid name collisions
#include "libnss_neoip_err.c"
#include "libnss_neoip_tools.c"
#include "libnss_neoip_hostent.c"
#include "libnss_neoip_dir.c"
#include "libnss_neoip_socket.c"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          handler processing
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Process all the handler of this directory
 */
static nss_err_t process_handler_in_dir(char *dirpath, struct dirent **name_list, size_t nb_name
			, char *request_buf, struct hostent *result, char *buf_ptr, size_t buf_len)
{
	char		ipaddr_str[1024];
	int		port, timeout_in_sec;
	int		i, err;
	struct stat	stat_buf;
	char		reply_buf[1024];
	nss_err_t	nss_err;
	time_t		limit_date	= time(NULL) - (24*60*60);

	// go thru each name of the directory
	for( i = 0; i < nb_name; i++ ){
		// build the filepath
		char	filepath[PATH_MAX+1];
		snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, name_list[i]->d_name);
		// log to debug
		KLOG_DBG("filepath=%s\n", filepath);
		
		// check if the file is not too old, aka not modified during the last 24h
		err	= stat(filepath, &stat_buf);
		if( err || stat_buf.st_mtime < limit_date ){
			KLOG_DBG("deleting %s as it is too old\n", filepath);
			unlink(filepath);	// delete the invalid handler
			continue;
		}
		
		// open the file
		FILE *	fIn	= fopen(filepath, "rt");
		// check for error
		if( !fIn ){
			KLOG_ERR("Cant open the file %s due to errno %d:%s\n", filepath, errno, strerror(errno));
			unlink(filepath);	// delete the invalid handler
			continue;
		}
		// parse the content of the file
		if( fscanf(fIn, "%s %d %d", ipaddr_str, &port, &timeout_in_sec) != 3 ){
			KLOG_ERR("Cant open the file %s due to errno %d:%s\n", filepath, errno, strerror(errno));
			fclose(fIn);
			unlink(filepath);	// delete the invalid handler
			continue;
		}
		// log to debug
		KLOG_DBG("ipaddr_str=%s\n", ipaddr_str);
		KLOG_DBG("port=%d\n", port);
		KLOG_DBG("timeout_in_sec=%d\n", timeout_in_sec);
		
		// send a request thru a socket and get a reply string 
		int err = fwd_req_thru_socket(ipaddr_str, port, timeout_in_sec
						, request_buf, reply_buf, sizeof(reply_buf));
		if( err ){
			KLOG_ERR("Cant do the protocol exchange over the socket for the file %s\n", filepath);
			fclose(fIn);
			unlink(filepath);	// delete the invalid handler
			continue;			
		}

		// convert the reply string into the hostent
		nss_err = hostent_from_reply_str(reply_buf, result, buf_ptr, buf_len);
		// if the result is found, exit now
		if( nss_err == NSS_ERR_FOUND ){
			fclose(fIn);
			return nss_err;
		}
		// if an protocol error occured, consider this handler invalid
		if( nss_err == NSS_ERR_PROTO ){
			fclose(fIn);
			unlink(filepath);	// delete the invalid handler
			continue;
		}
		
		// close the file
		fclose(fIn);
	}
	// if this point is reached, no handler of this directory provided answer, 
	return NSS_ERR_NOTFOUND;
}

/** \brief Command post process
 * 
 * - this function tests all the handlers registered in ~/.libnss_neoip and
 *   /etc/libnss_neoip.d directory
 */
static nss_err_t cmd_post_process(char *request_buf, struct hostent *result, char *buf_ptr,size_t buf_len)
{
	char			homedir[PATH_MAX+1];
	char			dirpath[PATH_MAX+1];
	struct dirent **	name_list;
	int			nb_name;
	nss_err_t		nss_err	= NSS_ERR_NOTFOUND;

// handle the ~/.libnss_neoip directory
	// build the dirpath
	snprintf(dirpath, sizeof(dirpath), "%s/.libnss_neoip", get_home_dir(homedir, sizeof(homedir)));
	// read the directory
	nb_name		= mydir_open(dirpath, &name_list);
	if( nb_name > 0 ){
		// process the handler in this directory
		nss_err		= process_handler_in_dir(dirpath, name_list, nb_name, request_buf
								, result, buf_ptr, buf_len);
		// close the directory
		mydir_close(name_list, nb_name);
	}

	
// if the result is positive, return now
	if( nss_err == NSS_ERR_FOUND )	return nss_err;

// handle the /etc/libnss_neoip.d directory
	// build the dirpath
	snprintf(dirpath, sizeof(dirpath), "/etc/libnss_neoip.d");
	// read the directory
	nb_name		= mydir_open(dirpath, &name_list);
	if( nb_name > 0 ){
		// process the handler in this directory
		nss_err		= process_handler_in_dir(dirpath, name_list, nb_name, request_buf
								, result, buf_ptr, buf_len);
		// close the directory	
		mydir_close(name_list, nb_name);
	}

	// return the last result
	return nss_err;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        core of NSS interception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief to intercept gethostbyname2 via nss
 */
enum nss_status _nss_neoip_gethostbyname2_r(const char *name, int addr_family, struct hostent *result
			, char *buf_ptr, size_t buf_len, int *errnop, int *h_errnop)
{
	char		request_buf[1024];
	nss_err_t	nss_err;
	// build the request
	if( addr_family == AF_INET ){
		snprintf(request_buf, sizeof(request_buf), "gethostbyname/%s/AF_INET\n", name);
	}else{
		snprintf(request_buf, sizeof(request_buf), "gethostbyname/%s/AF_INET6\n", name);
	}

	// do the command post processing - common to all command
	nss_err = cmd_post_process(request_buf, result, buf_ptr, buf_len);
	return nss_err_to_nss_status(nss_err, errnop, h_errnop);
}

/** \brief to intercept gethostbyname via nss
 * 
 * - just a wrapper on top of gethostbyname2_r
 */
enum nss_status _nss_neoip_gethostbyname_r(const char *name, struct hostent *result, char *buffer
			, size_t buflen, int *errnop, int *h_errnop)
{
	return _nss_neoip_gethostbyname2_r(name, AF_INET, result, buffer, buflen, errnop, h_errnop);
}


/** \brief to intercept gethostbyaddr via nss
 */
enum nss_status _nss_neoip_gethostbyaddr_r(const void * addr, int addr_len, int addr_family
			, struct hostent *result
			, char *buf_ptr, size_t buf_len, int *errnop, int *h_errnop)
{
	char		request_buf[1024];
	char		addr_str[1024];
	nss_err_t	nss_err;
	// convert the address into a string
	inet_ntop( addr_family, addr, addr_str, sizeof(addr_str) );
	// build the request
	if( addr_family == AF_INET ){
		snprintf(request_buf, sizeof(request_buf), "gethostbyaddr/%s/AF_INET\n", addr_str);
	}else{
		snprintf(request_buf, sizeof(request_buf), "gethostbyaddr/%s/AF_INET6\n", addr_str);
	}

	// do the command post processing - common to all command
	nss_err = cmd_post_process(request_buf, result, buf_ptr, buf_len);
	return nss_err_to_nss_status(nss_err, errnop, h_errnop);
}
