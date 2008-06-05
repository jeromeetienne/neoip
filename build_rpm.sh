#
# build the binary rpm for yavipin
#

# clean the source
#make -C src -f Makefile.static clean
# build the source tgz
(cd ..; tar cvzf /usr/src/redhat/SOURCES/neoip_kad_daemon.tgz yavipin)
# copy the spec file into rpm directory structure
cp neoip_kad_daemon.spec /usr/src/redhat/SPECS
#cp neoip_group.spec /usr/src/redhat/SPECS
# build the rpm binary itself
(cd /usr/src/redhat/SPECS; rpmbuild -v -bb neoip_kad_daemon.spec)
#(cd /usr/src/redhat/SPECS; rpmbuild -v -bb neoip_group.spec)
# remove the source tgz from the rpm directory structure
rm /usr/src/redhat/SOURCES/neoip_kad_daemon.tgz
#
# remove the source from the BUILD directory
# - unclear why rpmbuild doesnt do it for me but $RPM_BUILD_ROOT doesnt seem to 
#   work in %clean when i tried
#
rm -rf /usr/src/redhat/BUILD/yavipin
# move built rpm from the rpm directory structure to local directory
mv /usr/src/redhat/RPMS/i386/neoip_*.rpm .
