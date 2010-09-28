#!/bin/bash

libnss_neoip_doinstall() {
	# setup some variable
	local ORIGIN_FNAME="/etc/nsswitch.conf"
	local BACKUP_FNAME="/etc/nsswitch.conf.pre_install.libnss_neoip.backup"

	# test if it is nss is already in nssswitch.conf
	grep -e "^hosts:.*neoip.*" $ORIGIN_FNAME >/dev/null
	local not_present="$?"	
	# if already present, return now
	if [ "$not_present" = "0" ]; then
		return 0
	fi
		
	# copy the original file into the backup one
	echo "backup $ORIGIN_FNAME into $BACKUP_FNAME"
	cp $ORIGIN_FNAME $BACKUP_FNAME
	# modify ORIGIN_FNAME to add the 'neoip' in hosts:
	echo "modify $ORIGIN_FNAME to add 'neoip' to the 'hosts:' line"
	sed 's/\(^hosts:.*files\)\(.*\)/\1 neoip\2/' < $BACKUP_FNAME > $ORIGIN_FNAME
}

# update the nss_switch
libnss_neoip_doinstall
# update ld cache
ldconfig
