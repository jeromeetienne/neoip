#!/usr/bin/ruby
# - this tools connect the url given in the cmdline parameter and measure
#   the delay between before the connection attempts and the first received
#   byte.
# - GOAL: to test the latency thru neoip-oload
# - i failed to read only 1 byte from a file over http
#   - ruby seems to download the whole file.
# - so here is an alternative in bash
#   time (curl -L "http://127.0.0.1:4550/flv/*static_filelen*13385143/http://blip.tv/file/get/TheSmartShow-TheSmartShowEpisode19352.flv" 2>/dev/null| read -n 1)
#   - for this file, latency of the first byte is 1.5-sec thru neoip-oload and 0.8-sec direct
#     - BUG BUG this should not be
#     - this is not specific to this file


require 'benchmark'

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################


# get the uri from the cmdline arg
url = ARGV[0]

puts "url=#{url}"

puts Benchmark.measure {
	first_char	= `curl -L "#{url}" 2>/dev/null | read -n 1`
}
