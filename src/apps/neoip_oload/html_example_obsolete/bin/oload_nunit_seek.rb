#!/usr/bin/ruby
# - this tools download an original uri and then the same uri thru oload
#   with various range. 
# - it check if the content from oload is the expected one. aka the one 
#   downloaded from the original uri

require 'net/http'
require 'uri'
require 'digest/md5'

class Object
  def assert(bool, message = 'assertion failure')
    raise AssertionFailure.new(message) unless bool
  end
end

# return the body from a http range request
def load_partial(test_url, get_range)
  url = URI.parse(test_url)
  req = Net::HTTP::Get.new(url.path)
  req.range = get_range
  res = Net::HTTP.start(url.host, url.port) {|http|
    http.request(req)
  }
  return res.body
end

def compare_partial(test_url, get_range, orig_body)
  # load the body for the range request
  test_body  = load_partial(test_url, get_range)
  # test that the returned test_body is as long as the requested range
  assert(test_body.size == get_range.to_a.length)
  
  # if the test_body is equal to the orig_body for the requested range, all is ok
  return if test_body == orig_body[get_range]
  # if this point is reached an error occured
  assert( false )
end

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

#orig_url  = "http://jmehost2/~jerome/IBM_Linux_Commercial.avi"
#test_url  = "http://localhost:4550/http:/jmehost2/~jerome/IBM_Linux_Commercial.avi"

orig_url  = "http://jmehost2/~jerome/output.flv"
test_url  = "http://localhost:4550/http:/jmehost2/~jerome/output.flv"

#orig_url  = "http://jmehost2/~jerome/go%20open.mp4"
#test_url  = "http://localhost:4550/http:/jmehost2/~jerome/go%20open.mp4"

#orig_url  = "http://jmehost2/~jerome/IBM_Linux_Commercial.avi"
#test_url  = "http://localhost:4550/*subfile*1/http:/jmehost2/~jerome/video_torrent_try.torrent/IBM_Linux_Commercial.avi"

#orig_url  = "http://jmehost2/~jerome/go%20open.mp4"
#test_url  = "http://localhost:4550/*subfile*1/http:/jmehost2/~jerome/video_torrent_try.torrent/go%20open.mp4"

puts "original url is #{orig_url}"
puts "test url is #{test_url}"


# load the orig file completly in memory
url = URI.parse(orig_url)
req = Net::HTTP::Get.new(url.path)
res = Net::HTTP.start(url.host, url.port) {|http| http.request(req) }
orig_body   = res.body
orig_digest = Digest::MD5.hexdigest(orig_body)
puts "original file is " + orig_body.size.to_s + "-byte long and has a md5 of " + orig_digest

for i in 0..10000
  req_beg = rand(orig_body.size)
  req_len = rand([orig_body.size - req_beg, 500*1024].min)
  req_end = req_beg + req_len
  puts "request #{i} for range #{req_beg..req_end}"
  compare_partial(test_url, req_beg..req_end, orig_body)
end
