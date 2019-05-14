#include "broker/broker.hh"
#include "broker/bro.hh"

#include <unistd.h>
#include <future>

#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>

using namespace broker;

boost::lockfree::queue<int> queue(256);

// demo receiver that is subscribed to the topic "demo/threaded-simple"
void broker_receiver() {
    endpoint ep;
    auto subscriber = ep.make_subscriber( {"demo/simple"} );
    ep.listen("127.0.0.1", 9999);

    while(true) {
        // block the current thread until a message gets available
        // we could also use select, see the threaded python example
        auto msg = subscriber.get();

        broker::topic topic = get_topic(msg);
        bro::Event response = get_data(msg);
        std::cout << "received on topic: " << topic << "     response event name: " << response.name() << "    content: " << response.args() << std::endl;
        // TODO insert into Boost queue here
    }
}

int main() {

    boost::thread_group producer_threads, consumer_threads;
    for (int i = 0; i != producer_thread

    // TODO try to read from Boost Queue here
    while(true) {
        std::cout << "main loop can do stuff \\o/" << std::endl;
        sleep(3);
    }

    return 0;
}
