%{!?_dist: %{expand: %%define dist fc2}}

#
# Preamble
#
Summary: Utility to handle NeoIP group inside scripts
Name: 	 neoip_group
Version: 0.5
Release: %{dist}
License: Proprietary
Source: neoip_group.tgz
Group: 	Applications/Internet
URL: 	http://off.net/~jme/neoip

%description
neoip_group is an utility which create NeoIP group and provide (i) presence, (ii) multicast communication and
(iii) leader election.

%prep
rm -rf $RPM_BUILD_DIR/neoip_router
zcat $RPM_SOURCE_DIR/neoip_router.tgz | tar -xvf -

%build
make -C yavipin/src -f Makefile.static all

%install
make -C yavipin/src -f Makefile.static install_group

%clean
make -C yavipin/src -f Makefile.static uninstall_group

%files
/usr/lib/neoip_group/neoip_group-bin
/usr/bin/neoip_group
/etc/neoip_group/neoip_lib.ini
/etc/neoip_group/neoip_lib_default.ini
/etc/neoip_group/neoip_group.ini
/etc/neoip_group/neoip_group_default.ini
/etc/neoip_group/root_ca_cert.pem
/usr/man/man8/neoip_group.8

%changelog
* Wed Feb 09 2004 jme
- initial rpm release
