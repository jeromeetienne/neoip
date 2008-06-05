/*! \file
    \brief Header of the neoip_timeout
    
*/


#ifndef __NEOIP_TIMEOUT_GLIB_HPP__ 
#define __NEOIP_TIMEOUT_GLIB_HPP__ 
/* system include */
#include <string>
#include <glib.h>
/* local include */
#include "neoip_timeout_wikidbg.hpp"
#include "neoip_timeout_cb.hpp"
#include "neoip_delay.hpp"
#include "neoip_date.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#ifndef USE_ELOOP_GLIB
#	error	"including a .hpp from a improper eloop implementation"
#endif

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a timeout
 * 
 * - \ref timeout_t provides a iterative timer aka it will repeat until it is stopped/destructed
 * - it may be copied by copy constructor or assignement operator
 * - it may be stopped by timeout_t::stop()
 * - the period may be changed by timeout_t::change_period()
 */
class timeout_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<timeout_t, timeout_wikidbg_init>
			{
private:
	delay_t		period;				//!< the period between timeout expiration
	date_t		next_expire_date;		//!< the date of the next expiration. The timeout
							//!< is considered running iif != delay_t::NEVER

	guint		glib_timeout;			//!< glib timeout tag

	void 		cpp_expired_cb()				throw();
	void		launch_glib_timeout(const delay_t &delay)	throw();

	/*************** callback stuff	***************************************/
	timeout_cb_t *	callback;			//!< callback to notify timeout expiration
	void *		userptr;			//!< the userptr associated with the callback
	bool		notify_callback()		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	timeout_t() 			throw();
	~timeout_t()			throw();
	
	/*************** query function	***************************************/
	bool		is_null()				const throw()	{ return callback == NULL;	}
	const delay_t &	get_period()				const throw()	{ return period;		}
	bool		is_running()				const throw();
	delay_t 	get_expire_delay()			const throw();

	/*************** action function	*******************************/
	void		start(const delay_t &period, timeout_cb_t * callback, void *userptr)	throw();
	void		stop()					throw();
	void		change_period(const delay_t &period)	throw();

	/*************** comparison operator	*******************************/
	int	compare(const timeout_t & other)	const throw();
	bool	operator ==(const timeout_t & other)  	const throw()	{ return compare(other) == 0; }
	bool	operator !=(const timeout_t & other)  	const throw()	{ return compare(other) != 0; }
	bool	operator < (const timeout_t & other)  	const throw()	{ return compare(other) <  0; }
	bool	operator <=(const timeout_t & other)  	const throw()	{ return compare(other) <= 0; }
	bool	operator > (const timeout_t & other)  	const throw()	{ return compare(other) >  0; }
	bool	operator >=(const timeout_t & other)  	const throw()	{ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string	to_string()				const throw();
	friend std::ostream & operator << (std::ostream & os, const timeout_t & timeout)	throw()
							{ return os << timeout.to_string();	}

	/*************** list of friend	***************************************/
	friend class	timeout_wikidbg_t;
	friend gboolean	neoip_timeout_glib_expire(gpointer userptr);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TIMEOUT_GLIB_HPP__  */



