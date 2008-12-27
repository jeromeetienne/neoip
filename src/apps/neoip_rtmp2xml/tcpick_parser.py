#!/usr/bin/python
# this script reads ascii data from tcpick on a rtmp connection
# and uses neoip-rtmp2xml to parse rtmp.
# - it aims to ease rtmp study

# 1. record the communication in a text file
#    $ sudo tcpick -i lo -h -yH -t "port 1935" >/tmp/output.txt
# 2. put the recorded communication in this filter
#    $ cat /tmp/output.txt | ./tcpick_parser.py | less

import sys
import re, binascii
import os, fcntl
import time

# skip the first 3 lines
for i in xrange(1, 4):
    sys.stdin.readline()

def parse_hdline(line):
    """parse the header line from tcpick"""
    fields  = line.split(' ')
    if len(fields) != 7:
    	return None
    result  = {}
    result['timestamp'] = fields[0]
    result['src']       = fields[2]
    result['pkt_flag']  = fields[3]
    result['dst']       = fields[5]
    result['bodysize']  = int(re.search("\((\d+)", fields[6]).group(1))
    return result

def parse_bdline(line):
    """parse a data line from tcpick"""
    line    = line.replace(' ', '').replace("\n", '')
    result  = binascii.unhexlify(line)
    return result

def cmdfd_setup():
    """setup the cmdfd"""
    cmdline = "./neoip-rtmp2xml -c ../src/config_dir_debug1"
    # create the popen and zero the buffer
    # - important to keep pkthd and neoip-rtmp2xml data
    i, o, e = os.popen3(cmdline, 0)
    # pass child.stdout in nonblocking
    flags   = fcntl.fcntl(o.fileno(), fcntl.F_GETFL);
    fcntl.fcntl(o.fileno(), fcntl.F_SETFL, flags | os.O_NONBLOCK)
    # return all fd
    return { 'i': i, 'o': o, 'e': e }

def cmdfd_push(ctx, prefix, body):
    """push data to cmdfd and display neoip-rtmp2xml output"""
    # compute the length to delete for rtmp handshake    
    len2del = min(ctx['hslen'], len(body))
    # remove the handshake data if needed
    if len2del > 0:
        ctx['hslen']-= len2del
        body         = body[len2del:]
    if len(body) == 0: return
    
    # just to debug
    #print "push to " + prefix
    #sys.stdout.write(body)
    #return

    # send the data to the cmd
    ctx['i'].write(body)
    ctx['i'].flush()
    # try to read data from the cmd
    try:
        recv_data = ctx['o'].read()
    except IOError, error:
        # if errno is not due to nonblock, rethrow it
        if error.errno != os.errno.EAGAIN:   raise
        return
    # display the data
    print prefix
    sys.stdout.write( recv_data ) 

# setup the pipe
i2r_ctx = cmdfd_setup()
i2r_ctx['hslen']    = 1536*2+1
r2i_ctx = cmdfd_setup()
r2i_ctx['hslen']    = 1536*2+1

# loop and read stdin line from tcpick
while True:
    # read the head line
    hdline  = sys.stdin.readline()
    # if hdline is empty or start with 'tcpick', the parsing is over
    if hdline == "" or hdline.startswith('tcpick:'):    break
    # parse the head line
    head    = parse_hdline(hdline);
    if head == None:
        continue
    # if bodysize == 0, no bodyline is provided
    if head['bodysize'] == 0:
        continue
    # read the body line
    bdline  = sys.stdin.readline()
    # parse the body line
    body    = parse_bdline(bdline)
    #print head
    #print len(body)
    if head['dst'] == "127.0.0.1:1935":
        cmdfd_push(i2r_ctx, "%s ITOR >>-->>-->> RESP" % head['timestamp'], body)
    else:
        cmdfd_push(r2i_ctx, "%s ITOR <<--<<--<< RESP" % head['timestamp'], body)
    # this sleep is to let time to neoip-rtmp2xml to answer
    # - without it data are buffered in neoip-rtmp2xml and so
    #   the synchronicity of the xml output is delivered
    #   long after the input. There is a zerotimer_t
    # - this messup the order between i2r and r2i data
    time.sleep(1)