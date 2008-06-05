/*! \file
    \brief Definition of the \ref bt_jamstd_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_jamstd_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_jamstd_profile_t constant
#if 0
	const size_t		bt_jamstd_profile_t::DH_PUBKEY_LEN	= 768/8;
	const std::string	bt_jamstd_profile_t::DH_GROUP		= 
						"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
						"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
						"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
						"E485B576625E7EC6F44C42E9A63A36210000000000090563";
	const std::string	bt_jamstd_profile_t::DH_GENERATOR		= "2";
	const std::string	bt_jamstd_profile_t::VERIF_CST		= std::string("\0\0\0\0\0\0\0\0",8);	
	const size_t		bt_jamstd_profile_t::PADESTA_MAXLEN	= 512;
	const size_t		bt_jamstd_profile_t::PADAUTH_MAXLEN	= 512;
	const bt_jamstd_negoflag_t bt_jamstd_profile_t::NEGOFLAG_ALLOWED	= 
					bt_jamstd_negoflag_t(bt_jamstd_negoflag_t::PLAINTEXT_VAL
							| bt_jamstd_negoflag_t::ARCFOUR_VAL);
#else
	const size_t		bt_jamstd_profile_t::DH_PUBKEY_LEN	= 768/8;
	const std::string	bt_jamstd_profile_t::DH_GROUP		= 
						"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
						"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
						"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
						"E485B576625E7EC6F44C42E9A63A36210000000000090563";
	const std::string	bt_jamstd_profile_t::DH_GENERATOR		= "2";
	const std::string	bt_jamstd_profile_t::VERIF_CST		= std::string("\0\0\0\0\0\0\0\0",8);	
	const size_t		bt_jamstd_profile_t::PADESTA_MAXLEN	= 512;
	const size_t		bt_jamstd_profile_t::PADAUTH_MAXLEN	= 512;
	const bt_jamstd_negoflag_t bt_jamstd_profile_t::NEGOFLAG_ALLOWED	= 
					bt_jamstd_negoflag_t(bt_jamstd_negoflag_t::PLAINTEXT_VAL
							| bt_jamstd_negoflag_t::ARCFOUR_VAL);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_jamstd_profile_t::bt_jamstd_profile_t()	throw()
{
	// init each plain field with its default value
	dh_pubkey_len	( DH_PUBKEY_LEN		);
	dh_group	( DH_GROUP		);
	dh_generator	( DH_GENERATOR		);
	verif_cst	( VERIF_CST		);
	padesta_maxlen	( PADESTA_MAXLEN	);
	padauth_maxlen	( PADAUTH_MAXLEN	);
	negoflag_allowed( NEGOFLAG_ALLOWED	);
}

/** \brief Destructor
 */
bt_jamstd_profile_t::~bt_jamstd_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_jamstd_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

