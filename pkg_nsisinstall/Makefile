# This is a makefile to build all the one-click installer for the win32 apps
# - this is done by nsis from http://nsis.sourceforge.net/Main_Page
# - it run on linux
# - TODO currently it doesnt handle the glib in dll

NEOIP_SRC=../src

all: oload_mkdir oload_mkpkg

clean:	oload_clean

################################################################################
#			neoip-oload
################################################################################

# build the package
oload_mkpkg:
	i586-mingw32msvc-strip	neoip_oload_tmpdir/neoip-oload-bin-static.exe
	makensis neoip-oload.nsi

# create the directory needed for the package - MUST be done before the package
oload_mkdir:
	mkdir neoip_oload_tmpdir
	mkdir neoip_oload_tmpdir/config_dir
	mkdir neoip_oload_tmpdir/config_dir/tmp
	mkdir neoip_oload_tmpdir/config_dir/cache
	mkdir neoip_oload_tmpdir/config_dir/kad_bstrap
	ln -s /home/samba_share/libglib-2.0-0.dll	neoip_oload_tmpdir
	ln -s /home/samba_share/libiconv-2.dll		neoip_oload_tmpdir
	ln -s /home/samba_share/libintl-3.dll		neoip_oload_tmpdir
	ln -s ../$(NEOIP_SRC)/neoip-oload-bin-static.exe neoip_oload_tmpdir
	ln -s ../../$(NEOIP_SRC)/config_dir_orig/oload neoip_oload_tmpdir/config_dir
	cd neoip_oload_tmpdir/config_dir && 					\
		find ../../$(NEOIP_SRC)/config_dir_orig/ -maxdepth 1 -type f 	\
		| xargs --max-args=1 ln -s

# clean the direcotry needed for the package
oload_clean:
	find neoip_oload_tmpdir/ -type l | xargs rm
	rmdir neoip_oload_tmpdir/config_dir/tmp
	rmdir neoip_oload_tmpdir/config_dir/cache
	rmdir neoip_oload_tmpdir/config_dir/kad_bstrap
	rmdir neoip_oload_tmpdir/config_dir/
	rmdir neoip_oload_tmpdir
	rm neoip-oload-installer.exe