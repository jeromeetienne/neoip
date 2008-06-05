#!/usr/bin/ruby
#
# NOTE: this tool is designed to build nested uri from the command line
# usage example:
# $ neoip-nested-uri-builder.rb -t torrent -s Elephants_Dream_480-h264-st-aac.mov -d http://super.com/slota.mov http://localhost:4000 http://tribler.org/content/Elephants_Dream_480-h264-st-aac.mov.torrent?slota
# http://localhost:4000/*link_type*torrent/*subfile*1/*dupuri*http%253A%252F%252Fsuper.com%252Fslota.mov/http/tribler.org/content/Elephants_Dream_480-h264-st-aac.mov.torrent/Elephants_Dream_480-h264-st-aac.mov?slota
#
# TODO this is obsolete...a lot of variables are missing
# - look at neoip-oload-nester-uri man page and update it
# - with outter_var and metavar
# 

require 'uri'
require 'pathname'
require 'optparse'
require 'base64'

################################################################################
# parse the command line option
################################################################################
def parse_cmdline(args)
  # define the default values
  @outter_uri  = nil
  @inner_uri   = nil
  @outter_var  = {}
  @dupuri_db   = []
  
  # initialize the option parser
  opts  = OptionParser.new do |opts|
            opts.banner = "Usage: neoip-nested-uri-builder.rb [options] outter_uri inner_uri"
            opts.separator ""
            opts.separator "Specific options:"  

            # link_type option
            opts.on("-t", "--link_type [torrent|metalink|http_static]",
                    "The type of the link in case it cant be guessed by basename extension") do |val|
                @outter_var[:link_type] = val
            end
            # subfile option
            opts.on("-s", "--subfile subfile_path",
                    "the subfile_path which is used to address multifile metadata link") do |val|
                @outter_var[:subfile] = val
            end
            # dupuri option
            opts.on("-d", "--dupuri duplicate_uri",
                    "one uri which contains a duplicate of the content") do |val|
                @dupuri_db << val
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
  # from the remaining arguments extrace the outter_uri/inner_uri
  @outter_uri = URI.parse(args[0])
  @inner_uri  = URI.parse(args[1])
end

################################################################################
# replace the outter_var[:subfile] pathname with a subfile_level and put the 
# subfile_path in the outter_uri
################################################################################
def process_subfile()
	# if there are no subfile in outter_var, do nothing and return now
	return if @outter_var[:subfile].nil?
  
  # back up the subfile_path
  subfile_path  = "/" + @outter_var[:subfile]
  
  # compute the subfile_level
  subfile_level = 0;
  Pathname.new(subfile_path).cleanpath.each_filename{ |filename| subfile_level += 1 }
  
  # update the inner_uri path - to append the subfile_path
  @inner_uri.path += subfile_path
  
  # remote the subfile outter_var
  @outter_var.delete(:subfile)
  # add a subfile_level outter_var - to replace the subfile with the subfile_level
  @outter_var[:subfile_level] = subfile_level
end

################################################################################
# append all the outter_var into the outter_uri
################################################################################
def process_outter_var()
  # add all member
  @outter_var.each { |key, val|
    @outter_uri.path += "/" + "*" + key.to_s + "*" + val.to_s
  }
end

################################################################################
# append all the outter_var into the outter_uri
################################################################################
def process_dupuri_db()
  @dupuri_db.each { |val|
    # encode the val into a base64 url-safe alphabet
    tmp = Base64.encode64(val).tr("+/","-_").gsub(/\n/,'')
    # add this dupri into the outter_uri path
    @outter_uri.path += "/" + "*dupuri*" + tmp.to_s
  }
end

################################################################################
# build a nested uri
################################################################################
def build_nested_uri()
  nested_uri   = @outter_uri.to_s
  nested_uri  += "/http://"
  nested_uri  += @inner_uri.to_s[7..-1]
  return nested_uri
end

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

# some dummy value for testing
#@outter_uri  = URI.parse("http://localhost:4000")
#@inner_uri   = URI.parse("http://tribler.org/content/Elephants_Dream_480-h264-st-aac.mov.torrent?slota")
#@outter_var  = { :subfile => "Elephants_Dream_480-h264-st-aac.mov" ,
#                 :link_type => "torrent",
#              }
#@dupuri_db   = [ "http://super.com/slota.mov" ]

# parse the command line
parse_cmdline(ARGV)

# process the subfile
process_subfile()
# process the outter_var
process_outter_var()
# process the dupuri_db
process_dupuri_db()
# NOW the nested_uri may be built 
nested_uri=build_nested_uri()
# output the nested_uri on the stdout
puts nested_uri



