/*! \file
    \brief Header of the neoip_timeout
    
*/


#ifndef __NEOIP_TIMEOUT_LEVT_HPP__ 
#define __NEOIP_TIMEOUT_LEVT_HPP__ 
/* system include */
#include <string>
#include <event.h>
#ifdef _WIN32
#	undef ERROR		// remove dirty define from mingw
#	undef INFINITE
#	undef OPTIONAL
#endif
/* local include */
#include "neoip_timeout_wikidbg.hpp"
#include "neoip_timeout_cb.hpp"
#include "neoip_delay.hpp"
#include "neoip_date.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#ifndef USE_ELOOP_LEVT
#	error	"including a .hpp from a improper eloop implementation"
#endif

// forward declaration for libevent
struct event;

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a timeout
 * 
 * - \ref timeout_t provides a iterative timer aka it will repeat until it is stopped/destructed
 * - it may be copied by copy constructor or assignement operator
 * - it may be stopped by timeout_t::stop()
 * - the period may be changed by timeout_t::change_period()
 */
class timeout_t : NEOIP_COPY_CTOR_DENY, private wikidbg_obj_t<timeout_t, timeout_wikidbg_init> {
private:
	delay_t		period;				//!< the period between timeout expiration
	date_t		next_expire_date;		//!< the date of the next expiration. The timeout
							//!< is considered running iif not null

	/************** internal function	*******************************/
	void		launch_timerext(const delay_t &delay)		throw();
	void 		cpp_expired_cb()				throw();

	/**************	external timer stuff	*******************************/
	void		external_ctor()					throw();
	void		external_start(const delay_t &delay)		throw();
	void		external_stop()					throw();
	bool		external_is_started()				const throw();
	friend void	external_callback(int fd, short event, void *userptr);
	struct event *	external_ctx;
	char		external_ctx_buf[sizeof(struct event)];	// trick to avoid declaring struct event here and not
								// have #include <event.h> which contains a lot of #define

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
	bool		is_started()				const throw();
	delay_t 	get_expire_delay()			const throw();

	/*************** compatibility layer	*******************************/
	bool		is_running()				const throw()	{ return is_started();	}

	/*************** action function	*******************************/
	void		start(const delay_t &period, timeout_cb_t * callback, void *userptr) throw();
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
	/*************** list of friend	class	*******************************/
	friend class	timeout_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TIMEOUT_LEVT_HPP__  */



