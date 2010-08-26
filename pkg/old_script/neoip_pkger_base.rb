#! /usr/bin/ruby
# this tools is made to help the automatization of building packages
# under nsis_install and epm_install.
# - TODO to doc more
# - TODO to put logging to provide info - important in case of faillure
# - currently there is the basic to support nsis too
#   - but clearly not complete
# - TODO sort out this canon_apps mess
#   - some stuff got '-' other got '_' why is that ?
#   - executable got '-' by tradition
#   - why do i need a '_' ?
#     - seems like an error from the past... try with canon_name with '-'
#     - remove canon_name and use only apps_name
# - TODO start deamon on install. see comment on epm postint


require "fileutils"

# TODO some sanity check on the current directory
# yavipin/pkg

# determine all the possible pkg_type
pkg_type_supported = ['nsis_install', 'epm_install', "tgz_install"]

################################################################################
# Build a standalone directory for the application 
################################################################################
def mainsrc_dir()
	return "../src";
end

def get_build_target(pkg_type)
	if pkg_type == "epm_install"
		return "linux"
	elsif pkg_type == "tgz_install"
		return "linux";
	elsif pkg_type == "nsis_install"
		return "win32";
	end
	throw "should never happen"
end

# TODO here there is an nameing issue
# - build dir is the directory in which the executable are compile
# - but this script use build_dir for the directory which package file at put
#   - find a better name of the pkg_tmpdir
# - TODO unify all this
#   - possible solution:
#   - SRC_DIR = the path with the source in it
#   - BUILD_DIR = the path with the execuratble
#   - PKGTMP_DIR= the path where the package is built
def get_exec_dir(pkg_type)
	return "../build_#{get_build_target(pkg_type)}";
end

# return the original configuration_dir_sample
# - not to be confused by the one in the build_dir
def cfgdir_sample()
	return "#{mainsrc_dir}/config_dir_sample";
end

# return a string of the path to reach the 'build_dir
def get_build_dir(apps_name)
	return "#{apps_name}.build_dir";
end

# return a string of the apps_suffix
# - apps_suffix is 'router' in 'neoip-router'
def get_apps_suffix(apps_name)
	# sanity check - check if the apps_name starts with "neoip-"
	throw "invalid apps_name #{apps_name}" unless apps_name[0..5] == "neoip-"
	# return the apps_name without the preffix
	return apps_name[6.. -1]
end

def get_canon_name(apps_name)
	return apps_name.gsub(/-/,'_')
end

# return the apps_type
# - apps_type is SYS_BOOT/USR_BOOT/USR_TEMP
def get_apps_type(apps_name)
	canon_name	= get_canon_name(apps_name)
	apps_type	= `grep -e '^#define.*APPS_TYPE' #{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info.hpp | cut -d'"' -f 2 | head -c -1`
	return	apps_type
end

# return a string of the apps_version
# - it is taken from the #{apps_name}_info.hpp in the src and from 
#   the #{apps_name}_main.o compile time
def get_apps_version(pkg_type, apps_name)
	canon_name	= get_canon_name(apps_name)
	apps_version	= `grep -e '^#define.*APPS_VERSION' #{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info.hpp | cut -d'"' -f 2 | head -c -1`
	apps_compiletime= File.stat("#{get_exec_dir(pkg_type)}/apps/#{canon_name}/#{canon_name}_main.o").mtime
	return apps_version + "-" + apps_compiletime.strftime("%Y%m%d%H%M")
end

# return a string of the apps_summary
# - apps_summary is a short oneline summary of the apps
def get_apps_summary(apps_name)
	canon_name	= get_canon_name(apps_name)
	apps_summary	= `grep -e '^#define.*APPS_SUMMARY' #{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info.hpp | cut -d'"' -f 2 | head -c -1`
	return apps_summary
end

# return the apps_description as a array of string
# - the apps_description is a long description of the apps
def get_apps_description(apps_name)
	canon_name	= get_canon_name(apps_name)
	info_file	= File.open("#{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info.hpp").read
	info_line	= info_file.split(/\n/)
	
	# delete all lines from the begining up to "#define NEOIP_APPS_LONGDESC"
	while not info_line[0] =~ /^#define.*NEOIP_APPS_LONGDESC/
		info_line.delete_at(0)
	end
	# define the line "#define NEOIP_APPS_LONGDESC" itself
	info_line.delete_at(0)

	# delete all lines from the end up to "NEOIP_APPS_LONGDESC-ENDMARKER"
	while not info_line[info_line.size-1] =~ /NEOIP_APPS_LONGDESC-ENDMARKER/
		info_line.delete_at(info_line.size-1)
	end
	# define the line "NEOIP_APPS_LONGDESC-ENDMARKER" itself
	info_line.delete_at(info_line.size-1)
	
	# keep only what is between "" and remove the \n in the text
	info_line.collect! { |x| x.split(/\"/)[1].gsub(/\\n/,'') }
	
	# return the info_line which is the description
	return info_line
end

################################################################################
# Build a standalone directory for the application 
# - TODO split this function into one per pkg_type, to comply to the regularity rule
#   - make the common part in a apps_mkdir_common
################################################################################
def apps_mkdir(pkg_type, apps_name)
	# log the event
	puts "mkdir #{apps_name} for #{pkg_type}"

	# get the build_dir
	build_target	= get_build_target(pkg_type)

	# do the mkdir common to all
	apps_mkdir_common(pkg_type, apps_name)
	# forward to build_targer-specific function
	send("apps_mkdir_#{build_target}", pkg_type, apps_name)	
end

def apps_mkdir_common(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	canon_name	= get_canon_name(apps_name)
	exec_dir	= get_exec_dir(pkg_type)
	build_target	= get_build_target(pkg_type)
	# make the apps_basedir
	FileUtils.mkdir "#{build_dir}"

	# make the config_dir_sample 
	FileUtils.mkdir "#{build_dir}/config_dir_sample"

	# copy the kad_bstrap directory	
	FileUtils.cp_r "#{cfgdir_sample}/kad_bstrap", "#{build_dir}/config_dir_sample"	

	# copy all basename of in the config_dir_sample
	dir	= Dir.open("#{cfgdir_sample}");
	dir.each do |basename|
		# if this basename is a directory, skip it
		next unless not File.stat("#{cfgdir_sample}/#{basename}").directory?
		# do a symlink of the basename
		FileUtils.cp_r "#{cfgdir_sample}/#{basename}", "#{build_dir}/config_dir_sample"
	end

	# copy the config directory specific to this apps
	apps_suffix	= get_apps_suffix(apps_name)
	if apps_name == "neoip-webpeer"	# handle the special case for neoip-webpeer
		# neoip-webpeer is a bundle of casti/casto/oload so copy their config dir
		FileUtils.cp_r "#{cfgdir_sample}/casti", "#{build_dir}/config_dir_sample"	
		FileUtils.cp_r "#{cfgdir_sample}/casto", "#{build_dir}/config_dir_sample"	
		FileUtils.cp_r "#{cfgdir_sample}/oload", "#{build_dir}/config_dir_sample"	
	elsif FileTest.exist?("#{cfgdir_sample}/#{apps_suffix}")
		FileUtils.cp_r "#{cfgdir_sample}/#{apps_suffix}", "#{build_dir}/config_dir_sample"	
	end

	# copy the extrsc directory if it exist
	# - NOTE: use directly cp to use the -L option aka 'follow symlink'
	if FileTest.exist?("#{mainsrc_dir}/apps/#{canon_name}/pkg_extrsc")
		system("cp -Lr #{mainsrc_dir}/apps/#{canon_name}/pkg_extrsc #{build_dir}")
	end
end

# do the linux-specific part of mkdir
def apps_mkdir_linux(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	canon_name	= get_canon_name(apps_name)
	exec_dir	= get_exec_dir(pkg_type)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	apps_summary	= get_apps_summary(apps_name)
	apps_description= get_apps_description(apps_name)
	apps_type	= get_apps_type(apps_name)

	# copy the executable
	FileUtils.cp "#{exec_dir}/#{apps_name}-bin-static", "#{build_dir}"
	# TODO disable stripping for now
	system("strip #{build_dir}/#{apps_name}-bin-static")

	# ************** templating of all the .desktop + .init.d	********
	# get external rescources templates
	all_desktop	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.desktop$/.match(x) }
	all_initd	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.init.d$/.match(x) }
	all_sh		= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.sh$/.match(x) }
	all_template	= all_desktop + all_initd + all_sh
	# do the replace on all the template files
	all_template.each { |basename|
		file_path	= "#{build_dir}/pkg_extrsc/#{basename}"
		# read the template
		file_data	= File.open("#{file_path}").read
		file_data.gsub!(/@APPS_NAME@/		, "#{apps_name}");
		file_data.gsub!(/@APPS_TYPE@/		, "#{apps_type}");
		file_data.gsub!(/@APPS_SUMMARY@/	, "#{apps_summary}");
		file_data.gsub!(/@APPS_VERSION@/	, "#{apps_version}");
		file_data.gsub!(/@APPS_DESCRIPTION@/	, "#{apps_description}");
		# write the result to the .xml file
		File.open("#{file_path}", "w") { |fOut| fOut << file_data }	
	}
end

# do the win32-specific part of mkdir
def apps_mkdir_win32(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	canon_name	= get_canon_name(apps_name)
	exec_dir	= get_exec_dir(pkg_type)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	apps_summary	= get_apps_summary(apps_name)
	apps_description= get_apps_description(apps_name)
	apps_type	= get_apps_type(apps_name)

	# copy the executable
	FileUtils.cp "#{exec_dir}/#{apps_name}-bin-static.exe", "#{build_dir}"
	system("i586-mingw32msvc-strip #{build_dir}/#{apps_name}-bin-static.exe")
	# NOTE: for whatever reason im unable to link those lib statically
	FileUtils.ln_s("/home/samba_share/libglib-2.0-0.dll"	, "#{build_dir}")
	FileUtils.ln_s("/home/samba_share/libiconv-2.dll"	, "#{build_dir}")
	FileUtils.ln_s("/home/samba_share/libintl-3.dll"	, "#{build_dir}")	

	# ************** templating of all the .bat	************************
	# get external rescources templates
	all_bat		= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.bat$/.match(x) }
	all_template	= all_bat
	# do the replace on all the template files
	all_template.each { |basename|
		file_path	= "#{build_dir}/pkg_extrsc/#{basename}"
		# read the template
		file_data	= File.open("#{file_path}").read
		file_data.gsub!(/@APPS_NAME@/		, "#{apps_name}");
		file_data.gsub!(/@APPS_TYPE@/		, "#{apps_type}");
		file_data.gsub!(/@APPS_SUMMARY@/	, "#{apps_summary}");
		file_data.gsub!(/@APPS_VERSION@/	, "#{apps_version}");
		file_data.gsub!(/@APPS_DESCRIPTION@/	, "#{apps_description}");
		# write the result to the .xml file
		File.open("#{file_path}", "w") { |fOut| fOut << file_data }	
	}
end

################################################################################
# Remove a standalone directory for the application 
################################################################################
def apps_rmdir(pkg_type, apps_name)
	# log the event
	puts "rmdir #{apps_name} for #{pkg_type}"

	# get the build_dir
	build_dir	= get_build_dir(apps_name)

	# remove all the remaining directory
	# - NOTE: play funky with the path to reduce the risk to do rm_rf on wrong dir
	FileUtils.rm_rf "../../yavipin/pkg/#{build_dir}"
end

################################################################################
# Build the package - assume the standalone directory is already built
################################################################################
def apps_mkpkg(pkg_type, apps_name)
	# log the event
	puts "mkpkg #{apps_name} for #{pkg_type}"
	# forward to pkg_type-specific function
	send("apps_mkpkg_#{pkg_type}", apps_name)
end

def apps_rmpkg(pkg_type, apps_name)
	# log the event
	puts "rmpkg #{apps_name} for #{pkg_type}"
	# forward to pkg_type-specific function
	send("apps_rmpkg_#{pkg_type}", apps_name)
end

################################################################################
# mkpkg/rmpkg for epm_install - assume the standalone directory is already built
################################################################################

# mkpkg for epm_install
def apps_mkpkg_epm_install(apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	canon_name	= get_canon_name(apps_name)
	apps_version	= get_apps_version('epm_install', apps_name)
	apps_summary	= get_apps_summary(apps_name)
	apps_description= get_apps_description(apps_name)
	apps_type	= get_apps_type(apps_name)
	
	# get external rescources
	all_manpage	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.[0-9]$/.match(x) }
	all_desktop	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.desktop$/.match(x) }
	all_initd	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.init.d$/.match(x) }

	# read the postinstall_hook if any
	if FileTest.exist?("#{build_dir}/pkg_extrsc/#{apps_name}.epm_postinstall")
		postinstall_hook = File.open("#{build_dir}/pkg_extrsc/#{apps_name}.epm_postinstall").read
	end
	# read the postremove_hook if any
	if FileTest.exist?("#{build_dir}/pkg_extrsc/#{apps_name}.epm_postremove")
		postremove_hook = File.open("#{build_dir}/pkg_extrsc/#{apps_name}.epm_postremove").read
	end

# TODO neoip-router requires to get libnss-neoip
# - how do i do that ? a special case here ?
# - likely a config file in the pkg_extrsc directory

	# write the result to the .epm_list file
	File.open("#{apps_name}.epm_list", "w") { |fOut|
		fOut.puts("%product #{apps_name} - #{apps_summary}")
		fOut.puts("%copyright 2007")
		fOut.puts("%vendor NeoIP")
		fOut.puts("%version #{apps_version}")
		fOut.puts("%readme \"Read http://off.net/~jme\" ")
		fOut.puts("%license \"http://donotusethissoft.ever\"")
		apps_description.collect { |x| "%description #{x}" }.each { |x| 
			fOut.puts("#{x}")
		}
		
		fOut.puts("")
		fOut.puts(`mkepmlist -u root -g root --prefix /etc/#{canon_name} #{build_dir}/config_dir_sample`)
		
		# if appt_type == *_BOOT
		# - make the binary exec in ${apps_name}-bin
		# - add a ctrl scrip in ${apps_name}-ctrl
		# else (aka if apps_type == *_TEMP)
		# - make the binary exec in #{apps_name}
		if apps_type =~ /.*_BOOT/
			fOut.puts("f 755 root root /usr/lib/#{canon_name}/#{apps_name}-bin #{build_dir}/#{apps_name}-bin-static")
			fOut.puts("f 755 root root /usr/bin/#{apps_name}-ctrl #{build_dir}/pkg_extrsc/#{apps_name}-ctrl.sh")
		else
			fOut.puts("f 755 root root /usr/bin/#{apps_name} #{build_dir}/#{apps_name}-bin-static")
		end
		
		# add all the manpage
		all_manpage.each { |src_file|
			dst_dirname	= "/usr/local/man/man#{src_file[-1..-1]}"
			fOut.puts("f 755 root root #{dst_dirname}/#{src_file} #{build_dir}/pkg_extrsc/#{src_file}")
		}

		# add all the desktop
		all_desktop.each { |src_file|
			dst_dirname	= "/usr/share/autostart"
			fOut.puts("f 644 root root #{dst_dirname}/#{src_file} #{build_dir}/pkg_extrsc/#{src_file}")
		}

		# add all the initd
		all_initd.each { |src_file|
			dst_file	= "/etc/init.d/#{src_file.gsub(/(.*)\.init.d$/, '\1')}"
			fOut.puts("f 755 root root #{dst_file} #{build_dir}/pkg_extrsc/#{src_file}")
		}

		# do the post-install
		fOut << "%postinstall <<EOF\n"
		fOut << "	#{postinstall_hook}\n"
		if all_initd.size > 0 
			# if there are init.d files, do a update-rc.d		
			# - update-rc.d is debian specific
			# - apparnetly it is "chkconfig" for fedora
			fOut << "update-rc.d #{apps_name} defaults\n"
		end
		# start the application automatically in the postinstall
		if apps_type == "USR_BOOT"
			# if USR_BOOT and installed via sudo, launch it for SUDO_USER
			fOut << "if [ \"$$SUDO_USER\" != \"\" ]; then\n"
			fOut << "	su $$SUDO_USER -c \"#{apps_name}-ctrl start\"\n"
			fOut << "fi\n"
		elsif apps_type == "SYS_BOOT"
			# if SYS_BOOT, launch it as root now
			fOut << "#{apps_name}-ctrl start\n"
		end
		fOut << "	exit 0\n"
		fOut << "EOF\n"			

		fOut << "%postremove <<EOF\n"
		fOut << "	#{postremove_hook}\n"
		if all_initd.size > 0 
			# if there are init.d files, do a update-rc.d		
			# - update-rc.d is debian specific
			fOut << "	update-rc.d #{apps_name} remove\n"
		end
		fOut << "	exit 0\n"
		fOut << "EOF\n"			
		
		# if the apps_type is *_BOOT, put a preinstall/preremove script to kill the daemon
		# - NOTE: it is not possible to use the *-ctrl script as it is local to 
		#   each users and package installation is at the system level		
		if apps_type =~ /.*_BOOT/
			fOut << "%preinstall <<EOF\n"
			fOut << "	[ \"`pidof #{apps_name}-bin`\" != \"\" ] && killall #{apps_name}-bin\n"
			fOut << "	exit 0\n"
			fOut << "EOF\n"
			fOut << "%preremove <<EOF\n"
			fOut << "	[ \"`pidof #{apps_name}-bin`\" != \"\" ] && killall #{apps_name}-bin\n"
			fOut << "	exit 0\n"
			fOut << "EOF\n"
		end
	}
	
	# NOTE: epm compress using gzip. which is quite bad compared to lzma
	# - on #ubuntu-motu [01:19] "<RAOF> zorglu_: This depends on your pakcage's 
	#   build system (debhelper? cdbs? anything else), but a simple check could
	#   be to simply unpack your existing deb and repack it with 'dpkg-deb --build
	#   -Zlzma <packagedir>'"
	# - extract may be "dpkg -x ../neoip-webpeer_0.0.1-200801150047_i386.deb  webpeer"
	# - this produce a "dpkg-deb: unknown compression type `lzma'!"
	
	# about package size, on a webpeer linux with -O0
	# - pure7z=1705165, tar.gz=3170879(with gzip -9), tar.7z=2294475(top 7z compression)
	# - .deb = 3162192
	
	# build the package itself
	puts ("Doing 'sudo epm' to build the epm_package. if it asks for a password, give sudo password.")
	system("sudo epm -n --output-dir . -f deb #{apps_name} #{apps_name}.epm_list")

	# epm put '-' between name/version, and version/arch
	# - just rename it the usual debian way
	system("mv #{apps_name}-#{apps_version}.deb #{apps_name}_#{apps_version}_i386.deb")
	
	# remove the configuration file
	#FileUtils.rm_f "#{apps_name}.epm_list"
end

# mkpkg for epm_install
def apps_rmpkg_epm_install(apps_name)
	# remove the .deb for all version
	Dir.glob("#{apps_name}_*_i386.deb") { |filename| FileUtils.rm_f filename	}
end


################################################################################
# mkpkg/rmpkg for tgz_install - assume the standalone directory is already built
################################################################################

# mkpkg for tgz_install
def apps_mkpkg_tgz_install(apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version('tgz_install', apps_name)
	apps_type	= get_apps_type(apps_name)

	# TODO put an easy script to handle the start stop more easily ?
	# - would be nice but how to do that ? 
	# - this script should be called #{apps_name} in the build_dir
	#   - this is what firefox/azureus do
	# - a difference is that it is a pure deamon
	# - so the best would be to have something similar to the -ctrl script

	# TODO maybe another script install.sh and uninstall.sh
	# - which install/uninstall all the external resource like a .deb/.rpm 

	# create a symlink to ensure that the .tgz will extract in a 'normal' directory name
	# - i.e. #{apps_name}_#{apps_version}
	FileUtils.ln_s "#{build_dir}", "#{apps_name}_#{apps_version}"
	# build the .tgz 
	system("tar chzf #{apps_name}_#{apps_version}_i386.tgz #{apps_name}_#{apps_version}");
	# remove the symlink for clean extraction dirname
	FileUtils.rm_f "#{apps_name}_#{apps_version}"
end

# mkpkg for tgz_install
def apps_rmpkg_tgz_install(apps_name)
	# remove the installer .tgz for all version
	Dir.glob("#{apps_name}_*.tgz") { |filename| FileUtils.rm_f filename	}
end

################################################################################
# mkpkg/rmpkg for nsis_install - assume the standalone directory is already built
################################################################################

# mkpkg for nsis_install
def apps_mkpkg_nsis_install(apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version('nsis_install', apps_name)
	apps_type	= get_apps_type(apps_name)
	
	# read the template
	# - TODO i should improve the template to get more flexible
	#   - especially the version which is completly hidden
	#   - neoip-oload-0.0.1.exe ?
	intf_file	= File.open("template_nsis_install.nsi").read
	intf_file.gsub!(/@APPS_NAME@/		, "#{apps_name}");
	intf_file.gsub!(/@APPS_VERSION@/	, "#{apps_version}");
	# write the result to the .xml file
	File.open("#{apps_name}.nsi", "w") { |fOut| fOut << intf_file }

	# make the package itself now
	nsis_option	= "";
	if apps_type =~ /.*_BOOT/
		nsis_option += " -DNEOIP_DAEMON";
	end
	#puts "makensis #{nsis_option} #{apps_name}.nsi"
	system("makensis #{nsis_option} #{apps_name}.nsi")
end

# mkpkg for nsis_install
def apps_rmpkg_nsis_install(apps_name)
	# remove the installer .exe for all version
	Dir.glob("#{apps_name}-*.exe") { |filename| FileUtils.rm_f filename	}
	# remove the nsis configuration file
	FileUtils.rm_f "#{apps_name}.nsi"
end

################################################################################
# upload the package - assume the package has been built
################################################################################

def apps_upload(pkg_type, apps_name)
	# log the event
	puts "upload #{apps_name} for #{pkg_type}"
	# forward to pkg_type-specific function
	send("apps_upload_#{pkg_type}", apps_name)
end

# upload for epm_install
def apps_upload_epm_install(apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version('epm_install', apps_name)
	# upload to jmeserv
	system("scp #{apps_name}_#{apps_version}_i386.deb jmeserv:public_html/download/linux")
end

# upload for tgz_install
def apps_upload_tgz_install(apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version('tgz_install', apps_name)
	# upload to jmeserv
	system("scp #{apps_name}_#{apps_version}_i386.tgz jmeserv:public_html/download/linux")
end

# upload for nsis_install
def apps_upload_nsis_install(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version('nsis_install', apps_name)
	# upload to jmeserv
	system("scp #{apps_name}-#{apps_version}.exe jmeserv:public_html/download/win32")
end

################################################################################
################################################################################
# Program itself
################################################################################
################################################################################

# if the number of argument is not 3, display an inline help and return an error
if ARGV.length < 3
        puts "ERROR: Invalid number of parameters"
	puts "Usage: builder_op.rb pkg_type apps_name build_op [build_op]+"
	exit 1
end


# get the parameter from the command line
pkg_type	= ARGV[0]
apps_name	= ARGV[1]

# sanity check on the parameter pkg_type
if pkg_type_supported.include?(pkg_type) == false
        puts "ERROR: pkg_type '#{pkg_type}' is invalid. only [#{pkg_type_supported.join('|')}] are valid"
	exit 1
end

# sanity check on the parameter apps_name
if apps_name[0..5] != "neoip-"
        puts "ERROR: apps_name '#{apps_name}' is invalid. all apps_name MUST start with 'neoip-'"
	exit 1
end

# loop over all the build_op at the end of the command line parameter
for curr_arg in (2..ARGV.length-1)
	# get the build_op
	build_op	= ARGV[curr_arg]
	
	# sanity check on the parameter build_op
	if( build_op != "mkdir" and build_op != "rmdir" and build_op != "mkpkg" and build_op != "rmpkg" and build_op != "upload")
	        puts "ERROR: build_op '#{build_op}' is invalid. only 'mkdir', 'rmdir', 'mkpkg', 'rmpkg' and 'upload'  are valid."
		exit 1
	end

	# call the various function depending on the build_op
	if build_op == "mkdir"
		apps_mkdir( pkg_type, apps_name )
	elsif build_op == "rmdir"
		apps_rmdir( pkg_type, apps_name )
	elsif build_op == "mkpkg"
		apps_mkpkg( pkg_type, apps_name )
	elsif build_op == "rmpkg"
		apps_rmpkg( pkg_type, apps_name )
	elsif build_op == "upload"
		apps_upload( pkg_type, apps_name )
	end
end
