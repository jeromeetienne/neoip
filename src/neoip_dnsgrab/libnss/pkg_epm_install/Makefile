# very raw Makefile to build libnss_neoip package via epm

package:
	make -C .. libnss_neoip.so.2
	sudo epm -n --output-dir . -f deb libnss-neoip libnss_neoip.epm_list
	mv libnss-neoip-0.1.deb libnss-neoip_0.1_386.deb

clean:
	rm -f libnss-neoip_*.deb
