# define installer name
outFile "neoip-oload-installer.exe"

# The default installation directory
InstallDir $PROGRAMFILES\neoip-oload

# Registry key to check for directory (so if you install again, it will 
# overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_neoipoload" "Install_Dir"


#--------------------------------
# Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


# default section start
section "neoip-oload (required)"

	SectionIn RO

	# define output path
	setOutPath $INSTDIR
 
	# specify file to go in output path
	file /r neoip_oload_tmpdir/config_dir
	file neoip_oload_tmpdir/neoip-oload-bin-static.exe
	# add the .dll - for unknown reason i cant compile them in static
	file neoip_oload_tmpdir/libglib-2.0-0.dll
	file neoip_oload_tmpdir/libiconv-2.dll
	file neoip_oload_tmpdir/libintl-3.dll
  
	# Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\NSIS_neoip-oload "Install_Dir" "$INSTDIR"
  
  
	# Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\neoip-oload" "DisplayName" "neoip-oload"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\neoip-oload" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\neoip-oload" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\neoip-oload" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
sectionEnd
 
# Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

	#CreateDirectory "$SMPROGRAMS\neoip-oload"
	#CreateShortCut "$SMPROGRAMS\neoip-oload\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
	# jme- removed this one to never launch neoip-oload from menu
	#CreateShortCut "$SMPROGRAMS\neoip-oload\neoip-oload (MakeNSISW).lnk" "$INSTDIR\neoip-oload.nsi" "" "$INSTDIR\neoip-oload.nsi" 0
  
SectionEnd


# Optional section (can be disabled by the user)
Section "Start automatically"

# A possibility is to use NSIS Simple Service Plugin
# - http://nsis.sourceforge.net/NSIS_Simple_Service_Plugin

#
# both are not tested.
# - left here for future references
	# stuff to register it as autostart for the current user
	#WriteRegStr HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "SomeName" "Command Line"
	# stuff to register it as autostart for the whole system
	#WriteRegStr HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "SomeName" "Command Line"

SectionEnd

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
	# Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\neoip-oload"
	DeleteRegKey HKLM SOFTWARE\NSIS_neoip-oload

	# Remove files and uninstaller
	Delete $INSTDIR\uninstall.exe

	# jme- this rmdir /r is very dangerous ?
	# - well the doc advised against using this
	rmdir /r "$INSTDIR\config_dir"
	delete	$INSTDIR\neoip-oload-bin-static.exe
	# remove the dll
	delete	$INSTDIR\libglib-2.0-0.dll
	delete	$INSTDIR\libiconv-2.dll
	delete	$INSTDIR\libintl-3.dll

	# Remove shortcuts, if any
	Delete "$SMPROGRAMS\neoip-oload\*.*"

	# Remove directories used
	rmdir	"$SMPROGRAMS\neoip-oload"
	rmdir	"$INSTDIR"


sectionEnd