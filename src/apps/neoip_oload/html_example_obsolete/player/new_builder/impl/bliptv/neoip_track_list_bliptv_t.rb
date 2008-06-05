#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'impl/base/neoip_track_list_base_t'
require 'impl/bliptv/bliptv_t'
require 'impl/bliptv/neoip_track_item_bliptv_t'


module	Neoip
module	Track_list_t
class	Bliptv_t

# include the base of Neoip::Track_list_t
include Neoip::Track_list_t::Base_t

################################################################################
################################################################################
#			build function
################################################################################
################################################################################
def self.build(query_arg)
	rss_data	= ::Bliptv::query_data(query_arg);
	# build an xml document from rss_data
	rss_doc		= Hpricot::XML(rss_data)
	# go thru all the 'item' in the rss
	rss_items	= rss_doc.search("rss > channel > item");
	# build the track_list
	# - this is an array containing a Neoip::Track_item_t::Bliptv for each rss_item
	track_list	= [];
	rss_items.each { |rss_item|
		track_item	= Neoip::Track_item_t::Bliptv_t.new(rss_item);
		track_list.push( track_item );
	}
	# return the just built track_list
	return track_list
end

end	# end of class	BlipTV
end	# end of module Track_list_t
end	# end of module Neoip



