#!/usr/bin/ruby
#
# \par Brief Description
# Neoip::Track_item_t::Base_t handle all the functions common to all Neoip::Track_item_t

# the 'require' for the library i use

module	Neoip
module	Track_item_t
module	Base_t

################################################################################
################################################################################
#
################################################################################
################################################################################

def get_field(field_name)
	case field_name
	when "track_title"
		return track_title
	when "flv_uri"
		return flv_uri
	end
end

################################################################################
################################################################################
#			Convertion to_jspf function
################################################################################
################################################################################

# convert this item into a track_jspf format
def to_jspf
	# build a base track_jspf from the flv_uri and the track_title
	track_jspf	= Neoip::Flv_uri_t.to_track_jspf(flv_uri, track_title);
	# if an error occured, return nil now
	if( track_jspf == nil )
		return nil;
	end
		
	# return the just-build track_jspf
	return track_jspf
end


end	# end of module	Base_t
end	# end of module Track_item_t
end	# end of module Neoip



