# This is a makefile on top of the neoip-pkger.rb
#
# - TODO it is mostly a stub right 
# - but it has "make clean"
# - TODO to doc too

all: deb_based
	
rpm_based:
	./neoip_pkger.rb rpm_install neoip-webpeer full

deb_based:
	./neoip_pkger.rb deb_install neoip-webpeer full

ppa_based:
	./neoip_pkger.rb ppa_install neoip-webpeer full

win32:
	./neoip_pkger.rb nsis_install neoip-webpeer full

macos:
	./neoip_pkger.rb macos_install neoip-webpeer full

clean:
	./neoip_pkger.rb all all clean
