/*! \file
    \brief Bunch of define to build a default argpack_t
    
NOTE: currently it is not much tested - it is in incubation   

\par Brief Description
- a argpack is a object in which parameter are set and may be retrieved later
- the argpack is copiable
- it has been coded as a 'class builder' aka 
  - all the parameter to create an class are stored in a argpack
  - the class to be created accept the argpack and create the object depending on it
  - the advantage of this is tunability
    - e.g. with socket_itor_t, by defining a socket_itor_arg_t, it is possible
      to determine how socket_itor_t are created by passing only a single copiable
      object. thus in a big layer, like kademlia, it is possible a socket_itor_arg_t to determine
      how socket are itor, thus controling the whole low-level networking from outside the kademlia
      layer
  - moreover they are 'stackable' aka a argpack may contains other argpack
    - e.g. on the socket example, one could pass a socket_arg_t to kademlia and 
      socket_arg_t would contain, socket_itor_arg_t, socket_resp_arg_t, socket_full_arg_t
  - another advantage is clarity and succint-ness
    - the parameter are 'named' for the caller => more clear
    - there is less code to handle each parameter in the object to be created

\par TODO
- fix the issue with const/non const in NEOIP_ARGPACK_DECL_ITEM*_PTR
  - NEOIP_ARGPACK_DECL_ITEM doesnt work for pointer
  - this create unnecessary duplication of code, poor source code alignement - unclean

\par Possible Improvement
- a function able to check the validity of all the argument
  - like their values are coherent
  - all mandatory parameters have been set
- a function to normalize the result - would allow to compute parameter
  - arg.normalize().check() return false if no error, true otherwise
- do a default serialization and display on optionnal define
- ability to add any custom function

*/

#ifndef __NEOIP_ARGPACK_HPP__ 
#define __NEOIP_ARGPACK_HPP__ 

/* system include */
/* local include */
#include "neoip_assert.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;


#define NEOIP_ARGPACK_DECL_CLASS_BEGIN(class_name)						\
	class class_name : NEOIP_COPY_CTOR_ALLOW {						\
		public: class_name() throw();
#define NEOIP_ARGPACK_DECL_IS_VALID_FCT(class_name)						\
		public:	bool	is_valid()	const throw();
#define NEOIP_ARGPACK_DECL_IS_VALID_STUB(class_name)						\
		public:	bool	is_valid()	const throw()	{ return true;		}
#define NEOIP_ARGPACK_DECL_NORMALIZE_FCT(class_name)						\
		public:	class_name &normalize()	throw();
#define NEOIP_ARGPACK_DECL_NORMALIZE_STUB(class_name)						\
		public:	class_name &normalize()	throw()	{ return *this;		}
#define NEOIP_ARGPACK_DECL_CLASS_END(class_name)						\
	};
#define NEOIP_ARGPACK_DECL_ITEM(class_name, item_type, item_name)					\
	private:	item_type	item_name##_val;						\
			bool		item_name##_been_set;						\
	public:												\
		class_name &item_name(const item_type &val)	throw()					\
			{ item_name##_val = val; item_name##_been_set = true; return *this; }		\
		const item_type &item_name()	const throw()	{ DBG_ASSERT(item_name##_present());	\
								  return item_name##_val; }		\
		bool  item_name##_present()	const throw()	{ return item_name##_been_set;}

// NOTE: this _DECL_ITEM_PTR is duplicated because im a shit and dont know how to write a single define 
//       able to support pointer
#define NEOIP_ARGPACK_DECL_ITEM_PTR(class_name, item_type, item_name)					\
	private:	item_type	item_name##_val;						\
			bool		item_name##_been_set;						\
	public:												\
		class_name &item_name(item_type val)	throw()						\
			{ item_name##_val = val; item_name##_been_set = true; return *this; }		\
		item_type item_name()		const throw()	{ DBG_ASSERT(item_name##_present());	\
								return item_name##_val; }		\
		bool  item_name##_present()	const throw()	{ return item_name##_been_set;}


#define NEOIP_ARGPACK_DEF_CLASS_BEGIN(class_name)	class_name::class_name() throw() {
#define NEOIP_ARGPACK_DEF_ITEM(class_name, item_type, item_name)	\
		item_name##_been_set	= false;
#define NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(class_name, item_type, item_name, item_default)	\
		item_name##_val		= item_default;					\
		item_name##_been_set	= true;
#define NEOIP_ARGPACK_DEF_CLASS_END(class_name)		}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ARGPACK_HPP__  */





