#!/usr/bin/ruby
#
# \par Brief Description
# This is a small utility to update playlist. this allow to specify the playlist_id

require 'optparse'
require 'rubygems'
require 'json'
require 'neoip_playlist_updater_t'

################################################################################
################################################################################
# 			parse the command line option
################################################################################
################################################################################
def parse_cmdline(args)
	cmdline_arg	= {};
	# define the default values
	cmdline_arg['noloop']		= false;
	cmdline_arg['loop_delay']	= 30;
  
	# initialize the option parser
	opts  = OptionParser.new do |opts|
		opts.banner = "Usage: neoip-playlist-upd.rb [options] playlist_id track_pool_name"
		opts.separator ""
		opts.separator "Specific options:"  

		# 'noloop' option
		opts.on('-n', "--noloop",
				"explicitly ask NOT to loop") do |val|
			cmdline_arg['noloop']	= true
		end
		# 'loop_delay' option
		opts.on("-d", "--loop_delay nb_second",
			"specify how long to wait between 2 iterations of the loop") do |val|
			cmdline_arg['loop_delay']	= val;
		end

		opts.separator ""
		opts.separator "Common options:"
		# No argument, shows at tail.  This will print an options summary.
		# Try it and see!
		opts.on_tail("-h", "--help", "Show this message") do
			puts opts
			exit
		end
	end
	# parse the args
	opts.parse!(args)
	
	# NOTE: from the remaining arguments, extract them from the remaining args
	# - example: cmdline_arg['track_pool_name']	= args[0]

	# return the result
	return cmdline_arg;
end

################################################################################
################################################################################
#			Program itself
################################################################################
################################################################################

# parse the command line
cmdline_opt	= parse_cmdline(ARGV)

# loop on the Playlist_updater_t
while true
	# get the plistgen_info_arr from the plistgen_info_dirpatch
	plistgen_info_dirpath	= "../new_builder/plistgen_info"
	plistgen_info_fname_arr	= Dir.entries(plistgen_info_dirpath).delete_if{ |x|
					!x.match(/\.plistgen_info$/)
				}

	# update all jspf playlist for each found plistgen_info
	plistgen_info_fname_arr.each { |plistgen_info_fname|
		json_str	= File.read("#{plistgen_info_dirpath}/#{plistgen_info_fname}")
		plistgen_info	= JSON.parse(json_str)
		playlist_id	= plistgen_info['playlist_id'];
		playlist_fname	= "playlist.jspf/dynamic/#{playlist_id}.playlist.jspf";
		
		puts "playlist with playlist_id=#{playlist_id}"
	
		# launch a Playlist_updater_t on this playlist_id
		playlist_updater	= Neoip::Playlist_updater_t.new(plistgen_info, playlist_fname)
		playlist_updater.update();
	}

	
	# if no loop must be made, return now
	if cmdline_opt['noloop'] == true
		break;
	end
	# wait 'loop_delay' before going on with the next update
	puts "======= start to sleep for #{cmdline_opt['loop_delay']}-sec =========="
	sleep	cmdline_opt['loop_delay']
end


