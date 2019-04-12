#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream>

#include <pcap.h>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

using namespace Magnum;


void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);


class Ebc: public Platform::Application {
    public:
        explicit Ebc(const Arguments& arguments);

    private:
        void drawEvent() override;
};

Ebc::Ebc(const Arguments& arguments): Platform::Application{arguments, Configuration{}.setTitle("EvenBetterCap")} {

    char errbuf[PCAP_ERRBUF_SIZE] = "N/A";

    pcap_if_t *ifaces;

    if(pcap_findalldevs(&ifaces, errbuf) == -1) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        std::exit(1);
    }

    Magnum::Utility::Arguments args;
    args.addArgument("filename").setHelp("filename","the pcap file to process")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setHelp("Displays the Ethernet Broadcast Domain in 3D.")
        .parse(arguments.argc, arguments.argv);

    pcap_t *handle = pcap_open_offline(args.value("filename").c_str(), errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Failed to create interface: %s\n", errbuf);
        std::exit(1);
    }

    int linktype = pcap_datalink(handle);
    std::cout << "Link type ";
    switch(linktype) {
        case DLT_EN10MB:
            std::cout << "Ether 802.3" << std::endl;
            break;
        case DLT_LINUX_SLL:
            std::cout << "Linux SLL" << std::endl;
            std::exit(1);
            break;
        case DLT_IEEE802_11:
            std::cout << "Wifi 802.11" << std::endl;
            std::exit(1);
            break;
        default:
            std::cout << "Type not handled" << std::endl;
            std::exit(1);
    }

    u_char b[9000];
    pcap_loop(handle, 10, packet_handler, b);

    pcap_close(handle);

}

void Ebc::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);



    swapBuffers();
}

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    printf("%s\n", user);
    fprintf(stdout, "packet %d %d %x\n", h->caplen, h->len, bytes[0]);
}

MAGNUM_APPLICATION_MAIN(Ebc)
