#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'rubygems'
require 'youtube'
require 'neoip_flv_uri_t'


module	Neoip
module	Youtube
class	Track_pool_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(policy_name)
	# copy the parameter
	@policy_name	= policy_name;
	
	# start a client with my dev_id (NOTE: this is personal stuff)
	@youtube 	= ::YouTube::Client.new 'bwo8aCyQwSE'

	# init the class fields
	@track_jspf_arr	= [];
end

################################################################################
################################################################################
#			misc
################################################################################
################################################################################

def video_arr_by_policy()
	# if @policy_name is "youtube_featured_at_random" return the featured_videos
	if( @policy_name == "youtube_featured_at_random" )
		return @youtube.featured_videos;
	end
	
	# if @policy_name is "youtube_tag_at_random", pick a random tag, and get the video for it
	if( @policy_name == "youtube_tag_at_random" )
		video_arr	= @youtube.featured_videos;
		chosen_video	= video_arr[rand(video_arr.length)];
		tag_arr		= chosen_video.tags.split(' ');
		chosen_tag	= tag_arr[rand(tag_arr.length)];
		return	@youtube.videos_by_tag(chosen_tag);
	end
	
	# NOTE: this point MUST NEVER be reached
	raise "#{policy_name} is an unknown policy_name";
end

################################################################################
################################################################################
#			populate pool
################################################################################
################################################################################

# feed the pool
def populate_pool()
	# get the featured video
	video_arr	= video_arr_by_policy();

	# iterate over the rss item to get the track_jspf
	nb_contiguous	= [3, video_arr.length].min;
	base_idx	= video_arr.length <= nb_contiguous ? 0 : rand(video_arr.length-nb_contiguous);
	(nb_contiguous-1).downto(0) do |i|
		video		= video_arr[base_idx+i];
		track_title	= video.title
		
		# log to debug
		puts "starting on #{track_title}"

		# get the flv_uri
		flv_uri		= Neoip::Flv_uri_t::from_youtube_videoid(video.id);
				
		# build the track_jspf
		track_jspf	= Neoip::Flv_uri_t.to_track_jspf(flv_uri, track_title);
		if( track_jspf == nil )
			$stderr.puts "unable to produce track_jspf for track title #{track_title}";
			next
		end

		# add some specific to youtube
		oload	= track_jspf['extension']['oload'];
		oload['neoip_metavar_http_mod_type']		= "flv";
		oload['neoip_metavar_http_mod_flv_varname']	= "start";
		track_jspf['meta']['location_rangereq_byuri_varname']	= 'start';

		# put this track at the end of the @track_jspf_arr
		@track_jspf_arr.push(track_jspf);
	end
end

################################################################################
################################################################################
#			pop_some
################################################################################
################################################################################

# pop some track from the pool
def pop_some
	# if the pool is currently empty, populate it
	if( @track_jspf_arr.empty? )
		populate_pool();
	end
	
	# set the result to @track_jspf_arr
	result	= @track_jspf_arr

	# empty @track_jspf_arr
	@track_jspf_arr	= [];

	# return the result
	return result;
end

end	# end of class	Track_pool_t
end	# end of module Youtube
end	# end of module Neoip



