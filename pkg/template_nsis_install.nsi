# define installer name
outFile "@APPS_NAME@-@APPS_VERSION@.exe"

# The default installation directory
InstallDir $PROGRAMFILES\@APPS_NAME@

# Registry key to check for directory (so if you install again, it will 
# overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_@APPS_NAME@ " "Install_Dir"

# to compress the package in lzma - very efficient
SetCompressor lzma

#--------------------------------
# Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


# default section start
section "@APPS_NAME@ (required)"

	SectionIn RO

	# define output path
	setOutPath $INSTDIR
 
	# specify file to go in output path
	file /r @APPS_NAME@.build_dir/config_dir_sample
	file @APPS_NAME@.build_dir/@APPS_NAME@-bin-static.exe
	# add those .dll - for unknown reason i cant compile them in static
	file @APPS_NAME@.build_dir/libglib-2.0-0.dll
	file @APPS_NAME@.build_dir/libiconv-2.dll
	file @APPS_NAME@.build_dir/libintl-3.dll

!ifdef NEOIP_DAEMON
	file @APPS_NAME@.build_dir/pkg_extrsc/@APPS_NAME@-ctrl.bat
!endif

  
	# Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\NSIS_@APPS_NAME@ "Install_Dir" "$INSTDIR"
  
	# Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\@APPS_NAME@" "DisplayName" "@APPS_NAME@"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\@APPS_NAME@" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\@APPS_NAME@" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\@APPS_NAME@" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
sectionEnd
 
# Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

	CreateDirectory "$SMPROGRAMS\@APPS_NAME@"

# TODO this doesnt works for an unknown reason
# - win32 menu do not find the -ctrl.bat
# - likely an issue with the quote
#!ifdef NEOIP_DAEMON
#	# make a menu entry to start/stop the daemon
#	CreateShortCut "$SMPROGRAMS\@APPS_NAME@\Start.lnk" '"$INSTDIR\@APPS_NAME@-ctrl.bat" start'
#	CreateShortCut "$SMPROGRAMS\@APPS_NAME@\Stop.lnk" '"$INSTDIR\@APPS_NAME@-ctrl.bat" stop'
#!endif

	# make a menu entry for uninstall
	CreateShortCut "$SMPROGRAMS\@APPS_NAME@\UnInstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0  
SectionEnd


!ifdef NEOIP_DAEMON
# Optional section (can be disabled by the user)
Section "Start automatically"
	# to start automatically, a window service is installed
	# - it uses the NSIS Simple Service Plugin
	# - see http://nsis.sourceforge.net/NSIS_Simple_Service_Plugin
	
	# TODO what about testing the result of all those SimpleSC command ?
	# - would allow to notify the user of a previously installed version

	# install the service
	SimpleSC::InstallService "@APPS_NAME@" "@APPS_NAME@" 2 '"$INSTDIR\@APPS_NAME@-bin-static.exe" -c "$INSTDIR\config_dir_sample"' "Tcpip" "" ""

	# start the service
	SimpleSC::StartService "@APPS_NAME@"

SectionEnd
!endif

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
	# Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\@APPS_NAME@"
	DeleteRegKey HKLM SOFTWARE\NSIS_@APPS_NAME@

!ifdef NEOIP_DAEMON
	# stop the service
	SimpleSC::StopService "@APPS_NAME@"
	# remove the service
	SimpleSC::RemoveService @APPS_NAME@
!endif
	# Remove files and uninstaller
	Delete $INSTDIR\uninstall.exe

	# jme- this rmdir /r is very dangerous ?
	# - well the doc advised against using this
	# - on the other hand, this will work IIF config_dir_sample exists, so unlikely to
	#   delete anything if the user put a weirdo $INSTDIR on uninstall
	rmdir /r "$INSTDIR\config_dir_sample"
	delete	$INSTDIR\@APPS_NAME@-bin-static.exe
	# remove the dll
	delete	$INSTDIR\libglib-2.0-0.dll
	delete	$INSTDIR\libiconv-2.dll
	delete	$INSTDIR\libintl-3.dll
!ifdef NEOIP_DAEMON
	delete	$INSTDIR\@APPS_NAME@-ctrl.bat
!endif

	# Remove shortcuts, if any
	Delete "$SMPROGRAMS\@APPS_NAME@\*.*"

	# Remove directories used
	rmdir	"$SMPROGRAMS\@APPS_NAME@"
	rmdir	"$INSTDIR"


sectionEnd