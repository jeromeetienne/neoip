%{!?_dist: %{expand: %%define dist fc2}}

#
# Preamble
#
Summary: Deamon which provides ip end-to-end and security (aka no nat + kind of ipsec)
Name: 	 neoip_router
Version: 0.9
Release: %{dist}
License: Proprietary
Source: neoip_router.tgz
Group: 	Applications/Internet
URL: 	http://off.net/~jme/neoip

%description
NeoIP router restores IP end-to-end and provides ip security.
A more simple and understandable version, the users of NeoIp router
no more have to care about NAT and about configuring VPN.

%prep
rm -rf $RPM_BUILD_DIR/neoip_router
zcat $RPM_SOURCE_DIR/neoip_router.tgz | tar -xvf -

%build
make -C yavipin/src -f Makefile.static all

%install
make -C yavipin/src -f Makefile.static install_router

%clean
make -C yavipin/src -f Makefile.static uninstall_router

%files
/usr/lib/neoip_router/neoip_router-bin
/usr/lib/neoip_router/neoip_router_configurator.sh
/usr/bin/neoip_router
/usr/bin/neoip_router_tui
/usr/bin/neoip_router_gui
/etc/init.d/neoip_router
/usr/man/man8/neoip_router.8
/etc/neoip_router/neoip_lib.ini
/etc/neoip_router/neoip_lib_default.ini
/etc/neoip_router/neoip_router.ini
/etc/neoip_router/neoip_router_default.ini
/etc/neoip_router/root_ca_cert.pem
/etc/neoip_router/neoip.ico
/usr/share/applications/neoip_router.desktop
/usr/share/pixmaps/neoip_router.png

%post
/sbin/chkconfig --add neoip_router
#service neoip_router start
                                                                                
%preun
if [ $1 = 0 ]; then
		service neoip_router stop
        /sbin/chkconfig --del neoip_router
fi


%changelog
* Wed Feb 09 2004 jme
- initial rpm release
