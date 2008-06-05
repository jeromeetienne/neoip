#!/bin/sh
CONFIG_DIR=$1
CMD=$2
MAIN_CFG_FILE_DIR=/home/jerome/workspace/yavipin/src/cfg_file
shift 2

disp_netdiag() {
	# set the netdiag directory
	NETDIAG_DIR=$CONFIG_DIR/netdiag
	echo -n "The peer in $CONFIG_DIR is "
	# display the nat status
	if [ "`cat $NETDIAG_DIR/natted`" == "NO" ]; then
		echo -n "not behind nat"
	else
		echo -n "behind a "
		if [ "`cat $NETDIAG_DIR/nat_sym`" == "NO" ]; then
			echo -n "non-"
		fi
		echo -n "symmetrical nat with"
		if [ "`cat $NETDIAG_DIR/nat_loopback_support`" == "NO" ]; then
			echo -n "out"
		fi
		echo -n " loopback translation support"
	fi
	echo -n " and "
	if [ "`cat $NETDIAG_DIR/direct_inetreach`" == "NO" ]; then
		echo -n "not directly reachable from the public internet"
	else
		echo -n "directly reachable from the public internet thru "`cat $NETDIAG_DIR/inetreach_src_public_view`
	fi
	
	echo
}

# perform a manual netdiag
netdiag() {
	# set the default parameter
	NAT="NO"
	NAT_SYM="NO"
	NAT_LOOP="NO"
	DIRECT_INETREACH="NO"
	INETREACH_SRC_PUBLIC_VIEW=""
	# parse the parameter
	for i in $*; do
		case $i in
		nat)				NAT="YES";;
		nat_sym)			NAT_SYM="YES";	NAT="YES";;
		nat_loop)			NAT_LOOP="YES"; NAT="YES";;
		src_public_view=*)	INETREACH_SRC_PUBLIC_VIEW=`echo $i| cut -d '=' -f 2-`; DIRECT_INETREACH="YES";;
		esac
	done
	# set the netdiag directory
	NETDIAG_DIR=$CONFIG_DIR/netdiag
	# build the directory if needed
	mkdir -p $NETDIAG_DIR
	# fill it
	echo $NAT						>$NETDIAG_DIR/natted
	echo $NAT_SYM					>$NETDIAG_DIR/nat_sym
	echo $NAT_LOOP					>$NETDIAG_DIR/nat_loopback_support
	echo $DIRECT_INETREACH			>$NETDIAG_DIR/direct_inetreach
	echo $INETREACH_SRC_PUBLIC_VIEW	>$NETDIAG_DIR/inetreach_src_public_view
}

gen_identity() {
	IDENTITY=$1
	echo ideennntt $IDENTITY
	FNAME_PRIVKEY=$CONFIG_DIR/$IDENTITY.privkey.pem
	FNAME_CERT=$CONFIG_DIR/$IDENTITY.cert.pem
	FNAME_CERT_REQ=$CONFIG_DIR/$IDENTITY.cert-req.pem
	ROOT_CA_DIR=/home/jerome/workspace/yavipin/src/config_rdv
	FNAME_CA_PRIVKEY=$MAIN_CFG_FILE_DIR/root_ca_privkey.pem
	FNAME_CA_CERT=$MAIN_CFG_FILE_DIR/root_ca_cert.pem
	# generate the privatekey file
	certtool --generate-privkey --outfile $FNAME_PRIVKEY
	# generate a certificate request
	echo "***** Put your identity in the common name."
	echo "******* TODO see certtool template to make this automatic. unfortunatly they are not documented"
	certtool --generate-request --load-privkey $FNAME_PRIVKEY --outfile $FNAME_CERT_REQ
	# generate the certificate file
    certtool --generate-certificate --load-request $FNAME_CERT_REQ				\
                 --outfile  $FNAME_CERT --load-ca-certificate $FNAME_CA_CERT	\
                 --load-ca-privkey $FNAME_CA_PRIVKEY
	rm $FNAME_CERT_REQ
	echo $IDENTITY >$CONFIG_DIR/peername.neoip
}

# this function setup a new configuration directory
init_config_dir() {
	APP_NAME=$1
	echo app=$APP_NAME
	if [ "$2" == "dev" ]; then
		SUFFIX="_dev"
	fi
	# make the config dir if needed
	mkdir -p $CONFIG_DIR
	cp $MAIN_CFG_FILE_DIR/root_ca_cert.pem			$CONFIG_DIR
	cp $MAIN_CFG_FILE_DIR/neoip_lib_default.ini		$CONFIG_DIR
	cp $MAIN_CFG_FILE_DIR/neoip_lib$SUFFIX.ini		$CONFIG_DIR/neoip_lib.ini
	if [ "$APP_NAME" == "mother" ]; then
		cp $MAIN_CFG_FILE_DIR/root_ca_privkey.pem	$CONFIG_DIR
	else
		cp $MAIN_CFG_FILE_DIR/$(echo $APP_NAME)_default.ini		$CONFIG_DIR
		cp $MAIN_CFG_FILE_DIR/$APP_NAME$SUFFIX.ini				$CONFIG_DIR/$APP_NAME.ini
	fi
}

case $CMD in
	netdiag)
		# write the data
		netdiag $*
		# then display them
		echo -n "Resulting netdiag is :: "
		disp_netdiag
		;;
	disp_netdiag)
		disp_netdiag
		;;
	gen_identity)
		gen_identity $1
		;;
	init_config_dir)
		init_config_dir $*
		;;
	*)
		echo "neoip_manual_startup.sh config_dir command [parameter]"
		echo " This application is for expert only. Use it at your own risk."
		echo " The possible commands are:"
		echo " - gen_identiy identity: generate privatekey and certificate files for identity."
		echo "                      NOTE: It doesnt register to the registeration server."
		echo " - netdiag [nat] [nat_sym] [nat_loop] [src_public_view=otsp_addr]"
		echo "               by default the peer isnt behind and isnt publicly reachable"
		echo " - disp_netdiag        : display the cached netdiag from this config_dir"
		RETVAL=1
		;;
esac


