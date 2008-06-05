# !/bin/sh
#
# usage: neoip_thumbnail_filename.sh file_name
# this tool generates the filename of the thumbnail for the file in command 
# line parameter as in the freedesktop scheme
# - it just display the file name. nothing more
#   - no file is created. or checked or anything

# copy the first command line parameter in FILE_NAME
FILE_NAME=$1
# make FILE_NAME an absolute path
echo $FILE_NAME | egrep "^/" >/dev/null
[ "$?" != 0 ] && FILE_NAME=$PWD/$FILE_NAME

# compute the url for the file - canonized using python
FILE_URL=file://`python -c 'import urllib; import sys; print urllib.pathname2url(sys.argv[1])' $FILE_NAME`

# THUMB_DIR is the directory where the thumbnails are stored - as in freedesktop scheme
THUMB_DIR=$HOME/.thumbnails/normal

# THUMB_NAME is the filename of the thumbnail
THUMB_NAME=$THUMB_DIR/`echo -n $FILE_URL | md5sum | cut -d" " -f 1`.png

echo $THUMB_NAME
