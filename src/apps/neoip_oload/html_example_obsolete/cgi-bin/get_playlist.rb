#!/usr/bin/env ruby
#
# \par Brief Description
# this cgi script provides the playlist jspf 
# - this is a cgi in order to monitor when a given playlist is read or not
#   - e.g. this allow not to generate playlist which arent read by anybody

require 'cgi'
require 'time'

cgi		= CGI.new
# get the variable of the uri
uri_var		= cgi.params
playlist_id	= uri_var['playlist_id']

dirpath		= "../player/new_builder/playlist.jspf"
basename	= "#{playlist_id}.playlist.jspf"

# determine the fullpath of the playlist_jspf
# - TODO the crappyness about testing dynamic/static directory should be sortout
fullpath	= "#{dirpath}/dynamic/#{basename}"
if not File.exist?( "#{dirpath}/dynamic/#{basename}" )
	fullpath	= "#{dirpath}/static/#{basename}"
end

#print "curdir=#{Dir.getwd}"
#print "Hello, world title=#{uri_var['playlist_id']}\n"

begin
	# write nothing into a file - just to keep the date of the lastread
	readctx_fname	= "/tmp/.neoip_jsplayer_playlist.#{playlist_id}.lastread"
	File.open(readctx_fname, 'w+'){ |f| f.write('')	}
	
	# try to read the data
	playlist_str	= File.read(fullpath)

	# return the playlist_str with a "application/json" content-type
	cgi.out( "type"		=> "application/json" )	{ playlist_str	}
rescue => e
	cgi.out( "status"	=> "NOT_FOUND")		{ "due to #{e}"	}
end

