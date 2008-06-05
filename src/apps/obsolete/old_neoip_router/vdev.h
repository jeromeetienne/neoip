/*===========================[ (c) JME SOFT ]===================================
FILE        : [vdev.h]
CREATED     : 01/03/16 13:09:41		LAST SAVE    : 01/12/11 21:31:33
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================

==============================================================================*/

#ifndef __VDEV_H__
#define __VDEV_H__
/* system include */
#include <glib.h>

#define IFNAMSIZ_LAME 16	// TODO lame solution to a lame bug with libipq include
/* local include */

typedef struct {
	void 	(*pkt_in_callback)( void *user_ptr, int proto, char *buf, int buf_len );
	void	*user_ptr;
	char	dev_name[IFNAMSIZ_LAME];
	
	/* glib stuff */
	GPollFD	gfd;
	GSource *gsource;
} vdev_t;

/* prototype definition */
int vdev_open(vdev_t *vdev);
void vdev_close(vdev_t *vdev);
void vdev_register_callback( vdev_t *vdev, void (*pkt_in_callback)( void *userptr, int proto, char *buf, int buf_len ) );
int vdev_xmit(vdev_t *vdev, char *pkt, int pkt_len, int ethertype);


#endif	/* __VDEV_H__ */

