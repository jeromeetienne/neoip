/*! \file
    \brief Header of the \ref acl_t

- TODO to rename T_OP_NAME and T_OP_PAT in T_NAME and T_PAT
*/


#ifndef __NEOIP_ACL_HPP__ 
#define __NEOIP_ACL_HPP__ 
/* system include */
#include <iostream>
#include <list>
/* local include */
#include "neoip_acl_action.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
template <typename T_OP_NAME, typename T_OP_PAT> class acl_t;
template <typename T_OP_NAME, typename T_OP_PAT> 
		std::ostream& operator << ( std::ostream& os, const acl_t<T_OP_NAME, T_OP_PAT> &acl )
										throw();
template <typename T_OP_NAME, typename T_OP_PAT> 
		serial_t& operator << ( serial_t& serial, const acl_t<T_OP_NAME, T_OP_PAT> &acl )
										throw();
template <typename T_OP_NAME, typename T_OP_PAT> 
		serial_t& operator >> ( serial_t& serial, acl_t<T_OP_NAME, T_OP_PAT> &acl )
										throw(serial_except_t);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    Class Declaration
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief A single rule for a \ref acl_t
 * 
 * - a rule is a couple \ref acl_action_t and a operation pattern
 * - it is up to the instanciator of the template to define the type of the operation pattern
 */
template <typename T_OP_PAT > class acl_rule_t {
public:
	acl_action_t		action;
	T_OP_PAT		op_pattern;
	acl_rule_t( const acl_action_t &action, T_OP_PAT op_pattern )	throw()
			: action(action), op_pattern( op_pattern ) {}
};

/** \brief class definition for \ref acl_t
 * 
 * - a acl is a ordered list of \ref acl_rule_t
 * - it allows to determine the proper action to take when a given operation is attempted
 * - T_OP_NAME define the type of a given operation
 * - T_OP_PAT  define the type of a pattern of actions
 * - the definition of operation depends on the template intanciator
 *   - example of scenario: a acl used to limit who is allowed to connect
 *   - a T_OP_NAME is a name of a given peer say john.smith.org
 *   - a T_OP_PAT is a pattern of name e.g. *.smith.org
 */
template <typename T_OP_NAME, typename T_OP_PAT > class acl_t {
private:
	std::list<acl_rule_t<T_OP_PAT> >	rule_list;
public:
	acl_t<T_OP_NAME, T_OP_PAT>&	add( const acl_action_t &action, const T_OP_PAT &pattern )
										throw();
	acl_action_t			find( const T_OP_NAME &op_name )	const throw();
	
friend std::ostream& operator << <> ( std::ostream& os, const acl_t<T_OP_NAME, T_OP_PAT> &acl )	throw();	
friend serial_t& operator << <> ( serial_t& serial, const acl_t<T_OP_NAME, T_OP_PAT> &acl )	throw();
friend serial_t& operator >> <> (serial_t& serial, acl_t<T_OP_NAME, T_OP_PAT> &acl)throw(serial_except_t);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    add/find rules
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Add an rule to the \ref acl_t
 */
template <typename T_OP_NAME, typename T_OP_PAT>
	acl_t<T_OP_NAME, T_OP_PAT>& acl_t<T_OP_NAME, T_OP_PAT>::add( const acl_action_t &action, const T_OP_PAT &pattern)
					throw()
{
	acl_rule_t<T_OP_PAT>	rule(action, pattern);
	rule_list.push_back( rule );
	return *this;	
}

/** \brief find the action for a given operation name
 * 
 * - if no operation pattern matches, it return acl_action_t::NONE
 */
template <typename T_OP_NAME, typename T_OP_PAT> acl_action_t acl_t<T_OP_NAME,T_OP_PAT>::find( const T_OP_NAME &op_name ) const throw()
{
	typename std::list<acl_rule_t<T_OP_PAT> >::const_iterator	iter;
	// scan the whole list to find a matching rule
	for( iter = rule_list.begin(); iter != rule_list.end(); iter++ ){
		const acl_rule_t<T_OP_PAT> &	rule = *iter;
		// if the options match, return true
		if( rule.op_pattern.pattern_match(op_name) )
			return rule.action;
	}
	return acl_action_t::NONE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    OSTREAM redirection (mainly for debug)
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection (mainly for debug)
 */
template <typename T_OP_NAME, typename T_OP_PAT>
	std::ostream& operator << ( std::ostream& os, const acl_t<T_OP_NAME, T_OP_PAT> &acl )
										throw()
{
	const std::list<acl_rule_t<T_OP_PAT> > &	rule_list	= acl.rule_list;	
	typename std::list<acl_rule_t<T_OP_PAT> >::const_iterator	iter;
	// store all the rules
	for( iter = rule_list.begin(); iter != rule_list.end(); iter++ ){
		const acl_rule_t<T_OP_PAT> &	rule = *iter;
		os << "[";
		os << rule.op_pattern;
		os << " -> ";
		os << rule.action;
		os << "]";
	}
	return os;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          SERIALIZATION
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a \ref acl_t
 */
template <typename T_OP_NAME, typename T_OP_PAT>
serial_t& operator << (serial_t& serial, const acl_t<T_OP_NAME, T_OP_PAT> &acl)	throw()
{
	const std::list<acl_rule_t<T_OP_PAT> > &	rule_list	= acl.rule_list;	
	uint32_t					nb_rule		= rule_list.size();
	typename std::list<acl_rule_t<T_OP_PAT> >::const_iterator	iter;
	// store the number of rules
	serial << nb_rule;
	// store all the rules
	for( iter = rule_list.begin(); iter != rule_list.end(); iter++ ){
		const acl_rule_t<T_OP_PAT> &	rule = *iter;
		serial << rule.action;
		serial << rule.op_pattern;
	}
	return serial;
}

/** \brief unserialize a \ref acl_t
 */
template <typename T_OP_NAME, typename T_OP_PAT>
serial_t& operator >> ( serial_t& serial, acl_t<T_OP_NAME, T_OP_PAT> &acl )	throw(serial_except_t)
{
	uint32_t	nb_rule;
	// zero the list
	acl.rule_list = std::list<acl_rule_t<T_OP_PAT> >();
	// retrieve the number of options
	serial >> nb_rule;
	// retrieve all the options
	for( uint32_t i = 0; i < nb_rule; i++ ){
		acl_action_t	action;
		T_OP_PAT	pattern;
		serial >> action;
		serial >> pattern;
		acl.add( action, pattern );
	}
	return serial;
}
NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ACL_HPP__  */










