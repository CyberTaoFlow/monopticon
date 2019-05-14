event raw_packet(p: raw_pkt_hdr) 
{
    print p;
}

event arp_reply(mac_src: string, mac_dst: string, SPA: addr, SHA: string, TPA: addr, THA: string)
{
    print "req", mac_dst;
}

event arp_response(mac_src: string, mac_dst: string, SPA: addr, SHA: string, TPA: addr, THA: string)
{
    print "resp", mac_dst;
}

event bad_arp(SPA: addr, SHA: string, TPA: addr, THA: string, Explanation: string)
{
    print "bad arp", Explanation;
}

event new_connection(conn: connection)
{
    print "new conn", connection;
}
