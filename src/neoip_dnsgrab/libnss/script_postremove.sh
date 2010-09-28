#!/bin/bash

libnss_neoip_uninstall() {
	# setup some variable
	local ORIGIN_FNAME="/etc/nsswitch.conf"
	local BACKUP_FNAME="/etc/nsswitch.conf.pre_uninstall.libnss_neoip.backup"

	# test if it is nss is already in nssswitch.conf
	grep -e "^hosts:.*neoip.*" $ORIGIN_FNAME >/dev/null
	local not_present="$?"
	# if not present, no need to uninstall so return now
	if [ "$not_present" = "1" ]; then
		return 0
	fi
	
	# copy the original file into the backup one
	echo "backup $ORIGIN_FNAME into $BACKUP_FNAME"
	cp $ORIGIN_FNAME $BACKUP_FNAME
	# modify ORIGIN_FNAME to remove the 'neoip' in hosts:
	echo "modify $ORIGIN_FNAME to remove 'neoip' from the 'hosts:' line"
	sed 's/\(^hosts:.*\)neoip[ ]\(.*\)/\1\2/' < $BACKUP_FNAME > $ORIGIN_FNAME
}

# update the nss_switch
libnss_neoip_uninstall
# update ld cache
ldconfig
