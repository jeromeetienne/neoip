#!/usr/bin/env ruby
#
# \par Brief Description
# this script generates the playlist_arr (aka the list of playlist_jspf) 

require '../playlist_builder/neoip_playlist_t'
require 'rubygems'
require 'json'

# ruby author has a stance not to support assert... working around
def assert( cond )	raise "Assertion failed !" unless cond	end


plistgen_info_dirpath	= "./plistgen_info"
dynamic_playlist_dirpath= "./playlist.jspf/dynamic";
static_playlist_dirpath	= "./playlist.jspf/static";

playlist_fname_arr	= []
playlist_fname_arr	+= Dir.entries(dynamic_playlist_dirpath).
				collect { |x| dynamic_playlist_dirpath + "/#{x}"}
playlist_fname_arr	+= Dir.entries(static_playlist_dirpath).
				collect { |x| static_playlist_dirpath + "/#{x}"	}

playlist_fname_arr	= playlist_fname_arr.delete_if{ |x| !x.match(/\.playlist.jspf$/) }

playlist_arr	= []
playlist_fname_arr.each { |filename|
	playlist	= Neoip::Playlist_t.from_file(filename);
	playlist_title	= playlist.jspf['title'];
	playlist_id	= playlist.jspf['identifier'];

	puts "filename=#{filename} playlist_title=#{playlist_title} playlist_id=#{playlist_id}"	

	# build the playlist_uri
	playlist_uri	= "../../cgi-bin/get_playlist.rb?playlist_id=#{playlist_id}";

	# add this item in the playlist_arr
	playlist_arr.push({	"playlist_title"=> playlist_title,
				"playlist_uri"	=> playlist_uri
			})
}

# write the playlist_arr into a file using JSON format
json_data	= JSON.pretty_generate(playlist_arr)
File.open("playlist.jspf/ezplayer_playlist_arr.json", 'w+'){|f| f.write(json_data) }

