


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     convert data to struct hostent
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert raw data into a struct hostent
 * 
 * - a lot of dirty stuff due to very old backward compatibility
 * 
 * @return 0 on success, -1 on error (aka buffer too small)
 */
static int hostent_build(char *name, char **aliases_arr, int nb_aliases
			, int addr_family, int addr_len, char **addr_arr, int nb_addr
			, struct hostent *result, char *buf_ptr, size_t buf_len)
{
	char *		cur_ptr	= buf_ptr;
	size_t		cur_len	= buf_len;
	char **		aliases_ptr	= alloca(nb_aliases * sizeof(char *));
	char *		first_addr_ptr;
	int		i;

	// put the name in the buffer
	if( cur_len < strlen(name)+1 )	goto not_enougth_space;
	strcpy(cur_ptr, name);
	result->h_name = cur_ptr;
	KLOG_DBG("put h_name=%s\n", cur_ptr);
	cur_ptr += strlen(name)+1;
	cur_len -= strlen(name)+1;

	// put all the aliases
	for( i = 0; i < nb_aliases; i++ ){
		if( cur_len < strlen(aliases_arr[i])+1 )	goto not_enougth_space;
		aliases_ptr[i]	= cur_ptr;
		strcpy(cur_ptr, aliases_arr[i]);
		KLOG_DBG("put alias %dth = %s\n", i, cur_ptr);
		cur_ptr		+= strlen(aliases_arr[i])+1;
		cur_len		-= strlen(aliases_arr[i])+1;
	}

	// aligned the cur_ptr to be aligned with the pointer size
	if( (cur_ptr - buf_ptr) % sizeof(char*) ){
		int	offset = sizeof(char *) - ((cur_ptr - buf_ptr) % sizeof(char*));
		if( cur_len < offset )	goto not_enougth_space;
		cur_ptr += offset;
		cur_len -= offset;
	}

	// put pointer on all aliases (without the NULL at the end)
	result->h_aliases	= (void *)cur_ptr;
	for( i = 0; i < nb_aliases; i++ ){
		if( cur_len < sizeof(char*) )	goto not_enougth_space;
		*((char**) cur_ptr)	= aliases_ptr[i];
		cur_ptr		+= sizeof(char *);
		cur_len		-= sizeof(char *);
	}

	// put the NULL at the end of the aliases pointer in the buffer 
	if( cur_len < sizeof(char *) )	goto not_enougth_space;
	*((char**) cur_ptr)	= NULL;
	cur_ptr			+= sizeof(char *);
	cur_len			-= sizeof(char *);

	// put the address type
	result->h_addrtype	= addr_family;

	// put the address length
	result->h_length 	= addr_len;

	// put all the addresses
	first_addr_ptr		= cur_ptr;
	for( i = 0; i < nb_addr; i++ ){
		if( cur_len < result->h_length )	goto not_enougth_space;
		memcpy(cur_ptr, addr_arr[i], result->h_length);
		cur_ptr		+= result->h_length;
		cur_len		-= result->h_length;
	}

	// aligned the cur_ptr to be aligned with the pointer size
	if( (cur_ptr - buf_ptr) % sizeof(char*) ){
		int	offset = sizeof(char *) - ((cur_ptr - buf_ptr) % sizeof(char*));
		if( cur_len < offset )	goto not_enougth_space;
		cur_ptr += offset;
		cur_len -= offset;
	}

	// put pointer on all addresses (without the NULL at the end)
	result->h_addr_list	= (void *)cur_ptr;
	for( i = 0; i < nb_addr; i++ ){
		if( cur_len < sizeof(char*) )	goto not_enougth_space;
		*((char**) cur_ptr)	= first_addr_ptr + i * result->h_length;
		cur_ptr		+= sizeof(char *);
		cur_len		-= sizeof(char *);
	}

	// put NULL pointer at the end of all addresses
	if( cur_len < sizeof(char*) )	goto not_enougth_space;
	*((char**) cur_ptr)	= NULL;
	cur_ptr		+= sizeof(char *);
	cur_len		-= sizeof(char *);
	
	return 0;
not_enougth_space:	return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             hostent_from_reply_str
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Convert a reply string to a struct hostent
 * 
 * - the hostent struct is valid IIF the reply is found
 * 
 * @return -2 in case of protocol error, -1 if the buffer is too small, 0 if not found and 1 if found
 */
static nss_err_t hostent_from_reply_str(char *reply_str, struct hostent *result
						, char *buf_ptr, size_t buf_len)
{
	char *	strtok_ptrptr	= NULL;
	KLOG_DBG("reply_str=%s\n", reply_str);
	// get the command
	char *	command;
	command = my_strtok( reply_str, '/', &strtok_ptrptr);
	if( !command || !strlen(command) )		return NSS_ERR_PROTO;
	// log to debug
	KLOG_DBG("command=%s\n", command );
	
	// if the command is "notfound", notify the caller
	if( !strcasecmp(command, "notfound") )	return NSS_ERR_NOTFOUND;

	// sanity check - here the command MUST be "found"
	assert( !strcasecmp( command, "found") );
	
	// get the hostname
	char *	hostname_str;
	hostname_str = my_strtok( NULL, '/', &strtok_ptrptr);
	if( !hostname_str || !strlen(hostname_str))	return NSS_ERR_PROTO;
	// log to debug
	KLOG_DBG("hostname_Str=%s\n", hostname_str );
	
	// get the alias_str_list
	char *	alias_str_list;
	alias_str_list = my_strtok( NULL, '/', &strtok_ptrptr);
	if( !alias_str_list )	return NSS_ERR_PROTO;
	// log to debug
	KLOG_DBG("alias_str_list=%s\n", alias_str_list);

	// build the alias array
	char *	alias_arr[1024];
	int	alias_nb;
	char *	alias_strtok_ptrptr	= NULL;
	for( alias_nb = 0; alias_nb < sizeof(alias_arr)/sizeof(alias_arr[0]); alias_nb++ ){
		// get the alias from the alias_str_list
		char *	alias_curs	= (alias_nb == 0 ? alias_str_list : NULL);
		char *	alias_token	= my_strtok(alias_curs, '@', &alias_strtok_ptrptr);
		// if no more alias are present, leave the loop
		if( !alias_token || strlen(alias_token) == 0)	break;
		// log to debug
		KLOG_DBG("alias token %dth alias=[%s]\n", alias_nb, alias_token );
		// copy the alias into the array
		alias_arr[alias_nb]	= alloca(strlen(alias_token)+1);
		strcpy(alias_arr[alias_nb], alias_token);
	}
	// if there are not enougth room to get all the alias, return an error
	if( alias_nb == sizeof(alias_arr)/sizeof(alias_arr[0]) )	return NSS_ERR_PROTO;
	
		
	// get the addr_family_str
	char *	addr_family_str;
	int	addr_family;
	int	addr_len;
	addr_family_str = my_strtok( NULL, '/', &strtok_ptrptr);
	if( !addr_family_str )	return NSS_ERR_PROTO;
	// log to debug
	KLOG_DBG("addr_family_str=%s\n", addr_family_str);
	// convert the addr_family into an integer
	if( !strcasecmp(addr_family_str, "AF_INET") ){
		addr_family	= AF_INET;
		addr_len	= sizeof(ipv4_addr_t);
	}else if( !strcasecmp(addr_family_str, "AF_INET6") ){
		addr_family	= AF_INET6;
		addr_len	= sizeof(ipv6_addr_t);
	}else{
		return NSS_ERR_PROTO;
	}

	// get the addr_str_list
	char *	addr_str_list;
	addr_str_list = my_strtok( NULL, '/', &strtok_ptrptr);
	if( !addr_str_list )	return NSS_ERR_PROTO;
	// log to debug
	KLOG_DBG("addr_str_list=%s\n", addr_str_list);
	
	// build the addr array
	char *	addr_arr[1024];
	int	addr_nb;
	char *	addr_strtok_ptrptr	= NULL;
	for( addr_nb = 0; addr_nb < sizeof(addr_arr)/sizeof(addr_arr[0]); addr_nb++ ){
		// get the addr from the addr_str_list
		char *	addr_curs	= (addr_nb == 0 ? addr_str_list : NULL);
		char *	addr_token	= my_strtok(addr_curs, '@', &addr_strtok_ptrptr);
		// if no more addr are present, leave the loop
		if( !addr_token || strlen(addr_token) == 0)	break;
		// log to debug
		KLOG_DBG("addr token %dth addr=[%s]\n", addr_nb, addr_token );
		// copy the addr into the array
		addr_arr[addr_nb] = alloca( addr_len );
		if( inet_pton(addr_family, addr_token, addr_arr[addr_nb]) <= 0 ){
			KLOG_DBG("blabla addr");
			
			return NSS_ERR_PROTO;
		}
	}
	// if there are not enougth room to get all the addr, return an error
	if( addr_nb == sizeof(addr_arr)/sizeof(addr_arr[0]) )	return NSS_ERR_PROTO;
	// if there are no address at all, return an error
	if( addr_nb == 0 )					return NSS_ERR_PROTO;


// display the struct - just to debug
	KLOG_DBG("hostname_str=%s\n", hostname_str);
	int	i;
	for( i = 0; i < alias_nb; i++ )	KLOG_DBG("alias %dth = %s\n", i, alias_arr[i]);
	KLOG_DBG("addr_family=%s\n", addr_family == AF_INET6 ? "AF_INET6" : "AF_INET" );
	KLOG_DBG("addr_len=%d\n", addr_len );
	for( i = 0; i < addr_nb; i++ ){
		char	tmp_addr_str[1024];
		inet_ntop(addr_family, addr_arr[i], tmp_addr_str, sizeof(tmp_addr_str) );
		KLOG_DBG("addr %dth = %s\n", i, tmp_addr_str);
	}

	// convert the parsed data into a struct hostent
	int failed = hostent_build(hostname_str, alias_arr, alias_nb, addr_family, addr_len
						, addr_arr, addr_nb, result, buf_ptr, buf_len );
	if( failed )	return NSS_ERR_ERANGE;
	
	return NSS_ERR_FOUND;
}


