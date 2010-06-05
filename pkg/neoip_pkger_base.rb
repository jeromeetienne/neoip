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
# - the current naming of the pkt_type is poor
#   - TODO to refactor it
#   - list of package description
#     - deb linux i386 dev
#     - rpm linux i64 rel
#     - nsis win32 i386 dev

# - TODO about EEEpc xandros packaging
#   - there is an issue in the neoip-webpack-ctrl with start-stop-daemon and the PATH
#     - just add the /sbin in PATH at the begining of the script if it is not present in PATH
#   - there is an issue with the neoip-dnsresolv.sh with a tools which is not present
#     - found a work around on the EEEpc install
#   - neoip-webpack.desktop is not called

require "fileutils"

# TODO some sanity check on the current directory
# yavipin/pkg

# determine all the possible pkg_type
pkg_type_supported = ['nsis_install', 'macos_install', 'deb_install', 'rpm_install', "tgz_install", "ppa_install"]

################################################################################
# Build a standalone directory for the application 
################################################################################
def mainsrc_dir()
	return "../src";
end

def get_build_target(pkg_type)
	arr	= {
		"deb_install"	=> "linux",
		"rpm_install"	=> "linux",
		"tgz_install"	=> "linux",
		"ppa_install"	=> "linux",
		"nsis_install"	=> "win32",
		"macos_install"	=> "macos",
		}
	throw "should never happen" if !arr[pkg_type]
	return arr[pkg_type]
end

def get_pkg_fext(pkg_type)
	arr	= {
		"deb_install"	=> "deb",
		"rpm_install"	=> "rpm",
		"tgz_install"	=> "tgz",
		"ppa_install"	=> "PPANOCLUE",
		"nsis_install"	=> "exe",
		"macos_install"	=> "dmg",
		}
	throw "should never happen" if !arr[pkg_type]
	return arr[pkg_type]
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
	apps_type	= `grep -e '^#define.*APPS_TYPE' #{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info.hpp | cut -d'"' -f 2 | tr -d "\n"`
	return	apps_type
end

# return a string of the apps_version
# - it is taken from the #{apps_name}_info.hpp in the src and from 
#   the #{apps_name}_main.o compile time
def get_apps_version(pkg_type, apps_name)
	canon_name	= get_canon_name(apps_name)
	apps_version	= `grep -e '^#define.*APPS_VERSION' #{mainsrc_dir}/apps/#{canon_name}/#{canon_name}_info_version.hpp | cut -d'"' -f 2 | tr -d "\n"`
	#apps_compiletime= File.stat("#{get_exec_dir(pkg_type)}/apps/#{canon_name}/#{canon_name}_main.o").mtime
	apps_compiletime= File.stat(get_build_dir(apps_name)).ctime
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

def patch_templ_file(src_fname, dst_fname, variables)
	# read the src file
	content	= File.read(src_fname)
	# patch each variables in it
	variables.each { |key, val|	content.gsub!("@#{key}@", val)	}
	# write the destination
	File.open(dst_fname, "w") { |fd| fd.write(content) }
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
	if apps_name == "neoip-webpack"	# handle the special case for neoip-webpack
		# neoip-webpack is a bundle of casti/casto/oload so copy their config dir
		FileUtils.cp_r "#{cfgdir_sample}/casti", "#{build_dir}/config_dir_sample"	
		FileUtils.cp_r "#{cfgdir_sample}/casto", "#{build_dir}/config_dir_sample"	
		FileUtils.cp_r "#{cfgdir_sample}/oload", "#{build_dir}/config_dir_sample"	
	elsif FileTest.exist?("#{cfgdir_sample}/#{apps_suffix}")
		FileUtils.cp_r "#{cfgdir_sample}/#{apps_suffix}", "#{build_dir}/config_dir_sample"	
	end

	# copy the extrsc directory if it exist
	# - NOTE: use directly cp to use the -L option aka 'follow symlink'
	if FileTest.exist?("#{mainsrc_dir}/apps/#{canon_name}/pkg_extrsc")
		system("cp -LR #{mainsrc_dir}/apps/#{canon_name}/pkg_extrsc #{build_dir}")
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


	# build the application
	cmdline	= "cd #{exec_dir} && ./m #{apps_name}-bin-static"
	system(cmdline)

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
		# - TODO port it to patch_templ_file()
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
	dll_srcdir	= "/home/jerome/win32/glib/inst/bin"
	FileUtils.ln_s("#{dll_srcdir}/libglib-2.0-0.dll"	, "#{build_dir}")
	FileUtils.ln_s("#{dll_srcdir}/libiconv-2.dll"		, "#{build_dir}")
	FileUtils.ln_s("#{dll_srcdir}/libintl-3.dll"		, "#{build_dir}")	

	# ************** templating of all the .bat	************************
	# get external rescources templates
	all_bat		= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.bat$/.match(x) }
	all_template	= all_bat
	# do the replace on all the template files
	all_template.each { |basename|
		file_path	= "#{build_dir}/pkg_extrsc/#{basename}"
		# read the template
		# - TODO port it to patch_templ_file()
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

# do the macos-specific part of mkdir
def apps_mkdir_macos(pkg_type, apps_name)
	# get pkg_type/apps_name derivative variables
	exec_dir	= get_exec_dir(pkg_type)
	build_dir	= get_build_dir(apps_name)	
	apps_type	= get_apps_type(apps_name)
	
	# TODO support the other apps_type too
	throw "only SYS_BOOT apps is implemented for macos build" unless apps_type == "SYS_BOOT"

	# copy the executable
	FileUtils.cp "#{exec_dir}/#{apps_name}-bin-static", "#{build_dir}"
	system("strip #{build_dir}/#{apps_name}-bin-static")

	# copy the launchd plist script
	# - TODO should be done for SYS_BOOT ONLY
	FileUtils.cp "macos_pkg_extrsc/org.neoip.webpack.plist", "#{build_dir}/pkg_extrsc"
	
	# Move the typical install directory into a subdirectory #{apps_name}
	FileUtils.mkdir "#{build_dir}/#{apps_name}"
	["config_dir_sample", "#{apps_name}-bin-static", "pkg_extrsc"].each { |fName|
		FileUtils.mv("#{build_dir}/#{fName}", "#{build_dir}/#{apps_name}")
	}
	
	# TODO what about all the shared lib i included thru macport ?
	# - "otool -L youexecname" to see the list
	# - should i copy them in the home directory ?
	# - likely... how to test ? ask alain ?
	# - waiting for some users to complain :)

	# copy file which are needed to build the packages
	FileUtils.cp "macos_pkg_extrsc/neoip-webpack.packproj"	, "#{build_dir}/"
	FileUtils.cp "macos_pkg_extrsc/my_postflight.sh"	, "#{build_dir}/"
	FileUtils.cp "macos_pkg_extrsc/television.jpg"		, "#{build_dir}/"
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
	send("apps_mkpkg_#{pkg_type}", pkg_type, apps_name)
end

def apps_rmpkg(pkg_type, apps_name)
	# log the event
	puts "rmpkg #{apps_name} for #{pkg_type}"
	# forward to pkg_type-specific function
	send("apps_rmpkg_#{pkg_type}", pkg_type, apps_name)
end

################################################################################
# mkpkg/rmpkg for epm_install - assume the standalone directory is already built
################################################################################

# mkpkg common for all pkg_type using epm
def apps_mkpkg_epm_common(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	canon_name	= get_canon_name(apps_name)
	apps_version	= get_apps_version(pkg_type, apps_name)
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
		fOut.puts("%copyright 2008")
		fOut.puts("%vendor NeoIP")
		fOut.puts("%version #{apps_version}")
		fOut.puts("%readme \"Read http://urfastr.net/webpack\" ")
		fOut.puts("%license \"http://donotusethissoft.ever\"")
		apps_description.collect { |x| "%description #{x}" }.each { |x| 
			fOut.puts("#{x}")
		}
		
		fOut.puts("")
		fOut.puts(`mkepmlist -u root -g root --prefix /etc/#{canon_name} #{build_dir}/config_dir_sample`)
		
		# if apps_type == *_BOOT
		# - make the binary exec in ${apps_name}-bin
		# - add a ctrl script in ${apps_name}-ctrl
		# else (aka if apps_type == *_TEMP)
		# - make the binary exec in #{apps_name}
		if apps_type =~ /SYS_BOOT/
			fOut.puts("f 755 root root /usr/lib/#{canon_name}/#{apps_name}-bin #{build_dir}/#{apps_name}-bin-static")
			fOut.puts("f 755 root root /usr/lib/#{apps_name}-ctrl #{build_dir}/pkg_extrsc/#{apps_name}-ctrl.sh")
		elsif apps_type =~ /USR_BOOT/
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
			# - TODO to sort it out
			#   - apparently it would be /usr/share/xdg under fedora
			if pkg_type == "deb_install"
				dst_dirname	= "/usr/share/autostart"
			elsif pkg_type == "rpm_install"
				dst_dirname	= "/usr/xdg/autostart"
			else
				throw "this should never happen"
			end
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
			# if there are init.d files, install it		
			if pkg_type == "deb_install"
				# - update-rc.d is debian specific
				fOut << "update-rc.d #{apps_name} defaults\n"
			elsif pkg_type == "rpm_install"
				# - FEDORA ~= "chkconfig --add #{apps_name}"
				fOut << "chkconfig --add #{apps_name}\n"
			else
				throw "this should never happen"
			end
		end
		# start the application automatically in the postinstall
		if apps_type == "USR_BOOT"
			# if USR_BOOT and installed via sudo, launch it for SUDO_USER
			fOut << "if [ \"$$SUDO_USER\" != \"\" ]; then\n"
			fOut << "	su $$SUDO_USER -c \"#{apps_name}-ctrl start\"\n"
			fOut << "fi\n"
		elsif apps_type == "SYS_BOOT"
			# if SYS_BOOT, launch it as root now
			fOut << "/usr/lib/#{apps_name}-ctrl start\n"
		end
		fOut << "	exit 0\n"
		fOut << "EOF\n"			

		fOut << "%postremove <<EOF\n"
		fOut << "	#{postremove_hook}\n"
		if all_initd.size > 0 
			# if there are init.d files, remove it
			if pkg_type == "deb_install"
				# - update-rc.d is debian specific
				fOut << "	update-rc.d #{apps_name} remove\n"
			elsif pkg_type == "rpm_install"
				# - FEDORA ~= "chkconfig --del #{apps_name}"
				fOut << "	chkconfig --del #{apps_name}\n"
			else
				throw "this should never happen"
			end
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
	# - extract may be "dpkg -x ../neoip-webpack_0.0.1-200801150047_i386.deb  webpack"
	# - this produce a "dpkg-deb: unknown compression type `lzma'!"
	
	# about package size, on a webpack linux with -O0
	# - pure7z=1705165, tar.gz=3170879(with gzip -9), tar.7z=2294475(top 7z compression)
	# - .deb = 3162192
	
	# build the package itself
	puts("Doing 'sudo epm' to build the epm_package. if it asks for a password, give sudo password.")
	epm_format	= pkg_type.split('_')[0];
	system("sudo epm -n --output-dir . -f #{epm_format} #{apps_name} #{apps_name}.epm_list")

	# IF pkg_type=="deb_install" => epm put '-' between name/version, and version/arch
	# - just rename it the usual debian way
	if pkg_type == "deb_install"
		system("mv #{apps_name}-#{apps_version}.deb #{apps_name}_#{apps_version}_i386.deb")
	end
	
	# remove the configuration file
	FileUtils.rm_f "#{apps_name}.epm_list"
end

# mkpkg for deb_install
def apps_mkpkg_deb_install(pkg_type, apps_name)
	apps_mkpkg_epm_common(pkg_type, apps_name)
end

# mkpkg for deb_install
def apps_rmpkg_deb_install(pkg_type, apps_name)
	# remove the .deb for all version
	Dir.glob("#{apps_name}_*_i386.deb") { |filename| FileUtils.rm_f filename	}
end

# mkpkg for rpm_install
def apps_mkpkg_rpm_install(pkg_type, apps_name)
	apps_mkpkg_epm_common(pkg_type, apps_name)
end

# mkpkg for rpm_install
def apps_rmpkg_rpm_install(pkg_type, apps_name)
	# remove the .rpm for all version
	Dir.glob("#{apps_name}*.rpm") { |filename| FileUtils.rm_f filename	}
end

################################################################################
# mkpkg/rmpkg for ppa_install - assume the standalone directory is already built
################################################################################

# mkpkg for ppa_install
# - debuild  -uc -us -b <- from above debian/, build a binary package without signing it
# - debuild  -k'jerome etienne' -b <- from above debian/, build a source package and sign it with "jerome etienne" gpg key
# - dpkg-source -x neoip-get_0.0.1*.dsc <- "decompress" the source pacakge described in the .dsc
# - dput -U ppa:jerome-etienne/neoip #{apps_name}_#{apps_version}_source.changes <- upload a source package to a ppa
def apps_mkpkg_ppa_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	canon_name	= get_canon_name(apps_name)
	apps_version	= get_apps_version(pkg_type, apps_name)
	apps_summary	= get_apps_summary(apps_name)
	apps_description= get_apps_description(apps_name)
	apps_type	= get_apps_type(apps_name)
	
	# determine some path
	tmp_dir		= "/tmp/neoip_pkger_base_#{apps_name}_#{apps_version}"
	tmp_dir		= "/tmp/neoip_pkger_base_#{pkg_type}_#{apps_name}"
	debian_rootdir	= "#{tmp_dir}/debian_rootdir"
	debian_confdir	= "#{debian_rootdir}/debian"
	
	# build the directory
	FileUtils.mkdir_p(debian_rootdir)
	
	# do the initial dh_make
	cmdline	= "cd #{debian_rootdir} && (yes | DEBFULLNAME='Jerome Etienne' DEBEMAIL='jerome.etienne@gmail.com' dh_make -e jerome.etienne@gmail.com -p #{apps_name}_#{apps_version} -s -n)"
	system(cmdline)

	# do the dummy changelog
	# - "--force-bad-version" is needed as dh_make create the same 
	cmdline	= "cd #{debian_rootdir} && dch --newversion #{apps_version} --maintmaint --force-bad-version --distribution `lsb_release -c -s` Another build"
	system(cmdline)
	
	# determine all the templ_vars
	templ_vars	= {
		:APPS_NAME		=> apps_name,
		:APPS_TYPE		=> apps_type,
		:APPS_SUMMARY		=> apps_summary,
		:APPS_VERSION		=> apps_version,
		:APPS_DESCRIPTION	=> apps_description.join('\n')
	}
	# read the template
	patch_templ_file("ppa_pkg_extrsc/control.templ"	, "#{debian_confdir}/control"	, templ_vars)
	patch_templ_file("ppa_pkg_extrsc/Makefile.templ", "#{debian_rootdir}/Makefile"	, templ_vars)
	patch_templ_file("ppa_pkg_extrsc/rules.templ"	, "#{debian_rootdir}/rules"	, templ_vars)
	system("chmod +x #{debian_rootdir}/rules")

	# get external rescources
	all_manpage	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.[0-9]$/.match(x) }
	all_desktop	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.desktop$/.match(x) }
	all_initd	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.init.d$/.match(x) }
	# sanity check
	raise "only one man page is handled"	if all_manpage.length > 1
	raise ".desktop is NOT handled"		if all_desktop.length > 0
	raise "only one initd is handled"	if all_initd.length > 1
	# install manpage
	all_manpage.each{ |basename|
		src_fname	= "#{build_dir}/pkg_extrsc/#{basename}"
		dst_fname	= "#{debian_confdir}/#{basename}"
		FileUtils.cp(src_fname, dst_fname)
	}
	File.open("#{debian_confdir}/#{apps_name}.manpages", "w") { |f|
		content	= all_manpage.collect { |basename| "debian/#{basename}" }.join("\n")
		f.write(content)
	}
	# install initd
	all_initd.each	{ |basename|
		src_fname	= "#{build_dir}/pkg_extrsc/#{basename}"
		dst_fname	= "#{debian_confdir}/#{File.basename(src_fname).gsub(/\.init\.d/, ".init")}"
		FileUtils.cp(src_fname, dst_fname)
	}

	# copy the source in it
	# - this is space+time consuming but debuild -S use tar without the option to follow symlink
	# - i tried "export TAR_OPTIONS=--dereference" which is honored by GNU tar (tested)
	#   but fails with debuild... dunno why... maybe they use their own tar 
	cmdline	= "rsync -va --exclude paper --exclude .git ../../yavipin/ #{debian_rootdir}/yavipin"
	system(cmdline)

	# do a make clean - reduce the size of the source package, so the upload time
	system("cd #{debian_rootdir}/yavipin/build_linux && ./m clean")
	system("cd #{debian_rootdir}/yavipin/build_linux && rm -f *-bin-static")
	system("cd #{debian_rootdir}/yavipin/pkg && make clean")

	# do build the source package
	cmdline	= "cd #{debian_rootdir} && (yes | debuild -S -k'jerome etienne')"
	system(cmdline)
end

# mkpkg for ppa_install
def apps_rmpkg_ppa_install(pkg_type, apps_name)
	# get the data specific to this apps_name
	#apps_version	= get_apps_version(pkg_type, apps_name)
	# determine some path
	#tmp_dir	= "/tmp/neoip_pkger_base_#{apps_name}_#{apps_version}"
	tmp_dir		= "/tmp/neoip_pkger_base_#{pkg_type}_#{apps_name}"

	# remove the whole directory	
	FileUtils.rm_rf(tmp_dir)
end

################################################################################
# mkpkg/rmpkg for tgz_install - assume the standalone directory is already built
################################################################################

# mkpkg for tgz_install
# NOTE: used by ppa_install too (to build the installed directories structure)
def apps_mkpkg_tgz_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	canon_name	= get_canon_name(apps_name)
	apps_version	= get_apps_version(pkg_type, apps_name)
	apps_summary	= get_apps_summary(apps_name)
	apps_description= get_apps_description(apps_name)
	apps_type	= get_apps_type(apps_name)

	# TODO put an easy script to handle the start stop more easily ?
	# - would be nice but how to do that ? 
	# - this script should be called #{apps_name} in the build_dir
	#   - this is what firefox/azureus do
	# - a difference is that it is a pure deamon
	# - so the best would be to have something similar to the -ctrl script

	# TODO maybe another script install.sh and uninstall.sh
	# - which install/uninstall all the external resource like a .deb/.rpm

	tmp_dir		= "/tmp/neoip_pkger_base_#{pkg_type}_#{apps_name}"
	install_dir	= "#{tmp_dir}/#{apps_name}_#{apps_version}"
	
	# if apps_type == *_BOOT
	# - make the binary exec in ${apps_name}-bin
	# - add a ctrl script in ${apps_name}-ctrl
	# else (aka if apps_type == *_TEMP)
	# - make the binary exec in #{apps_name}
	if apps_type =~ /SYS_BOOT/
		FileUtils.mkdir_p("#{install_dir}/usr/lib/#{canon_name}")
		FileUtils.cp("#{build_dir}/#{apps_name}-bin-static"		, "#{install_dir}/usr/lib/#{canon_name}/#{apps_name}-bin")
		FileUtils.cp("#{build_dir}/pkg_extrsc/#{apps_name}-ctrl.sh"	, "#{install_dir}/usr/lib/#{apps_name}-ctrl")
	elsif apps_type =~ /USR_BOOT/
		FileUtils.mkdir_p("#{install_dir}/usr/lib/#{canon_name}")
		FileUtils.mkdir_p("#{install_dir}/usr/bin")
		FileUtils.cp("#{build_dir}/#{apps_name}-bin-static"		, "#{install_dir}/usr/lib/#{canon_name}/#{apps_name}-bin")
		FileUtils.cp("#{build_dir}/pkg_extrsc/#{apps_name}-ctrl.sh"	, "#{install_dir}/usr/bin/#{apps_name}-ctrl")
	else
		FileUtils.mkdir_p("#{install_dir}/usr/bin")
		FileUtils.cp("#{build_dir}/#{apps_name}-bin-static"		, "#{install_dir}/usr/bin/#{apps_name}")
	end

	# copy the config dir
	FileUtils.mkdir_p("#{install_dir}/etc/#{canon_name}")
	FileUtils.cp_r("#{build_dir}/config_dir_sample/.", "#{install_dir}/etc/#{canon_name}")
	
	# get external rescources
	all_manpage	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.[0-9]$/.match(x) }
	all_desktop	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.desktop$/.match(x) }
	all_initd	= Dir.entries("#{build_dir}/pkg_extrsc").delete_if { |x| not /.*\.init.d$/.match(x) }
	# install external rescources
	all_manpage.each{ |basename|
		src_fname	= "#{build_dir}/pkg_extrsc/#{basename}"
		man_section	= basename.match(/.([0-9])$/)[1]
		dst_fname	= "#{install_dir}/usr/share/man/man#{man_section}/#{basename}"
		FileUtils.mkdir_p(File.dirname(dst_fname))
		FileUtils.cp(src_fname, dst_fname)
	}
	all_desktop.each{ |basename|
		src_fname	= "#{build_dir}/pkg_extrsc/#{basename}"
		dst_fname	= "#{install_dir}/usr/share/application/#{basename}"
		FileUtils.mkdir_p(File.dirname(dst_fname))
		FileUtils.cp(src_fname, dst_fname)
	}
	all_initd.each	{ |basename|
		src_fname	= "#{build_dir}/pkg_extrsc/#{basename}"
		dst_fname	= "#{install_dir}/etc/init.d/#{File.basename(src_fname).gsub(/\.init\.d/, '')}"
		FileUtils.mkdir_p(File.dirname(dst_fname))
		FileUtils.cp(src_fname, dst_fname)
	}

	# build the .tgz
	cmdline	= "tar chzf #{Dir.pwd}/#{apps_name}_#{apps_version}_i386.tgz -C #{install_dir} ."
	system(cmdline);
	
	# remove the tmp_dir
	FileUtils.rm_rf "#{tmp_dir}"
end

# mkpkg for tgz_install
def apps_rmpkg_tgz_install(pkg_type, apps_name)
	# remove the installer .tgz for all version
	Dir.glob("#{apps_name}_*.tgz") { |filename| FileUtils.rm_f filename	}
end

################################################################################
# mkpkg/rmpkg for nsis_install - assume the standalone directory is already built
################################################################################

# mkpkg for nsis_install
def apps_mkpkg_nsis_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
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
def apps_rmpkg_nsis_install(pkg_type, apps_name)
	# remove the installer .exe for all version
	Dir.glob("#{apps_name}-*.exe") { |filename| FileUtils.rm_f filename	}
	# remove the nsis configuration file
	FileUtils.rm_f "#{apps_name}.nsi"
end

################################################################################
# mkpkg/rmpkg for macos_install - assume the standalone directory is already built
################################################################################

# mkpkg for macos_install
def apps_mkpkg_macos_install(pkg_type, apps_name)
	# get the data specific to this pkg_type/apps_name
	pkg_fext	= get_pkg_fext(pkg_type)
	build_dir	= get_build_dir(apps_name)
	apps_version	= get_apps_version(pkg_type, apps_name)
	
	# call iceberg to build the .pkg
	system("freeze #{build_dir}/#{apps_name}.packproj");
	
	# compute for variable
	volume_name	= "#{apps_name}"
	mount_point	= "/Volumes/#{volume_name}"
	tmpdmg_fname	= "#{build_dir}/#{apps_name}.#{pkg_fext}"
	reldmg_fname	= "#{apps_name}_#{apps_version}.#{pkg_fext}"
	
	# build and attach the disk image
	system("hdiutil create -size 32m -fs HFS+ -volname #{volume_name} #{tmpdmg_fname}")
	system("hdiutil attach #{tmpdmg_fname}")
	
	# copy the kad_bstrap directory	
	FileUtils.cp_r("#{build_dir}/build/#{apps_name}.pkg", "#{mount_point}");

	# detach the disk image	
	device_name	= `df | grep #{mount_point} | cut -d' ' -f 1`
	system("hdiutil detach #{device_name}")

	# convert the temporary disk image into a read-only one with compression
	# - i experimented a bit with UDBZ or -imagekey zlib-level=value to decrease the package size
	# - the gain was negligible (under 1%) so decided "it doesnt worth the complication"
	system("hdiutil convert #{tmpdmg_fname} -format UDZO -o #{reldmg_fname}");
end

# mkpkg for macos_install
def apps_rmpkg_macos_install(pkg_type, apps_name)
	# remove the package for all version
	Dir.glob("#{apps_name}_*.dmg") { |filename| FileUtils.rm_f filename	}
end

################################################################################
# upload the package - assume the package has been built
################################################################################

def apps_upload(pkg_type, apps_name)
	# log the event
	puts "upload #{apps_name} for #{pkg_type}"
	# forward to pkg_type-specific function
	send("apps_upload_#{pkg_type}", pkg_type, apps_name)
end

# upload for deb_install
def apps_upload_deb_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	build_target	= get_build_target(pkg_type)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	dest
	# upload to jmeserv
	scp_dest	= "dedixl.jetienne.com:public_html/download/#{build_target}"
	system("scp #{apps_name}_#{apps_version}_i386.deb #{scp_dest}")
end

# upload for rpm_install
def apps_upload_rpm_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	build_target	= get_build_target(pkg_type)	
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	# upload to jmeserv
	scp_dest	= "dedixl.jetienne.com:public_html/download/#{build_target}"
	system("scp #{apps_name}-#{apps_version}.rpm #{scp_dest}")
end

# upload for rpm_install
def apps_upload_ppa_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	
	# determine some path
	tmp_dir		= "/tmp/neoip_pkger_base_#{apps_name}_#{apps_version}"
	tmp_dir		= "/tmp/neoip_pkger_base_#{pkg_type}_#{apps_name}"

	# upload the source package
	cmdline		= "dput -U ppa:jerome-etienne/neoip #{tmp_dir}/#{apps_name}_#{apps_version}_source.changes"
	system(cmdline)
end

# upload for tgz_install
def apps_upload_tgz_install(pkg_type, apps_name)
	# get the build_dir
	build_dir	= get_build_dir(apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	# upload to jmeserv
	scp_dest	= "dedixl.jetienne.com:public_html/download/#{build_target}"
	system("scp #{apps_name}_#{apps_version}_i386.tgz #{scp_dest}")
end

# upload for nsis_install
def apps_upload_nsis_install(pkg_type, apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	# upload to http://urfastr.net/webpack/download
	scp_dest	= "dedixl.jetienne.com:public_html/download/#{build_target}"
	system("scp #{apps_name}-#{apps_version}.exe #{scp_dest}")
end

# upload for nsis_install
def apps_upload_macos_install(pkg_type, apps_name)
	# get the data specific to this apps_name
	apps_version	= get_apps_version(pkg_type, apps_name)
	build_target	= get_build_target(pkg_type)
	pkg_fext	= get_pkg_fext(pkg_type)
	# upload to http://urfastr.net/webpack/download
	scp_dest	= "dedixl.jetienne.com:public_html/download/#{build_target}"
	system("scp #{apps_name}_#{apps_version}.#{pkg_fext} #{scp_dest}")
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
