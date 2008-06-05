# !/bin/sh 
# dump the /etc/mime.types file and extract "c++ instruction to cut past in neoip_mimetype.cpp"
cat /etc/mime.types | while true; do
	# read one line
	read line
	# if this read returned an error, considere the file fully processed
	if [ "$?" != "0" ]; then
		break;
	fi
	# strip the line
	line=`echo "$line" | tr -s "[:space:]" | tr "[:space:]" " "`
	# zero it if it is a comment
	line=`echo "$line" | grep -v ^#`
	# if this line is empty, goto the next
	if [ -z "$line" ]; then
		continue;
	fi
	
	# get the mimetype from the line
	mimetype=`echo "$line" | cut -d" " -f 1`

	# got the extensions of this mime type
	for ext in `echo "$line" | cut -d" " -f 2-`; do
		# echo the "c++ line"
		echo "add_type(\""$ext"\", \""$mimetype"\");"
	done
done
