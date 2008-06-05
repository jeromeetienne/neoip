#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv


require 'rubygems'
require 'mechanize'
require 'logger'
# the 'require' for the library i use
require 'impl/base/neoip_track_list_base_t'
require 'impl/youporn/neoip_track_item_youporn_t'

module	Neoip
module	Track_list_t
class	Youporn_t

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

	# iterate over the rss item to get the track_jspf
	nb_contiguous	= [3, video_arr.length].min;
	base_idx	= video_arr.length <= nb_contiguous ? 0 : rand(video_arr.length-nb_contiguous).to_i;
	(nb_contiguous-1).downto(0) do |i|
		page_video	= agent.click video_arr[base_idx+i];
		track_title	= page_video.title.scan(/.*?- (.*)/)[0][0]
		flv_uri		= page_video.links.with.href(%r{/flv/}).href

		# put this track in the track_list	
		track_item	= Neoip::Track_item_t::Youporn_t.new(
					{ 	"flv_uri"	=> flv_uri,
						"track_title"	=> track_title	});
		track_list.push( track_item );
	end
	
	# return the just built track_list
	return track_list
end

end	# end of class	Youporn_t
end	# end of module	Track_list_t
end	# end of module Neoip



