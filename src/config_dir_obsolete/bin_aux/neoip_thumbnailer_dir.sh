# !/bin/sh
#
# usage: neoip_thumbnailer_dir.sh dir_name
# this tool go thru the whole directory and tries to produce a icon for each of 
# the files.
# - neoip_thumbnailer.sh is used to produce the icon of a file
# - it use the freedesktop thumbnail scheme
# - if an icon file already exists and is newer than the base file, dont try to produce a new one

# copy the first command line parameter in DIR_NAME
DIR_NAME=$1
# make DIR_NAME an absolute path
echo $DIR_NAME | egrep "^/" >/dev/null
[ "$?" != 0 ] && DIR_NAME=$PWD/$DIR_NAME

# THUMB_DIR is the directory where the thumbnails are stored - as in freedesktop scheme
THUMB_DIR=$HOME/.thumbnails/normal

# compute the name of the temporary file
TMP_FILE=/tmp/neoip_thumbnailer_dir.$$.png

# go thru the whole directory
for i in `find $DIR_NAME -type f`;do
	# set the FILE_NAME
	FILE_NAME=$i
	# compute the url for the file - canonized using python
	FILE_URL=file://`python -c 'import urllib; import sys; print urllib.pathname2url(sys.argv[1])' $FILE_NAME`
	# compute the thumbnail name - as in freedesktop scheme
	THUMB_NAME=$THUMB_DIR/`echo -n $FILE_URL | md5sum | cut -d" " -f 1`.png
	# if the thumbnail file exists and is newer than the filename, skip this file
	[ "$FILE_NAME" -ot "$THUMB_NAME" ] && continue
	echo try to generate $FILE_NAME $TMP_FILE
	# try to generate the thumbnail into the temporary file
	neoip_thumbnailer.sh $FILE_NAME $TMP_FILE
	# if the generation failed, skip this file
	[ "$?" != 0 ] && continue
	# copy the temporary file into the freedesktop directory
	cp $TMP_FILE $THUMB_NAME
done

# remove the temporary file
#[ -e $TMP_FILE ] && rm $TMP_FILE 

