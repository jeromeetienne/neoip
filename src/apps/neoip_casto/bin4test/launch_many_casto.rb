#! /usr/bin/ruby
# this tool launches many neoip-casto
# - usefull to be simulate many neoip-casto on a single computer
# - TODO i should unify the launch_many_casto and build_many_casto in a single
#   script
# - usage example: to launch the casto 0 to 3 included on one screen
#   - /home/jerome/workspace/yavipin/src/apps/neoip_casto/bin/launch_many_casto.rb 0 3
# - usage example: to launch the casto 4 to 7 included on another screen
#   - /home/jerome/workspace/yavipin/src/apps/neoip_casto/bin/launch_many_casto.rb 4 7
# - usage example: to build the configuration directory of 0 to 7
#   - /home/jerome/workspace/yavipin/src/apps/neoip_casto/bin/build_many_casto.rb 0 7

# populate the $origfile_arr
def launch_one_casto(idx)

  # compute the window position for this idx
  orig_x      = 0
  orig_y      = 0
  orig_x      += 515 if (idx % 2) == 1
  orig_y      += 358 if (idx % 4) >= 2

  # build the neoip-casto command
  working_dir = "/tmp/casto_test_config_dir#{idx}"
  config_dir  = "../casto_test_config_dir#{idx}"
  casto_cmd   = ""
  casto_cmd   += "cd #{working_dir}"
  casto_cmd   += " && "
  casto_cmd   += "export LD_LIBRARY_PATH=/home/jerome/workspace/yavipin/src/:$LD_LIBRARY_PATH"
  casto_cmd   += " && "
  casto_cmd   += "/home/jerome/workspace/yavipin/src/neoip-casto -c #{config_dir} -d"

  # compute the port
  http_port   = 5000 + idx 
  # build the player_url
  player_url  = "http://localhost:#{http_port}/http/jmehost2:8080/blabla.mpg"
  # build the player_cmd
  player_cmd  =  "mplayer -geometry 95x95+#{orig_x+409}+#{orig_y+262} "
  player_cmd  += "-vo x11 -zoom -nosound #{player_url}"
  
  # launch the xterm launching the neoip-casto
  system("xterm -geometry 84x17+#{orig_x}+#{orig_y} -e \"#{casto_cmd}\" &")
  # give a little time for neoip-casto to start listening on the port
  sleep 3
  # launch the player_cmd
  system("xterm -geometry 67x7+#{orig_x}+#{orig_y+244} -e \"#{player_cmd}\" &")
  # give a little time not to rush the cpu and cause video jitter due to lack of cpu
  sleep 3 
end

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

# if the number of argument is less than 1, display an inline help and return an error
if ARGV.length < 2
        puts "ERROR: Invalid number of parameters"
	puts "Usage: launch_many_casto.rb first_idx last_idx"
	exit 1
end

# get the index of the neoip-casto to launch from the command line parameters
first_idx=ARGV[0].to_i
last_idx=ARGV[1].to_i

# launch the many casto
(first_idx..last_idx).each { |i| launch_one_casto(i) }

