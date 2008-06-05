#!/usr/bin/ruby
#
# \par Brief Description
# Class to detect neoip-apps present on the localhost

# the 'require' for the library i use
require 'xmlrpc/client'

module	Neoip
class	Apps_detect_t

# probe for the application with suffic_name to the current_port on localhost
# - it return the version of the application if it is present
# - if not present, it return nil 
def self.probe_one(suffix_name, current_port)
	# build the uri of the xmlrpc server to probe
	uri	= "http://localhost:#{current_port}/neoip_#{suffix_name}_appdetect_xmlrpc.cgi"
	# define the xmlrpc server object
	server	= XMLRPC::Client.new2(uri)
	# try to probe the version of this application 
	begin
		version	= server.call("probe_apps")
	rescue => e
		version	= nil
	end
	# return the result
	return version
end

################################################################################
################################################################################
#			
################################################################################
################################################################################
def self.probe(suffix_name, first_port, last_port)
	# go thru every port
	(first_port..last_port).each { |current_port|
		version	= probe_one(suffix_name, current_port)
		if( version != nil )
			return {	"port"		=> current_port,
					"version"	=> version	}
		end
	}
	return nil
end


end	# end of class Apps_detect_t
end	# end of module Neoip



