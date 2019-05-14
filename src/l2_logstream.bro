module Monopticon;

export {
    redef enum Log::ID += { LOG };

    type ArpEvent: record {
        typ: string &log;
        tpa: addr &log;
    };

    type RawPktEvent: record {
        l3_proto: layer3_proto &log;
        src: string &optional &log;
        dst: string &optional &log;
    };
}

event raw_packet(p: raw_pkt_hdr)
{
    local r: RawPktEvent = [$src=p$l2$src, $dst=p$l2$dst, $l3_proto=p$l2$proto];
    Log::write(Monopticon::LOG, r);
}

event arp_reply(mac_src: string, mac_dst: string, SPA: addr, SHA: string, TPA: addr, THA: string)
{
    print "req", mac_dst;
    #Log::write(Monopticon::ARPLOG, p);
}

event arp_response(mac_src: string, mac_dst: string, SPA: addr, SHA: string, TPA: addr, THA: string)
{
    print "resp", mac_dst;
    #Log::write(Monopticon::ARPLOG, p);
}


event zeek_init()
{
    Log::create_stream(LOG, [$columns=RawPktEvent, $path="all-packets"]);
    #Log::create_stream(LOG, [$columns=arp_event, $path="arp-event"]);
}
