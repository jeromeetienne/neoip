#!/usr/bin/env ruby
#
# This small php script is a trick to 'echo' the bt_cast_mdata_t on a server
# - this allow to get a simple server running on a inetreach server
#   - neoip-casti may not be inet reach
# - thus neoip-casti can send their bt_cast_mdata_t here and the 
#   neoip-casto can retrieve them here too.
# - WARNING: absolutly no check or security is done
#   - this is prototype only
# - TEST in ruby
#   - require 'xmlrpc/client'
#   - server = XMLRPC::Client.new2("http:#localhost/~jerome/oload/cast_mdata_php/cast_mdata_echo_server.php")
#   - server.call("set_cast_mdata", "superstream.flv", { :slota => 'bip', :goup => 'slsdfadfsl' })
#   - server.call("get_cast_mdata", "superstream.flv")
#   - begin; server.call("get_cast_mdata", "superstream.flv"); rescue => e; puts "Received fault code #{e.faultCode} String #{e.faultString}"; end
# - all the data are stored in "/tmp/.neoip_cast_mdata_echo_ctx." 
#   - this is due to some file permissions stuff
#   - ugly but easy
# -----------------------------
# - how to measure latency
#   - Time.now - Time.at(server.call("get_cast_mdata", "superstream")['casti_date'])
#   - works if done with echo_server and casti clock are insync. like on the same box
#   - usefull to test the casti_date compensation
########################################
require 'logger'
require "xmlrpc/server"
require "xmlrpc/client"

def log_error(str)
	logger = Logger.new('/tmp/cast_mdata_echo_server_logs.log')
	logger.info "#{str}"
end

################################################################################
################################################################################
#			handle ctxfile
################################################################################
################################################################################

def ctxfile_name(cast_name)
	# stored in /tmp to get automatically cleanup on boot - just good policy
	return "/tmp/.neoip_cast_mdata_echo_ctx.#{cast_name}"
end

def ctxfile_write(cast_name, cast_ctx)
	file_name	= ctxfile_name(cast_name)
	File.open(file_name, 'w+'){|f| f.write(Marshal.dump(cast_ctx))}
end
def ctxfile_read(cast_name)
	file_name	= ctxfile_name(cast_name)
	file_data	= File.read(file_name);
	cast_ctx	= Marshal.load(file_data);
	return cast_ctx;
end
def ctxfile_exist?(cast_name)
	file_name	= ctxfile_name(cast_name)
	return File.exist?(file_name)
end
def ctxfile_delete(cast_name)
	file_name	= ctxfile_name(cast_name)
	File.delete(file_name)
end

def ctxfile_age(cast_name)
	file_name	= ctxfile_name(cast_name)
	return Time.now - File.mtime(file_name)
end

################################################################################
################################################################################
#
################################################################################
################################################################################


def set_cast_mdata_pull(cast_name, port_lview, port_pview, uri_pathquery)
	require	'ipaddr'
	remote_addr	= IPAddr.new( ENV['REMOTE_ADDR'] );

	# determine the $chosen_port depending on the remote_addr of rfc1918
	addr_is_public	= remote_addr.mask(8).to_s != "10.0.0.0" &&
				remote_addr.mask(12).to_s	!= "172.16.0.0"  &&
				remote_addr.mask(16).to_s	!= "192.168.0.0" &&
				remote_addr.mask(8).to_s	!= "127.0.0.0";
	port_chosen	= addr_is_public ? port_pview : port_lview;
	
	# build casti_srv_uri
	casti_srv_uri	= "http://#{remote_addr}:#{port_chosen}#{uri_pathquery}";
	
	log_error("cati_srv_uri=#{casti_srv_uri}");
	
	# build the cast_ctx
	cast_ctx	= {}
	cast_ctx['cast_name']	= cast_name;
	cast_ctx['ctx_type']	= "pull";
	cast_ctx['ctx_arg']	= casti_srv_uri;

	# write the cast_ctx in a file
	ctxfile_write(cast_name, cast_ctx);
	
	# return 0 - all the time
	return 0;
end

def set_cast_mdata_push(cast_name, cast_mdata)
	log_error "function=set_cast_mdata_push"
	log_error "cast_name=#{cast_name}."
	log_error "cast_mdata=#{cast_mdata.inspect}."
	
	# build the cast_ctx
	cast_ctx	= {}
	cast_ctx['cast_name']	= cast_name;
	cast_ctx['ctx_type']	= "push";
	cast_ctx['ctx_arg']	= cast_mdata;

	# write the cast_ctx in a file
	ctxfile_write(cast_name, cast_ctx);
	
	# return 0 - all the time
	return 0;
end


def get_cast_mdata(cast_name)
	# if the ctxfile_file doesnt exist, return an error
	if( !ctxfile_exist?(cast_name) )
		raise XMLRPC::FaultException.new(-1, "stream #{cast_name} doesnt exist.")
	end
	
	# get the age of this context
	ctx_age		= ctxfile_age(cast_name);
	# get the cast_ctx itself
	cast_ctx	= ctxfile_read(cast_name);
	
	# forward to the proper handler depending on cast_ctx['ctx_type']
	if( cast_ctx['ctx_type'] == "pull" )
		return get_cast_mdata_pull(cast_ctx, ctx_age);
	else
		return get_cast_mdata_push(cast_ctx, ctx_age);
	end
end

def get_cast_mdata_pull(cast_ctx, ctx_age)
	cast_name	= cast_ctx['cast_name'];

	# get casti_srv_uri from cast_ctx['ctx_arg']
	casti_srv_uri	= cast_ctx['ctx_arg'];

	# create the xmlrpc_client to forward the xmlrpc call to neoip-casti directly	
	xmlrpc_client = XMLRPC::Client.new2(casti_srv_uri);
	begin
		cast_mdata	= xmlrpc_client.call("get_cast_mdata", cast_name);
	rescue => e;
		# if the exception is a xmlrpc fault from neoip-casti, just forward it
		# - else return a 'custom' xmlrpc fault
		if( e.class != XMLRPC::FaultException )
			raise XMLRPC::FaultException.new(e.faultCode, e.faultString)
		else
			raise XMLRPC::FaultException.new(-1, "Error #{e.to_s}")
		end
	end

	# return the cast_mdata
	return cast_mdata;
end

def get_cast_mdata_push(cast_ctx, ctx_age)
	cast_name	= cast_ctx['cast_name'];
	# if ctx_age is too old, delete the ctxfile and return an error
	# - TODO make this delay tunable
	if( ctx_age > 60 )
		ctxfile_delete(cast_name);
		raise XMLRPC::FaultException.new(-1, "stream #{cast_name} is obsolete.")
	end
	
	# get cast_mdata from cast_ctx['ctx_arg']
	cast_mdata	= cast_ctx['ctx_arg'];
	
	# attempts to update the casti_date
	cast_mdata['casti_date'] += ctx_age;
	
	# return the cast_mdata
	return cast_mdata;
end

def del_cast_mdata(cast_name)
	# delete the ctxfile
	ctxfile_delete(cast_name);
	# return 0 - all the time
	return 0;
end

################################################################################
################################################################################
#			CGI handling for xmlrpc
################################################################################
################################################################################
# - for basic xmlrpc via CGI example
#   - see http:#www.ntecs.de/projects/xmlrpc4r/server.html#label-19

# init the cfg_server
cgi_server	= XMLRPC::CGIServer.new     
# register all the xmlrpc function
cgi_server.add_handler("set_cast_mdata_pull") do |cast_name, port_lview, port_pview, uri_pathquery|
	set_cast_mdata_pull(cast_name, port_lview, port_pview, uri_pathquery);
end 
cgi_server.add_handler("set_cast_mdata_push") do |cast_name, cast_mdata|
	set_cast_mdata_push(cast_name, cast_mdata);
end 
cgi_server.add_handler("get_cast_mdata") do |cast_name|
	get_cast_mdata(cast_name);
end 
cgi_server.add_handler("del_cast_mdata") do |cast_name|
	del_cast_mdata(cast_name);
end 

cgi_server.add_handler("michael.add") do |a,b|
	a + b
end

cgi_server.add_handler("michael.div") do |a,b|
	if b == 0
		raise XMLRPC::FaultException.new(1, "division by zero")
	else
		a / b 
	end
end 

# handle the unknown/bad formered calls
cgi_server.set_default_handler do |name, *args|
	log_error("arg=#{args.inspect}")
	raise XMLRPC::FaultException.new(-99, "Method #{name} missing" +
                                   " or wrong number of parameters!")
end

# server the cgi_server
cgi_server.serve