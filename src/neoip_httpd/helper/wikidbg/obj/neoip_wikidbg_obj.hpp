/*! \file
    \brief Header of the wikidbg_obj_t
*/


#ifndef __NEOIP_WIKIDBG_OBJ_HPP__ 
#define __NEOIP_WIKIDBG_OBJ_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_wikidbg_obj_static_page.hpp"
#include "neoip_wikidbg_global_db.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

#define WIKIDBG_OBJ_NOTYPE		float
#define WIKIDBG_OBJ_TEMPLATE_DECL	template <typename T, wikidbg_init_cb_t INIT_CALLBACK	\
								, typename T1			\
								, typename T2			\
								, typename T3			\
								, typename T4			\
								, typename T5			\
								, typename T6			\
								, typename T7			\
								, typename T8			\
								>
#define WIKIDBG_OBJ_CLASSNAME		wikidbg_obj_full_t<T, INIT_CALLBACK, T1, T2, T3, T4, T5, T6, T7, T8>

/** \brief class used to easily insert/remove an object from the wikidbg_global_db_t
 * 
 * - The object to store in the wikidbg_global_db_t should inherite from this object
 *   thus all instances of this object will be properly inserted/removed from the 
 *   wikidbg_global_db_t without modifying the object itself.
 */
template <typename T, wikidbg_init_cb_t INIT_CALLBACK
						, typename T1 = WIKIDBG_OBJ_NOTYPE
						, typename T2 = WIKIDBG_OBJ_NOTYPE
						, typename T3 = WIKIDBG_OBJ_NOTYPE
						, typename T4 = WIKIDBG_OBJ_NOTYPE
						, typename T5 = WIKIDBG_OBJ_NOTYPE
						, typename T6 = WIKIDBG_OBJ_NOTYPE
						, typename T7 = WIKIDBG_OBJ_NOTYPE
						, typename T8 = WIKIDBG_OBJ_NOTYPE
						>
class wikidbg_obj_full_t {
private:
	wikidbg_obj_static_page_t *	static_page;
	/*************** internal function	*******************************/
	std::list<void *>	get_alias_ptr_db()	throw();
	void			internal_ctor()		throw();
	void			internal_dtor()		throw();
public:
	/*************** ctor/dtor	***************************************/
	wikidbg_obj_full_t()					throw()	{ internal_ctor();	}
	~wikidbg_obj_full_t()					throw()	{ internal_dtor();	}
	wikidbg_obj_full_t(const wikidbg_obj_full_t &other)	throw()	{ internal_ctor();	}
	wikidbg_obj_full_t &operator=(const wikidbg_obj_full_t& other)	throw();
	
	/*************** action function	*******************************/
	void	wikidbg_obj_add_static_page(const std::string &url_path)	throw()
			{ static_page = new wikidbg_obj_static_page_t(url_path, (T *)this);	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      copy operator and assignement
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief redefinition of the operator =
 */
WIKIDBG_OBJ_TEMPLATE_DECL 
WIKIDBG_OBJ_CLASSNAME &	WIKIDBG_OBJ_CLASSNAME::operator=(const wikidbg_obj_full_t& other)throw()
{
	// Gracefully handle self assignment
	if( this == &other )	return *this;
	// call the internal destructor
	internal_dtor();
	// call the internal constructor
	internal_ctor();
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   internal ctor to centralize all ctor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Private function which is called by all the ctor
 */
WIKIDBG_OBJ_TEMPLATE_DECL	void WIKIDBG_OBJ_CLASSNAME::internal_ctor()	throw()
{
	// zero some field
	static_page	= NULL;
	// Insert this object in the wikidbg_global_db_t
	wikidbg_global_db_t::insert((T *)this, INIT_CALLBACK, get_alias_ptr_db());
}

/** \brief Private function which is called by all the dtor
 */
WIKIDBG_OBJ_TEMPLATE_DECL	void WIKIDBG_OBJ_CLASSNAME::internal_dtor()	throw()
{
	// delete the static_page if needed
	if( static_page ){
		delete	static_page;
		static_page	 = NULL;
	}
	// Insert this object in the wikidbg_global_db_t
	wikidbg_global_db_t::remove((T *)this, get_alias_ptr_db());
}

/** \brief return a std::list of all the alias pointer
 */
WIKIDBG_OBJ_TEMPLATE_DECL	std::list<void *> WIKIDBG_OBJ_CLASSNAME::get_alias_ptr_db()	throw()
{
	std::list<void *>	alias_ptr_db;
	// populate it with all the subtype if they are defined
	if( typeid(T1) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T1 *)(T *)this);
	if( typeid(T2) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T2 *)(T *)this);
	if( typeid(T3) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T3 *)(T *)this);
	if( typeid(T4) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T4 *)(T *)this);
	if( typeid(T5) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T5 *)(T *)this);
	if( typeid(T6) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T6 *)(T *)this);
	if( typeid(T7) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T7 *)(T *)this);
	if( typeid(T8) != typeid(WIKIDBG_OBJ_NOTYPE) )	alias_ptr_db.push_back((T8 *)(T *)this);
	// return the built list
	return alias_ptr_db;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			a place-holder class used when NO_WIKIDBG_OBJ is defined
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T, wikidbg_init_cb_t INIT_CALLBACK
						, typename T1 = WIKIDBG_OBJ_NOTYPE
						, typename T2 = WIKIDBG_OBJ_NOTYPE
						, typename T3 = WIKIDBG_OBJ_NOTYPE
						, typename T4 = WIKIDBG_OBJ_NOTYPE
						, typename T5 = WIKIDBG_OBJ_NOTYPE
						, typename T6 = WIKIDBG_OBJ_NOTYPE
						, typename T7 = WIKIDBG_OBJ_NOTYPE
						, typename T8 = WIKIDBG_OBJ_NOTYPE
						>
class wikidbg_obj_none_t {
public:
	void	wikidbg_obj_add_static_page(const std::string &url_path)throw()	{}
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T, wikidbg_init_cb_t INIT_CALLBACK
						, typename T1 = WIKIDBG_OBJ_NOTYPE
						, typename T2 = WIKIDBG_OBJ_NOTYPE
						, typename T3 = WIKIDBG_OBJ_NOTYPE
						, typename T4 = WIKIDBG_OBJ_NOTYPE
						, typename T5 = WIKIDBG_OBJ_NOTYPE
						, typename T6 = WIKIDBG_OBJ_NOTYPE
						, typename T7 = WIKIDBG_OBJ_NOTYPE
						, typename T8 = WIKIDBG_OBJ_NOTYPE
						>
#if NO_WIKIDBG_OBJ
	class wikidbg_obj_t : public wikidbg_obj_none_t<T, INIT_CALLBACK, T1, T2, T3, T4, T5, T6, T7, T8> {
#else
	class wikidbg_obj_t : public wikidbg_obj_full_t<T, INIT_CALLBACK, T1, T2, T3, T4, T5, T6, T7, T8> {
#endif
};


// list of undef for #define local to this file
#undef	WIKIDBG_OBJ_NOTYPE
#undef	WIKIDBG_OBJ_TEMPLATE_DECL
#undef	WIKIDBG_OBJ_CLASSNAME

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_WIKIDBG_OBJ_HPP__  */










