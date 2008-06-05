#!/usr/bin/ruby
#
# \par Brief Description
# to handle a jspf playlist

require 'rubygems'
require 'json'

module Neoip
class Playlist_t

attr_accessor	:jspf	# object which will hold all the data

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(playlist_id = nil, playlist_title = nil)
	# copy the parameter
	@jspf	= {};
	
	# if playlist_id is provided... then put a defualt one
	# - TODO to remove.. this is shit... put that in its own function
	# - TODO those value are biased to a particular use
	if( playlist_id )
		@jspf['date']		= Time.now.gmtime.to_s;
		@jspf['title']		= playlist_title ? playlist_title : playlist_id;
		@jspf['identifier']	= playlist_id;
		@jspf['meta']		= {}
		@jspf['meta']['trackidx_beg']	= 0;
		@jspf['meta']['instance_nonce']	= self.class.new_instance_nonce();
		@jspf['meta']['may_loop']	= false;
		@jspf['meta']['reload_delay']	= 10*1000;	# TODO this is a bit short...
		@jspf['track']		= []
	end
end

################################################################################
################################################################################
#			Utility function
################################################################################
################################################################################

# return a new playlist.meta.instance_nonce 
def self.new_instance_nonce()
	chars	= ("a".."z").to_a + ("A".."Z").to_a + ("0".."9").to_a
	nonce	= ""
	1.upto(10) { |i| nonce << chars[rand(chars.size-1)] }
	return nonce
end

# return the total_duration of a playlist
def total_duration()
	total_duration	= 0;
	# go thru each track
	@jspf['track'].each do |track|
		total_duration += track['duration'];
	end
	# return the total_duration
	return	total_duration;
end

# remove all tracks which are completed before the data_threshold
def remove_track_before(date_threshold)
	playlist_date	= Time.parse(@jspf['date']);

	time_offset	= 0;
	tmp_arr		= [];
	# go thru each track of the playlist_jspf
	@jspf['track'].each { |track|
		# update time_offset
		time_offset	+= track['duration'];
		# if this track is completed before date_threshold, goto the next
		if( playlist_date + time_offset/1000 > date_threshold )
			# put this track into the tmp_arr
			tmp_arr.push(track);
		else
			@jspf['meta']['trackidx_beg']	+= 1;
			@jspf['date']	= (playlist_date + track['duration']/1000).to_s;
		end
	}
	# copy the tmp_arr into the playlist_jspf
	@jspf['track']	= tmp_arr;
end

################################################################################
################################################################################
#			from_file/to_file
################################################################################
################################################################################

# create a playlist from a file (supposed to be in jspf - json version of xspf)
def self.from_file(filename)
	# read the file
	file_data	= File.read(filename);
	# parse it as json
	json_data	= JSON.parse(file_data);
	# build a playlist with the json_data
	playlist	= Neoip::Playlist_t.new();
	playlist.jspf	= json_data['playlist'];
	# return the just built playlist
	return playlist;
end

# save the current Playlist_t to a filename
def to_file(filename)
	data2write	= JSON.pretty_generate({"playlist" => @jspf});
	File.open(filename, 'w+'){|f| f.write(data2write)}
end

end	# end of class	Playlist_t
end	# end of module	Neoip



