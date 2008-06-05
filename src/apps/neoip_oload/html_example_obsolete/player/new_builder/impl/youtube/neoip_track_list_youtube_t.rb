#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv


# the 'require' for the library i use
require 'rubygems'
require 'youtube'
require 'impl/base/neoip_track_list_base_t'
require 'impl/youtube/neoip_track_item_youtube_t'

module	Neoip
module	Track_list_t
class	Youtube_t

# include the base of Neoip::Track_list_t
include Neoip::Track_list_t::Base_t

################################################################################
################################################################################
#			build function
################################################################################
################################################################################
def self.build(query_arg)
	# build the track_list
	track_list	= [];

	# start a client with my dev_id (NOTE: this is personal stuff)
	youtube 	= ::YouTube::Client.new 'bwo8aCyQwSE'

	# TODO make this depend on query_arg
	
	# alias on the query_arg
	cmd	= query_arg['cmd'];
	arg	= query_arg['arg'];
	
	# process each cmd
	case cmd
	when 'featured_videos' 
		video_arr	= youtube.featured_videos;
	when 'favorite_videos'
		video_arr	= youtube.favorite_videos(arg['user']);
	when 'videos_by_category_and_tag'
		video_arr	= youtube.videos_by_category_and_tag(arg['category'], arg['tag']);
	when 'videos_by_category_id_and_tag'
		category_id	= arg['category_id'];
		if( category_id.class == String )
			category_id	= eval("YouTube::Category::#{category_id.upcase}")
		end
		video_arr	= youtube.videos_by_category_id_and_tag(category_id, arg['tag']);
	when 'videos_by_playlist'
		video_arr	= youtube.videos_by_playlist(arg['id']);
	when 'videos_by_related'
		video_arr	= youtube.videos_by_related(arg['tag']);
	when 'videos_by_tag'
		video_arr	= youtube.videos_by_tag(arg['tag']);
	when 'videos_by_user'
		video_arr	= youtube.videos_by_user(arg['user']);
	end
		
	# the tag_at_random policy
	# - TODO would be nice if this could be done at the list select level
	#video_arr	= youtube.featured_videos;
	#chosen_video	= video_arr[rand(video_arr.length)];
	#tag_arr	= chosen_video.tags.split(' ');
	#chosen_tag	= tag_arr[rand(tag_arr.length)];
	#video_arr	= youtube.videos_by_tag(chosen_tag);
	
	# build the track_list
	# - this is an array containing a Neoip::Track_item_t::Bliptv for each rss_item
	track_list	= [];
	video_arr.each { |video_item|
		track_item	= Neoip::Track_item_t::Youtube_t.new(video_item);
		track_list.push( track_item );
	}

	# return the just built track_list
	return track_list
end

end	# end of class	BlipTV
end	# end of module Track_list_t
end	# end of module Neoip



