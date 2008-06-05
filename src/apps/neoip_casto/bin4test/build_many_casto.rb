#! /usr/bin/ruby
# this tool build many neoip-casto configuration directories in /tmp
# - usefull to be simulate many neoip-casto on a single computer
# - usage example apps/neoip_casto/bin/build_many_casto.rb 0 3
#   - to build the configuration directory from the index 0 to 3

require "fileutils"

# populate the $origfile_arr
def build_one_dir(idx)
  # build the neoip-casto command
  src_dir     = "/home/jerome/workspace/yavipin/src/config_dir_debug1"
  dst_dir     = "/tmp/casto_test_config_dir#{idx}"
  # compute the port for this configuration directory
  debug_port  = 10000 + idx
  http_port   = 5000 + idx 

  # copy the directory
  FileUtils.cp_r src_dir, dst_dir
  # replace 9080 by the debug_port in dst_dir/neoip_session.conf
  system("sed -i s/9080/#{debug_port}/g #{dst_dir}/neoip_session.conf")
  # replace 4000 by the http_port in dst_dir/oload/neoip_casto.conf
  system("sed -i s/4000/#{http_port}/g #{dst_dir}/casto/neoip_casto.conf")
end

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

# if the number of argument is less than 2, display an inline help and return an error
if ARGV.length < 2
        puts "ERROR: Invalid number of parameters"
	puts "Usage: launch_many_csto.rb first_idx last_idx"
	exit 1
end

# get the index of the neoip-casto to launch from the command line parameters
first_idx=ARGV[0].to_i
last_idx=ARGV[1].to_i

# build the many directory
(first_idx..last_idx).each { |i| build_one_dir(i) }
