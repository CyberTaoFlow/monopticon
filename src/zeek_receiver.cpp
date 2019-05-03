#include "broker/broker.hh"
#include "broker/message.hh"
#include "broker/bro.hh"
#include <string>
#include <vector>

// Requires:
// export LD_LIBRARY_PATH=/usr/local/lib
// g++ -std=c++11 -lbroker -lcaf_core -lcaf_io -lcaf_openssl -o rec_test zeek_receiver.cpp
using namespace broker;

// Demo receiver that is subscribed to the topic "demo/simple"
int main() {
    endpoint ep;
    auto subscriber = ep.make_subscriber( {"demo/simple"} );
    ep.listen("127.0.0.1", 9999);
    std::cout << "Started peer listener" << std::endl;

    while(1) {
        broker::data_message msg = subscriber.get();
        auto topic = broker::get_topic(msg);
        auto msg_body = broker::get_data(msg);

        //decltype(msg_body)::foo = 1;
        auto parent_content = broker::get_if<broker::vector>(msg_body);
        auto content = broker::get_if<broker::vector>(parent_content->at(2));
        auto raw_pkt = broker::get_if<broker::vector>(content->at(1));
        auto raw_pkt_hdr = broker::get_if<broker::vector>(raw_pkt->at(0));
        auto l2_pkt_hdr = broker::get_if<broker::vector>(raw_pkt_hdr->at(0));
        auto mac_src = l2_pkt_hdr->at(3);
        auto mac_dst = l2_pkt_hdr->at(4);

        std::cout << "received a message: " << topic << " $ " << mac_src << " $ " << mac_dst << std::endl;
    }

    return 0;
}
