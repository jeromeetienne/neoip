#!/usr/bin/ruby
#
# \par Brief Description
# to handle a jspf playlist

require 'neoip_playlist_t'

module Neoip
class Playlist_updater_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(playlist_id, track_pool)
	# copy the parameter
	@playlist_id	= playlist_id;
	@track_pool	= track_pool;
	
	# build the filename 
	@filename		= "../playlist.jspf/#{playlist_id}.playlist.jspf";
end

################################################################################
################################################################################
#
################################################################################
################################################################################

def update()
	playlist	= nil;
	# read the data in the file
	begin
		playlist	= Neoip::Playlist_t.from_file(@filename);
	rescue => e
		# log the event
		$stderr.puts "unable to read playlist due to #{e}";
		# create initial playlist
		playlist	= Neoip::Playlist_t.new(@playlist_id)
	end

	# remove obsolete track
	playlist.remove_track_before(Time.now.gmtime);
	
	# if there are no more track at this point, act as if no file were found
	if( playlist.jspf['track'].empty? )
		playlist	= Neoip::Playlist_t.new(@playlist_id);
	end

	# loop and adds track from the pool
	while true
		# if the playlist end in more than 300-sec, stop looping
		date_end = Time.parse(playlist.jspf['date']) + playlist.total_duration()/1000;
		if( date_end >= Time.now + 300 )
			break;
		end

		# get some track from the track_pool
		track_arr_jspf	= @track_pool.pop_some();
	
		# log to debug
		track_arr_jspf.each { |track_jspf| $stderr.puts "Adding #{track_jspf['title']}"	}
	
		# put those tracks into the playlist_jspf
		playlist.jspf['track'].concat(track_arr_jspf);
	end

	puts "total_duration=" + playlist.total_duration.to_s
	#$stderr.puts JSON.pretty_generate(playlist.jspf);
	
	# write the data in the file
	begin
		playlist.to_file(@filename);
	rescue => e
		# log the event
		$stderr.puts "unable to write playlist file due to #{e}";
		# TODO throw an exception
	end
end

end	# end of class	Playlist_updater_t
end	# end of module	Neoip

