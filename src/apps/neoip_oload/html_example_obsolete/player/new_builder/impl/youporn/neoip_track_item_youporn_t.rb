#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'impl/base/neoip_track_item_base_t'

module	Neoip
module	Track_item_t
class	Youporn_t


# include the base of Neoip::Track_item_t
include Neoip::Track_item_t::Base_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(p_item_data)
	@item_data	= p_item_data
end

################################################################################
################################################################################
#			Query function
################################################################################
################################################################################

# Return the title of this Track_item_t
# - return nil, if cant be found 
def track_title
	# get the title from the @item_item
	track_title	= @item_data['track_title']
	# return the found track_title
	return track_title
end

# Return the flv_uri of this Track_item_t 
# - return nil, if cant be found 
def flv_uri
	# get the flv_uri
	flv_uri		= @item_data['flv_uri']
	
	# if failed to retrieved the flv_uri, log the event and goto the next
	if( flv_uri == nil )
		$stderr.puts "unable to find flv_uri for track title #{title}";
		return nil
	end
	
	# return the found flv_uri	
	return flv_uri
end

end	# end of class	Youporn_t
end	# end of module Track_item_t
end	# end of module Neoip



