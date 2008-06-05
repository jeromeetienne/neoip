#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on http://youporn.com
#
# \par Possible improvement - algo to produce a channel
# 1. get the number of page
# 2. get a random page
# 3. get a random video in this page
# 4. get this video
# 5. get a video related to it
# 5. loop to 4 until the loop is filled

# the 'require' for the library i use
require 'rubygems'
require 'mechanize'
require 'logger'
require 'neoip_flv_uri_t'


module	Neoip
module	Youporn
class	Track_pool_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(policy_name)
	# copy the parameter
	@policy_name	= policy_name;

	@track_jspf_arr	= [];


end

################################################################################
################################################################################
#			misc
################################################################################
################################################################################

def video_arr_by_policy()
	# if @policy_name is "youporn_at_random" return the featured_videos
	if( @policy_name == "youporn_at_random" )
		# init the WWW::mechanize agent
		agent		= WWW::Mechanize.new
		agent.user_agent_alias = 'Mac Safari'
		# get passed the 'age confirmation' page to reach the initial page
		page_ageconfirm	= agent.get('http://youporn.com/')
		form		= page_ageconfirm.forms[0]
		page_init	= agent.submit(form, form.buttons.first)
		
		# get the number of page from the initial page
		page_idxmax	= page_init.links.with.href(%r{\?page=}).collect{ |link| 
					link.href.split("=")[1]
				}.max
		
		# pick a page at random		
		page_idx	= rand(page_idxmax);
		page_front	= agent.get("/?page=#{page_idx}")

		# all the links toward the videos watch page
		video_arr	= page_front.links.with.href(%r{watch}).with.text("")
		# return the video_arr for this page
		return video_arr, agent;
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
	video_arr, agent= video_arr_by_policy();

	track_param_arr	= [];

	# iterate over the rss item to get the track_jspf
	nb_contiguous	= [3, video_arr.length].min;
	base_idx	= video_arr.length <= nb_contiguous ? 0 : rand(video_arr.length-nb_contiguous).to_i;
	(nb_contiguous-1).downto(0) do |i|
		page_video	= agent.click video_arr[base_idx+i];
		track_title	= page_video.title.scan(/.*?- (.*)/)[0][0]
		flv_uri		= page_video.links.with.href(%r{/flv/}).href

		# put this track in the track_param_arr	
		track_param_arr	<< { 	"flv_uri"	=> flv_uri,
					"track_title"	=> track_title	};
	end


	# process the track after the info has been retrieved between mechanize timeout...
	# - TODO there is likely a way to avoid it to timeout like that
	track_param_arr.each { |track_param|
		flv_uri		= track_param['flv_uri'];
		track_title	= track_param['track_title'];
				
		# build the track_jspf
		track_jspf	= Neoip::Flv_uri_t.to_track_jspf(flv_uri, track_title);
		if( track_jspf == nil )
			$stderr.puts "unable to produce track_jspf for track title #{track_title}";
			next
		end

		# put this track at the end of the @track_jspf_arr
		@track_jspf_arr.push(track_jspf);
	}
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
end	# end of module Youporn
end	# end of module Neoip



