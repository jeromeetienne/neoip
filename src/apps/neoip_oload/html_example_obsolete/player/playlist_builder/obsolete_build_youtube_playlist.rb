#!/usr/bin/ruby
#
# \par Brief Description
# this is only a test to see how to generate neoip.player_t .playlist.json
# from youtube using the official API
#
 
require 'rubygems' 
require 'youtube'
require 'json'
require 'neoip_flv_uri_t'

# start a client with my dev_id (NOTE: this is personal stuff)
youtube = YouTube::Client.new 'bwo8aCyQwSE'
 
################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

# get the featured video
video_pool	= youtube.featured_videos

video_arr	= Array.new;

#STDERR.puts	"pool size is #{video_pool.length}"

for i in 0..10
	# pick a video at random
	curr_video	= video_pool[rand(video_pool.length)];

	# put this curr_video into the array
	video_arr.push( curr_video );

	# TODO produce the json output
	#STDERR.print "curr_video url is " + curr_video.url + "\n"

	# compute the next tag
	tag_arr		= curr_video.tags.split(' ');
	next_tag	= tag_arr[rand(tag_arr.length)];
	#STDERR.puts "next_tag=" + next_tag;

	# reinit the pool
	# disabled for now - to get good bw for sure
	# - apparently rare video got MUCH lower bw as in connect very slowly 
	video_pool	= youtube.videos_by_tag(next_tag);
end

# start building the playlist
playlist = {}
playlist['date']	= Time.now.gmtime;
playlist['track']	= []

# for each item of the youtubeid_arr
video_arr.each_with_index{ |video, index|
	# get the url for the flv file of this youtubeid video
#	flv_uri		= "http://cache.googlevideo.com/get_video?video_id=#{video.id}"
	flv_uri		= Neoip::Flv_uri_t::from_youtube_videoid(video.id);

	# get the duration of this flv
	# NOTE: the duration from the rss is at the integer second, may produce issue
	# - so the end of the movie will be truncated
	#duration	= video.length_seconds;
	duration	= Neoip::Flv_uri_t.to_duration(flv_uri);

	# log to debug
	STDERR.puts "Item #{index} with flv_url #{flv_uri} and duration #{duration}-sec"

	# build the playlist_track
	playlist_track	= {}
	playlist_track['title']		= video.title;
	playlist_track['location']	= flv_uri + "&neoip_http_metavar_mod_type=flv" +
						"&neoip_http_metavar_mod_flv_varname=start";
	playlist_track['duration']	= video.length_seconds * 1000;
	playlist_track['extension']		= {}
	playlist_track['extension']['oload']	= {}
	playlist_track['extension']['oload']['mod']		= "flv";
	# NOTE: httpo_maxrate is hardcoded at 40k because youtube uses only this
	playlist_track['extension']['oload']['httpo_maxrate']	= "50k";

	# put this playlist_track in the playlist
	playlist['track'].push(playlist_track);
}

# display the json format for the playlist
print JSON.pretty_generate({"playlist" => playlist}) + "\n"
