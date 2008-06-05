

/* system include */
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <fnmatch.h>
/* local include */

/** \brief Return the home directory of the current user or a NULL
 */
static char *	get_home_dir(char *homedir_ptr, size_t homedir_len)
{
	struct passwd	pw_stt;
	struct passwd *	pwbufp;
	char		buf[10*1024];
	// get the passwd struct for the current uid
	if( getpwuid_r(getuid(), &pw_stt, buf, sizeof(buf), &pwbufp) )
		return NULL;
	// check that the destination buffer is large enougth
	if( strlen(pw_stt.pw_dir) >= homedir_len )
		return NULL;
	// copy the string
	strcpy(homedir_ptr, pw_stt.pw_dir);
	// return a pointer on the buffer
	return homedir_ptr;
}

/** \brief filter for mydir_open
 * 
 * - only accept regular file
 */
static int mydir_filter(const struct dirent *dir_entry)
{
	// reject the name if it doesnt start by prio*
	if( fnmatch("prio*", dir_entry->d_name, 0 ) )	return 0;
	// else accept it
	return 1;
}

/** \brief open and read the whole directory
 * 
 * @return the number of name in this directory
 */
static int	mydir_open(char *dir_str, struct dirent ***name_list)
{
	int	nb_name = scandir(dir_str, name_list, mydir_filter, alphasort);
	// log to debug
	KLOG_DBG("nb_name=%d\n", nb_name);
	// handle the 
	if( nb_name < 0 ){
		KLOG_ERR("Cant read dir %s due to errno %d:%s\n", dir_str, errno, strerror(errno));
		return -1;
	}
	return nb_name;
}

/** \brief close and free the whole directory
 */
static void mydir_close(struct dirent **name_list, size_t nb_name)
{
	int	i;
	// free each entry in the list
	for( i = 0; i < nb_name; i++ )	free(name_list[i]);
	// free the list itself
	free(name_list);
}



