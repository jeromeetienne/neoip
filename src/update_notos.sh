# !/bin/sh
# jme- update notos to have all the dynamic library and executable there
# - this avoid a local static linkage which consume a lot of time
# - the usage of rsync make it very fast

#
# Build the list of local files to copy
# 
# - put all the .so dynamic library
LOCAL_FILES=`echo *.so`

# - put all the existing excutable
[ -e neoip-nunitapps ]	   && LOCAL_FILES+=" neoip-nunitapps"
[ -e neoip-get ]	   && LOCAL_FILES+=" neoip-get"
[ -e neoip-router ]	   && LOCAL_FILES+=" neoip-router"
[ -e neoip-bstrapntudp ]   && LOCAL_FILES+=" neoip-bstrapntudp"
#[ -e neoip-btcli ]	   && LOCAL_FILES+=" neoip-btcli"
#[ -e neoip-oload ]	   && LOCAL_FILES+=" neoip-oload"

# actually do the rsync on the files
rsync -z -v -u -a --rsh='ssh -p 50022' --stats $LOCAL_FILES poi@btcorp.dyndns.org:/home/poi/workspace/yavipin/src
