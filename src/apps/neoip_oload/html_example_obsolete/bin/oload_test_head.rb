#!/usr/bin/ruby
# - this tools download do a simple http HEAD on the uri passed in the cmdline 
#   parameter
# - the goal is to test neoip-oload HEAD.
# - TODO this is half-backed

require 'net/http'
require 'uri'

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################


# get the uri from the cmdline arg
url = URI.parse(ARGV[0])
# do a http HEAD on the url
request	= Net::HTTP::Head.new(url.path)
response= Net::HTTP.start(url.host, url.port) {|http| http.request(request) }

# display all the HTTP header from the response
response.each_key { |key|
	puts "#{key}: #{response[key]}"
}
