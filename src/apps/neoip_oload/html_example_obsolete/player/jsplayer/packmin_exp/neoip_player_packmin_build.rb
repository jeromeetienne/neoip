#!/usr/bin/env ruby

require 'tempfile'

html_fname	= "neoip_player_demo.html"
jsfname_arr	= []

# parse html_fname file to extract all the needed js file and put them into jsfname_arr
File.read(html_fname).split(/\n/).each do |line|
	line.chomp
	# if this line contain the duration, extract it
	if line =~ /<script src=/
		jsfname	= line.scan(/<script src=\"(.*)\"/)[0][0].to_s
		jsfname_arr	<< jsfname
	end
end

# read all the jsfname_arr files and put them into jsfdata_expand 
jsfdata_expand	= ""
jsfname_arr.each { |jsfname| jsfdata_expand += File.read(jsfname)	}

# write the jsdata_str into jsfname_tmpfile
jsfname_tmpfile	= "/tmp/neoip_player_pack_builder.tmp.js"
File.open(jsfname_tmpfile, "w+") { |file| file.write(jsfdata_expand)	}

# compress the jsfdata_tmpfile with yuicompressor
cmdline	= "java -jar /home/jerome/tmp/yuicompressor-2.2.4/build/yuicompressor-2.2.4.jar"
cmdline	+= " #{jsfname_tmpfile}"
cmdline	+= " 2>/dev/null"
jsfdata_packed	= `#{cmdline}` 

# unlink the jsfname_tmpfile
#File.unlink(jsfname_tmpfile)

# write the jsfdata_packed into a file
jsfname_packed	= "neoip_player_packmin.js"
File.open(jsfname_packed, "w+") { |file| file.write(jsfdata_packed)	}


# display some statistic on the compression ratio
expand_ratio	= 1.0
packed_ratio	= 1.0 * jsfdata_packed.length/jsfdata_expand.length;
totlen_gziped	= `cat #{jsfname_packed} | gzip | wc -c`.to_f
gziped_ratio	= totlen_gziped/jsfdata_expand.length;
puts "total length expand=#{jsfdata_expand.length}-byte ratio=#{"%01.2f%%" % (100.0 * expand_ratio)}"
puts "total length packed=#{jsfdata_packed.length}-byte ratio=#{"%01.2f%%" % (100.0 * packed_ratio)}"
puts "total length packed+gzip=#{totlen_gziped}-byte ratio=#{"%01.2f%%" % (100.0 * gziped_ratio)}"

