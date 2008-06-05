#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require '../playlist_builder/neoip_flv_uri_t'
require 'impl/bliptv/neoip_track_list_bliptv_t'
require 'impl/direct/neoip_track_list_direct_t'
require 'impl/youtube/neoip_track_list_youtube_t'
require 'impl/youporn/neoip_track_list_youporn_t'


module	Neoip
class	Trackgen_t

attr_reader :track_list

################################################################################
################################################################################
#			constructor
################################################################################
################################################################################
def initialize(trackgen_info = nil)
	@track_list	= []
	
	# if a trackgen_info has been given, execute it now
	if( trackgen_info )
		execute(trackgen_info)
	end
end

################################################################################
################################################################################
#			TODO to command
################################################################################
################################################################################
def execute(trackgen_info)
	trackgen_info.each { |operation|
		if( operation['cmd'] == "concat" )
			execute_concat(operation['arg']);
		elsif( operation['cmd'] == "select_random_contiguous" )
			execute_select_random_contiguous(operation['arg']);
		elsif( operation['cmd'] == "filter_field" )
			execute_filter_field(operation['arg']);
		elsif( operation['cmd'] == "pick_one_randomly" )
			execute_pick_one_randomly(operation['arg']);
		elsif( operation['cmd'] == "sort_by" )
			execute_sort_by(operation['arg']);
		end
	}
end

# execute the "concat" command 
def execute_concat(list_desc)
	# handle the "bliptv" type of list
	case list_desc['space']
		when "direct"
			other_list	= Neoip::Track_list_t::Direct_t.build( list_desc['desc'])
			@track_list.concat(other_list);
		when "bliptv"
			other_list	= Neoip::Track_list_t::Bliptv_t.build( list_desc['desc'])
			@track_list.concat(other_list);
		when "youtube"
			other_list	= Neoip::Track_list_t::Youtube_t.build( list_desc['desc'])
			@track_list.concat(other_list);
		when "youporn"
			other_list	= Neoip::Track_list_t::Youporn_t.build( list_desc['desc'])
			@track_list.concat(other_list);
	end
end


# execute the "select_random_contiguous" command 
def execute_select_random_contiguous(arg)
	nb_contiguous	= [arg['nb_contiguous'], @track_list.length].min;
	base_idx	= @track_list.length <= nb_contiguous ? 0 : rand(@track_list.length-nb_contiguous).to_i;
	new_list	= []
	for i in (base_idx..base_idx+nb_contiguous-1)
		new_list.push( @track_list[i] )
	end
	@track_list	= new_list;
end

# execute the "filter_field" command 
def execute_filter_field(arg)
	new_list	= []
	for i in (0..@track_list.length-1)
		# get the value of the arg['field']
		field_value	= @track_list[i].get_field( arg['field'])
		# if this item matches the arg['pattern'], put it in new_list 
		if field_value.match(arg['pattern']) 
			new_list.push( @track_list[i] )
		end
	end
	@track_list	= new_list;
end


# execute the "pick_one_randomly" command 
def execute_pick_one_randomly(arg)
	# compute the prec_sum over all the element of the arg array
	prec_sum	= 0
	arg.each{ |elem| prec_sum += elem['precedence']	}
	
	# determine the picked_index
	prec_rand	= rand(prec_sum);
	for picked_index in 0..arg.length-1
		elem	= arg[picked_index]
		if( prec_rand <= elem['precedence'] )
			break;
		end
		prec_rand	-= elem['precedence']
	end

	# execute the picked one reccursively
	execute( arg[picked_index]['trackgen_info'] );
end



# execute the "sort_by" command 
def execute_sort_by(arg)
	@track_list	= @track_list.sort_by { |item| item.get_field( arg['field']) }
end
################################################################################
################################################################################
#			to_jspf_arr
################################################################################
################################################################################

# convert this track_list_t into a track_jspf_arr
def to_jspf_arr()
	# init the track_jspf_arr
	track_jspf_arr	= []
	# convert the @track_list into track_jspf_arr
	@track_list.each { |item|
		# build the track_jspf
		track_jspf	= item.to_jspf()
		if( track_jspf == nil )
			$stderr.puts "unable to produce track_jspf for track title #{item.track_title}";
		else
			# put this track at the end of the track_jspf_arr
			track_jspf_arr.push(track_jspf);	
		end
	}
	# return the just-built track_jspf_arr
	return track_jspf_arr
end

end	# end of class Track_list_t
end	# end of module Neoip



