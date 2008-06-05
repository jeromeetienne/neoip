#!/usr/bin/ruby
#
# \par Brief Description
# to handle a blip.tv API
# - currently not complete at all
# - must contains only the blip.tv API and nothing specific to the
#   neoip stuff

# the 'require' for the library i use
require 'open-uri'


module Bliptv

################################################################################
################################################################################
#			Constant definition
################################################################################
################################################################################

class Category	# list found in http://wiki.blip.tv/index.php/Categories
	BOOKS			= 11
	CITIZEN_JOURNALISM	= 4
	COMEDY			= 2
	CONFERENCE		= 10
	DEFAULT			= -1
	DOCUMENTARY		= 19
	EDUCATIONAL		= 21
	FOOD_AND_DRINK		= 12
	FRIENDS			= 15
	GAMES			= 13
	MUSIC_AND_ENTERTAINMENT	= 5
	PERSONAL		= 8
	POLITICS		= 3
	SCHOOL_AND_EDUCATION	= 16
	SCIENCE			= 14
	SPORT			= 6
	TECHNOLOGY		= 7 
	ENVIRONMENT		= 20
	MAINSTREAM_MEDIA	= 18
	WEB			= 17
	VIDEOBLOGGING		= 9
end

class OutputMethod	# list found in http://wiki.blip.tv/index.php/Output_Method
	RSS		= "rss"
	JSON		= "json"
	ATOM		= "atom"
	OPML		= "opml"
	FOAF		= "foaf"
end

class License		# list found in http://wiki.blip.tv/index.php/License_Types
	NO_LICENSE	= -1
	CC_BY		= 1
	CC_BY_ND	= 2
	CC_BY_NC_ND	= 3
	CC_BY_NC	= 4
	CC_NC		= 5 
	CC_SA		= 6
	PUBLIC_DOMAIN	= 7 
end


################################################################################
################################################################################
#			Build query uri
################################################################################
################################################################################

def self.build_query_uri(query_arg)
	base_uri		= "http://http.blip.tv"
	var_arr			= {};
	var_arr['pagelen']	= 100;
	var_arr['skin']		= "rss"	
	
	# TODO improve the building to be more flexible
	# TODO beef up the sanity check to ensure the query_argument are valid
	# TODO most likely missed query_argument from the bliptv main api
	# TODO likely some issue with the uri escaping too
	# - there is a class for uri in ruby. what about using it ? :)
	
	if query_arg['base_uri']
		base_uri		= query_arg['base_uri']
	end
	if query_arg['skin']
		var_arr['skin']		= query_arg['skin'];
	end
	if query_arg['search']
		var_arr['search']	= query_arg['search'];
	end
	if query_arg['pagelen']
		var_arr['pagelen']	= query_arg['pagelen'];
	end
	if query_arg['explicit_content'] == true
		var_arr['show_nsfw']	= 1;
	end
	if query_arg['license']
		license			= query_arg['license']
		if( license.class == String )
			license		= eval("Bliptv::License::#{license.upcase}")
		end
		var_arr['license']	= license;
	end
	if query_arg['category']
		category		= query_arg['category']
		if( category.class == String )
			category	= eval("Bliptv::Category::#{category.upcase}")
		end
		var_arr['category']	= category;
	end

	query_uri	= base_uri;
	# append all the variables to the uri
	var_arr.each_with_index { |item, index|
		# add the variable separator
		query_uri	+= (index == 0? "?": "&")
		query_uri	+= "#{URI::escape(item[0].to_s)}=#{URI::escape(item[1].to_s)}"
	}
	# log to debug
	puts "query_uri=#{query_uri}"
	# return the just-built uri
	return query_uri
end

################################################################################
################################################################################
#			do query data
################################################################################
################################################################################

def self.query_data(query_arg)
	query_uri	= build_query_uri(query_arg);
	data		= open(query_uri){ |f| f.read }
	return data;
end

end	# end of module BlipTV


