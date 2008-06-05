#! /usr/bin/env python
# jme - discover the public ip address using checkip.dyndns.org

import re
import urllib2

# get the page from dyndns.org
# - chosen as it is expected to remain valid for a middle term
html_page=urllib2.urlopen('http://checkip.dyndns.org/').read()

# remove some html at the head
tmp=re.sub('.*IP Address: ', '', html_page)

# get the public ip address
public_ip_addr=re.match('\d+\.\d+\.\d+\.\d+', tmp).group(0)

# display the result
print public_ip_addr
