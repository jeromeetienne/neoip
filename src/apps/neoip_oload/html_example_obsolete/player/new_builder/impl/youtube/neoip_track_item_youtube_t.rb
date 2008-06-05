#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'impl/base/neoip_track_item_base_t'

module	Neoip
module	Track_item_t
class	Youtube_t

# include the base of Neoip::Track_item_t
include Neoip::Track_item_t::Base_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(p_video_item)
	@video_item	= p_video_item
end

################################################################################
################################################################################
#			Query function
################################################################################
################################################################################

# Return the title of this Track_item_t
# - return nil, if cant be found 
def track_title
	# get the title from the @video_item
	track_title	= @video_item.title
	# return the found track_title
	return track_title
end

# Return the flv_uri of this Track_item_t 
# - return nil, if cant be found 
def flv_uri
	# get the flv_uri
	flv_uri		= Neoip::Flv_uri_t::from_youtube_videoid(@video_item.id);
	
	# if failed to retrieved the flv_uri, log the event and goto the next
	if( flv_uri == nil )
		$stderr.puts "unable to find flv_uri for track title #{title}";
		return nil
	end
	
	# return the found flv_uri	
	return flv_uri
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
	
	# add some specific to youtube
	oload	= track_jspf['extension']['oload'];
	oload['neoip_metavar_http_mod_type']		= "flv";
	oload['neoip_metavar_http_mod_flv_varname']	= "start";
	track_jspf['meta']['location_rangereq_byuri_varname']	= 'start';
		
	# return the just-build track_jspf
	return track_jspf
end

end	# end of class	Bliptv
end	# end of module Track_item_t
end	# end of module Neoip



