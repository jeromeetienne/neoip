# !/bin/sh
#
# usage: neoip_thumbnailer.sh base_file thumb_file
# this tool tries to produce a thumbnail of the base_file
# - if it succeed, a png picture of at most 128x128 will be created as thumb_file
#   and 0 will be returned
# - if it fails, it report a non zero exit code
#
# - evince is used for application/pdf
# - totem-video-thumbnailer is used for video/*
# - imagemagick convert is used for image/*

# copy the command line parameter
SRCFILE=$1
DSTFILE=$2
# get the mimetype of the source file
MIMETYPE=`file -i $SRCFILE | cut -d" " -f 2`
# get the first part of the mime type
MIMEROOT=`echo $MIMETYPE | cut -d"/" -f 1`

# log to debug
#echo $SRCFILE is a $MIMETYPE

if [ "$MIMETYPE" == "application/pdf" -o "$MIMETYPE" == "application/postscript" ]; then
	#echo using evince to store thumbnails of $SRCFILE in $DSTFILE
	evince-thumbnailer "$SRCFILE" "$DSTFILE"
elif [ "$MIMEROOT" == "video" ]; then
	#echo using totem-video-tumbnailer to store thumbnails of $SRCFILE in $DSTFILE
	# todo here the srcfile MUST be absolute
	totem-video-thumbnailer "file://$SRCFILE" "$DSTFILE"
elif [ "$MIMEROOT" == "image" ]; then
	#echo using imagemagick convert to store thumbnails of $SRCFILE in $DSTFILE
	convert "$SRCFILE" -thumbnail 128x128 "$DSTFILE"
else
	#echo unhandled mimetype $MIMETYPE
	exit -1
fi
