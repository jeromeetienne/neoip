/*! \file
    \brief Header of the object factory

*/


#ifndef __NEOIP_OBJ_FACTORY_HPP__ 
#define __NEOIP_OBJ_FACTORY_HPP__ 
/* system include */
#include <map>
#include <vector>
/* local include */
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief define a factory plant
 */
template <typename BASE_TYPE> class factory_api_t {
public:
	virtual ~factory_api_t()	{}
	virtual BASE_TYPE *	create() = 0;
};

/** \brief define a factory
 */
template <typename BASE_TYPE, typename DERIVED_TYPE> class factory_product_t
			: public factory_api_t<BASE_TYPE> {
public:
	factory_product_t()		{}
	virtual ~factory_product_t()	{}
	virtual BASE_TYPE * create()	{ return nipmem_new DERIVED_TYPE;		}
};

/** \brief define a factory
 */
template <typename KEY_TYPE, typename BASE_TYPE> class factory_plant_t {
private:
	std::map<KEY_TYPE, factory_api_t<BASE_TYPE> *>	product_map;
public:
	//! create a product from this factory
	BASE_TYPE *	create(const KEY_TYPE &key) const throw() {
		typename std::map<KEY_TYPE, factory_api_t<BASE_TYPE> *>::const_iterator	iter;
		iter = product_map.find(key);
		if( iter == product_map.end() )	return NULL;
		return (*iter).second->create();
	}
	//! add a key and a product in the factory
	void		key_add(const KEY_TYPE &key, factory_api_t<BASE_TYPE> *factory_api ) throw()
			{ bool	succeed = product_map.insert(std::make_pair(key, factory_api)).second;
			  DBG_ASSERT( succeed );	}
	//! remove a key and a product from the factory
	void		key_del(const KEY_TYPE &key ) throw()
			{ product_map.erase(key);		}
			
	//! return true if the key is present, false otherwise
	bool		is_key_present(const KEY_TYPE &key) const throw()
	{
		typename std::map<KEY_TYPE, factory_api_t<BASE_TYPE> *>::const_iterator	iter;
		iter = product_map.find(key);		
		if( iter == product_map.end() )	return false;
		return true;
	}
	
	//! return a list containing all the keys
	std::vector<KEY_TYPE>	get_key_list() const throw()
	{
		typename std::map<KEY_TYPE, factory_api_t<BASE_TYPE> *>::const_iterator	iter;
		typename std::vector<KEY_TYPE>	result;
		for( iter = product_map.begin(); iter != product_map.end(); iter++ ){
			const KEY_TYPE	&key	= (*iter).first;
			result.push_back(key);
		}
		return result;
	}
};

/** \brief to include in the class declaration of a factory product
 */
#define FACTORY_PRODUCT_DECLARATION(BASE_TYPE, DERIVED_TYPE)						\
	static factory_product_t<BASE_TYPE,DERIVED_TYPE>	obj_factory_product; 			\
	BASE_TYPE *	clone()		const throw()	{ return nipmem_new DERIVED_TYPE(*this);}	\
	void 		destroy()	throw() 	{ nipmem_delete this;			}	

#define FACTORY_PRODUCT_DECLARATION_NOCLONE(BASE_TYPE, DERIVED_TYPE)					\
	static factory_product_t<BASE_TYPE,DERIVED_TYPE>	obj_factory_product; 			\
	BASE_TYPE *	clone()		const throw()	{ DBG_ASSERT( 0 );	return NULL;	}	\
	void 		destroy()	throw() 	{ nipmem_delete this;			}		

/** \brief to include in the class definition of a factory product
 */
#define FACTORY_PRODUCT_DEFINITION(BASE_TYPE, DERIVED_TYPE)	\
		factory_product_t<BASE_TYPE,DERIVED_TYPE>	DERIVED_TYPE::obj_factory_product

#define FACTORY_BASE_CLASS_DECLARATION(BASE_TYPE)				\
		virtual BASE_TYPE *	clone()		const throw()	= 0;	\
		virtual void		destroy() 	throw()		= 0;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			#define for factory_plant_t declaration/definition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! helper for the declaration a factory_plant_t
#define FACTORY_PLANT_DECLARATION(FACTORY_VARNAME, KEY_TYPE, BASE_TYPE)	\
		extern factory_plant_t<KEY_TYPE, BASE_TYPE> *FACTORY_VARNAME
#define FACTORY_PLANT_DEFINITION(FACTORY_VARNAME, KEY_TYPE, BASE_TYPE) 		\
		factory_plant_t<KEY_TYPE, BASE_TYPE> * FACTORY_VARNAME	= NULL

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			to insert factory_product_t into factory_plant_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a factory product into a factory plant
*/
template <typename KEY_TYPE, typename BASE_TYPE>
void	factory_plant_insert_product(factory_plant_t<KEY_TYPE, BASE_TYPE> **factory_plant
		, const KEY_TYPE &key, factory_api_t<BASE_TYPE> *factory_api)	throw()
{
	// if the factory_plant_t is not yet created, create it now
	if( *factory_plant == NULL ){
		*factory_plant	= new factory_plant_t<KEY_TYPE, BASE_TYPE>();
	}
	// insert this gsuite_fct/index in the nunit_gsuite
	(*factory_plant)->key_add(key, factory_api);
}


#define FACTORY_PRODUCT_INSERT(FACTORY_VARNAME, KEY_TYPE, BASE_TYPE, KEY_VAL, PRODUCT_CLASS)	\
	factory_plant_insert_product<KEY_TYPE, BASE_TYPE>(&FACTORY_VARNAME, KEY_VAL		\
					, &PRODUCT_CLASS::obj_factory_product)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OBJ_FACTORY_HPP__  */



