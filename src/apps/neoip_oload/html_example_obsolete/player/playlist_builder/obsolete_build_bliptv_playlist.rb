#!/usr/bin/ruby
#
# \par Brief Description
# this is only a test to see how to generate neoip.player_t .playlist.json
# from http://blip.tv
#
# \par TODO the download is full aka not only the metadata
# - TODO the curl|neoip-flv filter is not killed for whatever reason
#   when reading on blip.tv
# - i dont think neoip-flvfilter die when the stdout is closed...
# - i dont know how to test if neoip-flvfilter is behaving right now
# - i coded a lame attempt but doesnt seem to work for now
#
# \par TODOs 
# - TODO test specifically if neoip-flv2xml is available to run
#   currently it just silently ignore and return null duration :)
# - sometime the file is not avaiable and create an error
#   - be more robust

# the 'require' for the library i use
require 'rubygems'
require 'hpricot'
require 'open-uri'
require 'json'
require 'playlist_util'

# read the rss data from http://blip.tv
# - TODO get it from the cmdline parameters
rss_uri		= ARGV[0];

# read the rss data from the rss_uri
rss_doc		= Hpricot::XML(open(rss_uri))

tmp_items	= []

# go thru all the 'item' in the rss
rss_items	= rss_doc.search("rss > channel > item");
nb_item		= [10, tmp_items.length].max
for i in 0..nb_item-1
	rss_item	= rss_items[i];
	m4v_uri		= rss_item.at("/enclosure").attributes['url'];
	
	# extract the info from the rss and put it in tmp_items
	tmp_item		= {}
	tmp_item['flv_uri']	= m4v_uri.gsub(/.[^.]*$/,'.flv');
	tmp_item['title']	= rss_item.at("title").inner_html;
	# put this item into the tmp_items	
	tmp_items << tmp_item;
end

# start building the playlist
playlist = {}
playlist['date']	= Time.now.gmtime;
playlist['track']	= []

# go thru all the selected track to build the playlist
tmp_items.each { |tmp_item|
	# log to debug
	#puts "movie '#{tmp_item['title']}' at #{tmp_item['flv_uri']}";
	# TODO extract the duration from the neoip-flvfilter
	# build a playlist from this rss
	
	# get the flv_mdata for this flv_uri
	duration	= flv_uri_to_duration(tmp_item['flv_uri']);
	
	# build the playlist_track
	playlist_track	= {}
	playlist_track['title']		= tmp_item['title'];
	playlist_track['location']	= tmp_item['flv_uri'];
	playlist_track['duration']	= (duration * 1000).round;
	playlist_track['extension']		= {}
	playlist_track['extension']['oload']	= {}
	playlist_track['extension']['oload']['mod']	= "flv";
	
	# put this playlist_track in the playlist
	playlist['track'].push(playlist_track);
}

# display the json format for the playlist
print JSON.pretty_generate({"playlist" => playlist}) + "\n"





