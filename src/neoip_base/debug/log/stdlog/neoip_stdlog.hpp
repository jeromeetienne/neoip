/*! \file
    \brief Declaration of the \ref log_layer_t

*/


#ifndef __NEOIP_STDLOG_HPP__ 
#define __NEOIP_STDLOG_HPP__ 
/* system include */
#include <sstream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

class stdlog_t {
public:
	static void	do_cout(const std::string &str)				throw();
	static void	do_cerr(const std::string &str)				throw();
};

//! some kludgly define to avoid accessing std::cout and std::cerr directly
// - NOTE: there are no \n appended at the end like other KLOG
#define KLOG_STDOUT( klog_data )				\
	do {							\
		std::ostringstream	klog_stdout_oss;	\
		klog_stdout_oss << klog_data;			\
		stdlog_t::do_cout(klog_stdout_oss.str());	\
	}while(0)

#define KLOG_STDERR( klog_data )				\
	do {							\
		std::ostringstream	klog_stderr_oss;	\
		klog_stderr_oss << klog_data;			\
		stdlog_t::do_cerr(klog_stderr_oss.str());	\
	}while(0)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STDLOG_HPP__  */





