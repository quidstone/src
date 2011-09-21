#!/usr/local/bin/python2.7
# end of new fragment overlaps old one

#     |>>>>>----|
# |--------|

# If the tail of the current framgent overlaps the beginning of an
# older fragment, cut the older fragment.
#                         m_adj(after->fe_m, aftercut);
# The older data becomes more suspect, and we essentially cause it
# to be dropped in the end, meaning it will come again.

import os
from addr import * 
from scapy.all import *

dstaddr=sys.argv[1]
pid=os.getpid()
payload="ABCDEFGHIJKLOMNO"
dummy="01234567"
packet=IPv6(src=SRC_OUT6, dst=dstaddr)/ICMPv6EchoRequest(id=pid, data=payload)
frag0=IPv6ExtHdrFragment(nh=58, id=pid, m=1)/str(packet)[40:56]
frag1=IPv6ExtHdrFragment(nh=58, id=pid, offset=1)/(dummy+str(packet)[56:64])
pkt0=IPv6(src=SRC_OUT6, dst=dstaddr)/frag0
pkt1=IPv6(src=SRC_OUT6, dst=dstaddr)/frag1
eth=[]
eth.append(Ether(src=SRC_MAC, dst=PF_MAC)/pkt1)
eth.append(Ether(src=SRC_MAC, dst=PF_MAC)/pkt0)

if os.fork() == 0:
	time.sleep(1)
	sendp(eth, iface=SRC_IF)
	os._exit(0)

ans=sniff(iface=SRC_IF, timeout=3, filter=
    "ip6 and src "+dstaddr+" and dst "+SRC_OUT6+" and icmp6")
a=ans[0]
if a and a.type == scapy.layers.dot11.ETHER_TYPES.IPv6 and \
    ipv6nh[a.payload.nh] == 'ICMPv6' and \
    icmp6types[a.payload.payload.type] == 'Echo Reply':
	id=a.payload.payload.id
	print "id=%#x" % (id)
	if id != pid:
		print "WRONG ECHO REPLY ID"
		exit(2)
	data=a.payload.payload.data
	print "payload=%s" % (data)
	if data == payload:
		exit(0)
	print "PAYLOAD!=%s" % (payload)
	exit(1)
print "NO ECHO REPLY"
exit(2)
