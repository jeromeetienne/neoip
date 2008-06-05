#!/usr/bin/ruby
#
# \par Brief Description
# This tool build a external flv_mdata file from a http uri pointing to a flv file
# - it will produces a xml file
#   - but this format may change in the future. im a bit afraid about the
#     processing time of xml
# - the fields contains try to match the usual one pushed by flvtool2
# - it makes use of neoip-flv2xml tool to extract the data from the flv file

# list of all 'require'
require "rexml/document"

# TODO get that from the command line
flv_uri	= ARGV[0];

# build the cmdline to run
cmdline		 = "";
cmdline		+= "curl -L #{flv_uri} 2>/dev/null";
cmdline		+= " | ";
# TODO this path is hardcoded for now because i dont install this apps
cmdline		+= "neoip-flv2xml --custom_kframe --tag_meta 2>/dev/null";

# TODO may use open3 to read it as it is produced ?
flv2xml_out	= `#{cmdline}`

# parse the flv2xml_out into xml
xml_input_doc	= REXML::Document.new(flv2xml_out);
xml_in		= xml_input_doc.root;

# Create a empty xml output document
xml_output_doc	= REXML::Document.new;
xml_out		= xml_output_doc.add_element("flv_mdata"); 

# put all keyframes from xml_in into xml_out
kframe_time_arr	= []
kframe_offs_arr	= []
xml_in.each_element('//keyframe') { |keyframe|
	kframe_time_arr	<< keyframe.attributes['timestamp']
	kframe_offs_arr	<< keyframe.attributes['byte_offset']
}
kframe_elem	= xml_out.add_element("keyframes")
kframe_elem.add_element("times").text		=  kframe_time_arr.join(", ");
kframe_elem.add_element("filepositions").text	=  kframe_offs_arr.join(", ");

# TODO this may take many element.... no good
# - take only one, the one in the tag_meta onMetadata
# - as example use the hypo_from_vgoogle.flv
xml_in.each_element('//width') { |elem_in|
	elem_out	= xml_out.add_element("video_width")
	elem_out.text	= elem_in.text;
}
xml_in.each_element('//height') { |elem_in|
	elem_out	= xml_out.add_element("video_height")
	elem_out.text	= elem_in.text;
}

# output the result in stdout
puts xml_out

