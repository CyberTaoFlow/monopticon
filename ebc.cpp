#include <cstring>
#include <stdio.h>
#include <iostream>

#include <file.h>
#include <cfile.h>
#include <pcap.h>
#include <epan/epan.h>
#include <epan/addr_resolv.h>
#include <capchild/capture_session.h>
#include <capchild/capture_sync.h>
#include <capture_info.h>

#include <ui/ws_ui_util.h>
#include <ui/capture.h>


#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

void pipe_input_set_handler(gint source, gpointer user_data, ws_process_id *child_process, pipe_input_cb_t input_cb);
void capture_input_new_packets(capture_session *cap_session, int to_read);
void capture_input_error_message(capture_session *cap_session _U_, char *error_msg, char *secondary_error_msg);
gboolean capture_input_new_file(capture_session *cap_session, gchar *new_file);
void capture_input_drops(capture_session *cap_session _U_, guint32 dropped);
void capture_input_closed(capture_session *cap_session, gchar *msg);
void capture_input_cfilter_error_message(capture_session *cap_session, guint i, char *error_message);

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);
cf_status_t cf_open(capture_file *cf, const char *fname, unsigned int type, gboolean is_tempfile, int *err);

static const nstime_t *
ret_null(struct packet_provider_data *prov, guint32 frame_num)
{
    return NULL;
}

static epan_t* ebc_epan_new(capture_file *cf) {
   static const struct packet_provider_funcs funcs = {
    ret_null, //tshark_get_frame_ts,
    cap_file_provider_get_interface_name,
    cap_file_provider_get_interface_description,
    NULL,
  };

  return epan_new(&cf->provider, &funcs);

}

capture_file cfile;

cf_status_t cf_open(capture_file *cf, const char *fname, unsigned int type, gboolean is_tempfile, int *err)
{
  wtap  *wth;
  gchar *err_info;

  bool perform_two_pass_analysis = true;
  wth = wtap_open_offline(fname, type, err, &err_info, perform_two_pass_analysis);
  if (wth == NULL)
    goto fail;

  /* The open succeeded.  Fill in the information for this file. */

  /* Create new epan session for dissection. */
  epan_free(cf->epan);
  cf->epan = ebc_epan_new(cf);

  cf->provider.wth = wth;
  cf->f_datalen = 0; /* not used, but set it anyway */

  /* Set the file name because we need it to set the follow stream filter.
     XXX - is that still true?  We need it for other reasons, though,
     in any case. */
  cf->filename = g_strdup(fname);

  /* Indicate whether it's a permanent or temporary file. */
  cf->is_tempfile = is_tempfile;

  /* No user changes yet. */
  cf->unsaved_changes = FALSE;

  cf->cd_t      = wtap_file_type_subtype(cf->provider.wth);
  cf->open_type = type;
  cf->count     = 0;
  cf->drops_known = FALSE;
  cf->drops     = 0;
  cf->snap      = wtap_snapshot_length(cf->provider.wth);
  nstime_set_zero(&cf->elapsed_time);
  cf->provider.ref = NULL;
  cf->provider.prev_dis = NULL;
  cf->provider.prev_cap = NULL;

  cf->state = FILE_READ_IN_PROGRESS;

  wtap_set_cb_new_ipv4(cf->provider.wth, add_ipv4_name);

  return CF_OK;

fail:
  std::cout << "Capture File open failed" << std::endl;
  return CF_ERROR;
}



namespace Magnum {

class Ebc: public Magnum::Platform::Application {
    public:
        explicit Ebc(const Arguments& arguments);

    private:
        void drawEvent() override;
};

Ebc::Ebc(const Arguments& arguments): Magnum::Platform::Application{arguments, Configuration{}.setTitle("EvenBetterCap")} {

    std::cout << "hello, world!" << std::endl;

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

    std::cout << args.value("filename") << std::endl;

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

    wtap_init(FALSE);

    if (!epan_init(register_all_protocols, register_all_protocol_handoffs, NULL, NULL)) {
        std::cout << "Epan init failed" << std::endl;
        std::exit(1);
    }

    epan_cleanup();
    wtap_cleanup();
    pcap_close(handle);

}

void Ebc::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    swapBuffers();
}


}


void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    printf("%s\n", user);
    fprintf(stdout, "packet %d %d %x\n", h->caplen, h->len, bytes[0]);
}

void
pipe_input_set_handler(gint source, gpointer user_data, ws_process_id *child_process, pipe_input_cb_t input_cb)
{
    return;
}

/* capture child tells us we have new packets to read */
void capture_input_new_packets(capture_session *cap_session, int to_read)
{
    return;
}

void capture_input_error_message(capture_session *cap_session _U_, char *error_msg, char *secondary_error_msg)
{
    return;
}

/* capture child tells us we have a new (or the first) capture file */
gboolean capture_input_new_file(capture_session *cap_session, gchar *new_file)
{
    return TRUE;
}


void capture_input_drops(capture_session *cap_session _U_, guint32 dropped) {
    return;
}

void capture_input_closed(capture_session *cap_session, gchar *msg) {
    return;
}

void capture_input_cfilter_error_message(capture_session *cap_session, guint i, char *error_message) {
    return;
}

MAGNUM_APPLICATION_MAIN(Magnum::Ebc)
