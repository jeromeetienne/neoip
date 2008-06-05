Name: neoip-webpack
Version: 0.0.1
Release: 200806050733
License: 2007
Packager: root@jmehost2
Vendor: NeoIP
BuildRoot: /home/jerome/workspace/yavipin/pkg/./buildroot
%define _topdir /home/jerome/workspace/yavipin/pkg/.
Summary: neoip-webpack - bundle neoip-casti, neoip-casto and neoip-oload
Group: Applications
%pre
	[ "`pidof neoip-webpack-bin`" != "" ] && killall neoip-webpack-bin
	exit 0
%post
	
if [ "$SUDO_USER" != "" ]; then
	su $SUDO_USER -c "neoip-webpack-ctrl start"
fi
	exit 0
%preun
	[ "`pidof neoip-webpack-bin`" != "" ] && killall neoip-webpack-bin
	exit 0
%postun
	
	exit 0
%description 
sloaaaaaa one
sloaaaaaa two
sloaaaaaa three
sloaaaaaa four
%files
%attr(0755,root,root) %dir "/etc/neoip_webpack/casti"
%attr(0644,root,root) "/etc/neoip_webpack/casti/neoip_casti.conf"
%attr(0755,root,root) %dir "/etc/neoip_webpack/casto"
%attr(0644,root,root) "/etc/neoip_webpack/casto/neoip_casto.conf"
%attr(0755,root,root) %dir "/etc/neoip_webpack/kad_bstrap"
%attr(0644,root,root) "/etc/neoip_webpack/kad_bstrap/kad_realm_neoip_bt.fixfile"
%attr(0644,root,root) "/etc/neoip_webpack/kad_bstrap/kad_realm_ntudp.fixfile"
%attr(0755,root,root) "/etc/neoip_webpack/neoip-dnsresolve.sh"
%attr(0755,root,root) "/etc/neoip_webpack/neoip-ui-launcher.sh"
%attr(0755,root,root) "/etc/neoip_webpack/neoip_apps_ctrl.sh"
%attr(0644,root,root) "/etc/neoip_webpack/neoip_http.ico"
%attr(0644,root,root) "/etc/neoip_webpack/neoip_log.conf"
%attr(0644,root,root) "/etc/neoip_webpack/neoip_session.conf"
%attr(0755,root,root) %dir "/etc/neoip_webpack/oload"
%attr(0644,root,root) "/etc/neoip_webpack/oload/neoip_oload.conf"
%attr(0755,root,root) "/usr/bin/neoip-webpack-ctrl"
%attr(0755,root,root) "/usr/lib/neoip_webpack/neoip-webpack-bin"
%attr(0755,root,root) "/usr/local/man/man8/neoip-webpack.8"
%attr(0644,root,root) "/usr/xdg/autostart/neoip-webpack.desktop"
