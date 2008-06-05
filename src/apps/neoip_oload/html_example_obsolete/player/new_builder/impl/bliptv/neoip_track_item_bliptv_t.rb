#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'rubygems'
require 'hpricot'
require 'impl/base/neoip_track_item_base_t'


module	Neoip
module	Track_item_t
class	Bliptv_t

# include the base of Neoip::Track_item_t
include Neoip::Track_item_t::Base_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(p_rss_item)
	@rss_item	= p_rss_item
end

################################################################################
################################################################################
#			Query function
################################################################################
################################################################################

# Return the title of this Track_item_t
# - return nil, if cant be found 
def track_title
	# get the title from the @rss_item
	track_title	= @rss_item.at("title").inner_html;
	# return the found track_title
	return track_title
end

# Return the flv_uri of this Track_item_t 
# - return nil, if cant be found 
def flv_uri
	# NOTE: there is a bug/feature in hpricot which make it impossible 
	#       to get media:content
	# - this seems due to the ':'
	# - so i go thru the whole item to get the proper info
	# - other way to workaround - by deriving the .flv name from the mp4 one
	#   - often works... but not always :)
	#   - m4v_uri	= rss_item.at("/enclosure").attributes['url'];
	#   - flv_uri	= m4v_uri.gsub(/.[^.]*$/,'.flv');
	flv_uri		= nil;
	@rss_item.traverse_all_element { |e|
		if( e.class == Hpricot::Elem &&	e.name == "media:content" &&
				e.attributes['type']=="video/x-flv") then
			flv_uri	= e.attributes['url'];
		end
	}
	
	# if failed to retrieved the flv_uri, log the event and goto the next
	if( flv_uri == nil )
		$stderr.puts "unable to find flv_uri for track title #{track_title}";
		return nil
	end
	
	# return the found flv_uri	
	return flv_uri
end


end	# end of class	Bliptv
end	# end of module Track_item_t
end	# end of module Neoip



