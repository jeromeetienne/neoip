#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'impl/base/neoip_track_list_base_t'
require 'impl/direct/neoip_track_item_direct_t'

module	Neoip
module	Track_list_t
class	Direct_t

# include the base of Neoip::Track_list_t
include Neoip::Track_list_t::Base_t

################################################################################
################################################################################
#			build function
################################################################################
################################################################################
def self.build(space_desc)
	# build the track_list
	track_list	= [];

	# build a Track_item_t for each item in the space_desc array
	space_desc.each {|item|
		track_title	= item['title']
		flv_uri		= item['uri']
		# put this track in the track_list	
		track_item	= Neoip::Track_item_t::Direct_t.new(
					{ 	"flv_uri"	=> flv_uri,
						"track_title"	=> track_title	});
		track_list.push( track_item );
	}	
	# return the just built track_list
	return track_list
end

end	# end of class	Direct_t
end	# end of module	Track_list_t
end	# end of module Neoip



