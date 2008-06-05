#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'rubygems'
require 'hpricot'
require 'open-uri'
require 'neoip_flv_uri_t'


module	Neoip
module	Bliptv
class	Track_pool_t

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(policy_name)
	# copy the parameter
	@policy_name	= policy_name;

	# init the class fields
	@rss_uri_arr	= [];
	@rss_uri_arr	<< "http://tedtalks.blip.tv/rss";
	@rss_uri_arr	<< "http://wildcaster.blip.tv/rss";
	@rss_uri_arr	<< "http://thesmartshow.blip.tv/rss";
	#@rss_uri_arr	<< "http://democracynow.blip.tv/rss";
	#@rss_uri_arr	<< "http://nateperkins.blip.tv/rss";
	#@rss_uri_arr	<< "http://poduniversal.blip.tv/rss";
	@rss_uri_arr	<< "http://make.blip.tv/rss";
	#@rss_uri_arr	<< "http://tubapower.blip.tv/rss";
	#@rss_uri_arr	<< "http://thesportreport.blip.tv/rss";
	#@rss_uri_arr	<< "http://sportcomp.blip.tv/rss";
	#@rss_uri_arr	<< "http://gtchannel.blip.tv/rss";
	#@rss_uri_arr	<< "http://juvimedia.blip.tv/rss";
	#@rss_uri_arr	<< "http://pulsetvtoday.blip.tv/rss";
	# no kframe_idx
	#@rss_uri_arr	<< "http://thedailyreel.blip.tv/rss";

	@track_jspf_arr	= [];
end

################################################################################
################################################################################
#			populate pool
################################################################################
################################################################################

# feed the pool
def populate_pool()
	# get a random rss_uri in the @rss_uri_arr
	rss_uri		= @rss_uri_arr[rand(@rss_uri_arr.length)]	
	# read the rss data from the rss_uri
	rss_doc		= Hpricot::XML(open(rss_uri))
	
	# go thru all the 'item' in the rss
	rss_items	= rss_doc.search("rss > channel > item");

	# iterate over the rss item to get the track_jspf
	nb_contiguous	= [3, rss_items.length].min;
	base_idx	= rss_items.length <= nb_contiguous ? 0 : rand(rss_items.length-nb_contiguous).to_i;
	(nb_contiguous-1).downto(0) do |i|
		rss_item	= rss_items[base_idx + i];
		track_title	= rss_item.at("title").inner_html;
		
		# NOTE: there is a bug/feature in hpricot which make it impossible 
		#       to get media:content
		# - this seems due to the ':'
		# - so i go thru the whole item to get the proper info
		# - other way to workaround - by deriving the .flv name from the mp4 one
		#   - often works... but not always :)
		#   - m4v_uri	= rss_item.at("/enclosure").attributes['url'];
		#   - flv_uri	= m4v_uri.gsub(/.[^.]*$/,'.flv');
		flv_uri		= nil;
		rss_item.traverse_all_element { |e|
			if( e.class == Hpricot::Elem &&	e.name == "media:content" &&
					e.attributes['type']=="video/x-flv") then
				flv_uri	= e.attributes['url'];
			end
		}
		
		# if failed to retrieved the flv_uri, log the event and goto the next
		if( flv_uri == nil )
			$stderr.puts "unable to find flv_uri for track title #{track_title}";
			next;
		end
				
		# build the track_jspf
		track_jspf	= Neoip::Flv_uri_t.to_track_jspf(flv_uri, track_title);
		if( track_jspf == nil )
			$stderr.puts "unable to produce track_jspf for track title #{track_title}";
			next
		end

		# put this track at the end of the @track_jspf_arr
		@track_jspf_arr.push(track_jspf);
	end
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
end	# end of module Bliptv
end	# end of module Neoip



