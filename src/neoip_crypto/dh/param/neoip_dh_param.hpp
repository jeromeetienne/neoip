/*! \file
    \brief Header of the \ref dh_param.cpp

*/


#ifndef __NEOIP_DH_PARAM_HPP__ 
#define __NEOIP_DH_PARAM_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman parameters
 */
class dh_param_t : NEOIP_COPY_CTOR_ALLOW{
private:
	gcry_mpi_t		gcry_group;		//!< the group in gcrypt format
	gcry_mpi_t		gcry_generator;		//!< the generator in gcrypt format


	const gcry_mpi_t &	get_gcry_group()	const throw()	{ return gcry_group;	}
	const gcry_mpi_t &	get_gcry_generator()	const throw()	{ return gcry_generator;}

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const dh_param_t &other)		throw();
public:	
	dh_param_t()				throw()	{ zeroing();			}
	dh_param_t(const dh_param_t &other)	throw()	{ zeroing(); copy(other);	}
	~dh_param_t()			 	throw()	{ nullify();			}
	dh_param_t &	operator = (const dh_param_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}

	
	// canonical object management stuff
	void		nullify()				throw();
	bool		is_null()				const throw();

	// ctor with value
	dh_param_t(const std::string & group_str, const std::string & generator_str)	throw();
	int		get_key_len()		const throw();
	
	static dh_param_t	build_ike_mopd_grp1()	throw();

	/*************** Comparison operator	*******************************/
	int	compare(const dh_param_t & other)	const throw();
	bool	operator ==(const dh_param_t & other)	const throw()	{ return compare(other) == 0; }
	bool	operator !=(const dh_param_t & other)	const throw()	{ return compare(other) != 0; }
	bool	operator < (const dh_param_t & other)	const throw()	{ return compare(other) <  0; }
	bool	operator <=(const dh_param_t & other)	const throw()	{ return compare(other) <= 0; }
	bool	operator > (const dh_param_t & other)	const throw()	{ return compare(other) >  0; }
	bool	operator >=(const dh_param_t & other)	const throw()	{ return compare(other) >= 0; }

	/*************** Display function	*******************************/
	std::string	to_string()				const throw();
	friend	std::ostream & operator << (std::ostream & os, const dh_param_t &skey_auth )	throw()
								{ return os << skey_auth.to_string();	}

	/*************** List of friend class	*******************************/
	friend	class dh_privkey_t;
	friend	class dh_shsecret_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DH_PARAM_HPP__  */



