#!/usr/bin/ruby
#
# \par Brief Description
# This is a small utility to update playlist. this allow to specify the playlist_id
# and the track_pool which gonna generate it

require 'optparse'
require 'neoip_playlist_t'
require 'neoip_playlist_updater_t'
require 'neoip_track_pool_bliptv_t'
require 'neoip_track_pool_youtube_t'
require 'neoip_track_pool_youporn_t'

################################################################################
################################################################################
# 			parse the command line option
################################################################################
################################################################################
def parse_cmdline(args)
	cmdline_arg	= {};
	# define the default values
	cmdline_arg['loop']		= false;
	cmdline_arg['loop_delay']	= 30;
  
	# initialize the option parser
	opts  = OptionParser.new do |opts|
		opts.banner = "Usage: neoip-playlist-upd.rb [options] playlist_id track_pool_name"
		opts.separator ""
		opts.separator "Specific options:"  

		# 'loop' option
		opts.on('-l', "--loop",
				"explicitly ask to loop") do |val|
			cmdline_arg['loop']	= true
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
	# if the remaining arguments are not of the valid number, display error + help
	if( args.length != 2 )
		puts "Wrong number of arguments!"	
		puts opts.help()
		exit
	end
	# from the remaining arguments, extract the playlist_id/track_pool_name
	cmdline_arg['track_pool_name']	= args[0]
	cmdline_arg['playlist_id']	= args[1]
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

# alias on the playlist_id
playlist_id	= cmdline_opt['playlist_id'];
track_pool_name	= cmdline_opt['track_pool_name'];

# initilialize the track_pool
if( track_pool_name == "bliptv" )
	track_pool	= Neoip::Bliptv::Track_pool_t.new(playlist_id);
elsif( track_pool_name == "youtube" )
	track_pool	= Neoip::Youtube::Track_pool_t.new(playlist_id);
elsif( track_pool_name == "youporn" )
	track_pool	= Neoip::Youporn::Track_pool_t.new(playlist_id);
else
	$stderr.puts "track_pool #{track_pool_name} is unknown. aborting"
	exit
end

# loop on the Playlist_updater_t
while true
	puts	"update #{playlist_id} from #{track_pool_name} - #{Time.now}"
	playlist_updater= Neoip::Playlist_updater_t.new(playlist_id, track_pool);
	playlist_updater.update();
	
	# if no loop must be made, return now
	if cmdline_opt['loop'] == false
		break;
	end
	# wait 'loop_delay' before going on with the next update
	sleep	cmdline_opt['loop_delay']
end


