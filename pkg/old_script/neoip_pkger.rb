#! /usr/bin/ruby
# this tools is made to help the automatization of building packages
# under nsis_install and epm_install.
# TODO - to document

# if the number of argument is less than 2, display an inline help and return an error
if ARGV.length < 2
        puts "ERROR: Invalid number of parameters"
        puts "#{$0} pkg_type apps_name [list of operation]"
        puts "- if list of operation is not provided, it default to 'full'"
        puts "- example: #{$0} epm_install neoip-oload"
        exit
end

# get the parameters from the command line
pkg_type_list	= [ARGV[0]]
apps_name_list	= [ARGV[1]]
if ARGV.length >= 3
	build_op_list	= [ARGV[2..-1]].flatten!
else
	build_op_list	= ["full"]
end

######################################
# expand all pkg_type_list with the shortcut
pkg_type_list.collect! { |pkg_type|
	if pkg_type == "all"
		pkg_type = ["epm_install", "tgz_install","nsis_install"]
	end
	pkg_type
}.flatten!

######################################
# expand all apps_name_list with the shortcut 
apps_name_list.collect! { |apps_name|
	if apps_name == "all_router"
		apps_name = ["neoip-router", "neoip-bstrapntudp"]
	elsif apps_name == "all_bt"
		apps_name = ["neoip-oload", "neoip-casto", "neoip-casti", "neoip-get", "neoip-btcli", "neoip-btrelay"]
	elsif apps_name == "all"
		apps_name = ["neoip-oload", "neoip-casto", "neoip-casti", "neoip-get", "neoip-btcli", "neoip-btrelay", "neoip-router", "neoip-bstrapntudp", "neoip-upnp", "neoip-flv2xml", "neoip-webpeer"]
	end
	apps_name
}.flatten!

######################################
# expand all build_op_list with the shortcut
build_op_list.collect! { |build_op|
	if build_op == "clean"
		build_op = ["rmdir", "rmpkg"]
	elsif build_op == "full"
		build_op = ["rmdir", "rmpkg", "mkdir", "mkpkg"]
	end
	build_op
}.flatten!

######################################
# do a neoip_pkger for all combinaison of pkg_type_list/apps_name_list/build_op_list
for pkg_type in pkg_type_list
	for apps_name in apps_name_list
		puts "Do a #{pkg_type} #{apps_name} #{build_op_list.join(' ')}"
		system("neoip_pkger_base.rb #{pkg_type} #{apps_name} #{build_op_list.join(' ')}")
	end
end
