#include <cstring>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

#include "file.h"
#include "cfile.h"
#include <capture_info.h>
#include <capture_opts.h>
#include <capchild/capture_sync.h>
#include <capchild/capture_session.h>
#include <caputils/capture-pcap-util.h>
#include <epan/addr_resolv.h>
#include <epan/conversation_table.h>
#include <epan/epan.h>
#include <epan/funnel.h>
#include <epan/prefs.h>
#include <epan/print_stream.h>
#include <epan/register.h>
#include <epan/rtd_table.h>
#include <epan/srt_table.h>
#include <epan/stat_tap_ui.h>
#include <epan/timestamp.h>
#include "pcap.h"
#include "log.h"
#include "extcap.h"
#include <ui/capture.h>
#include <ui/ws_ui_util.h>
#include <ui/taps.h>
#include <ui/cli/tshark-tap.h>
#include <ui/filter_files.h>
#include <ui/dissect_opts.h>
#include <version_info.h>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

/* Exit codes */
#define INIT_FAILED 2
#define INVALID_FILE 2
#define INVALID_OPTION 2

capture_file cfile;

static capture_options global_capture_opts;
static capture_session global_capture_session;

void pipe_input_set_handler(gint source, gpointer user_data, ws_process_id *child_process, pipe_input_cb_t input_cb);
void capture_input_new_packets(capture_session *cap_session, int to_read);
void capture_input_error_message(capture_session *cap_session _U_, char *error_msg, char *secondary_error_msg);
gboolean capture_input_new_file(capture_session *cap_session, gchar *new_file);
void capture_input_drops(capture_session *cap_session _U_, guint32 dropped);
void capture_input_closed(capture_session *cap_session, gchar *msg);
void capture_input_cfilter_error_message(capture_session *cap_session, guint i, char *error_message);

void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);
cf_status_t cf_open(capture_file *cf, const char *fname, unsigned int type, gboolean is_tempfile, int *err);

static const nstime_t* ret_null(struct packet_provider_data *prov, guint32 frame_num)
{
    return NULL;
}

static epan_t* ebc_epan_new(capture_file *cf)
{
   static const struct packet_provider_funcs funcs = {
    ret_null, //tshark_get_frame_ts,
    cap_file_provider_get_interface_name,
    cap_file_provider_get_interface_description,
    NULL,
  };

  return epan_new(&cf->provider, &funcs);

}

cf_status_t cf_open(capture_file *cf, const char *fname, unsigned int type, gboolean is_tempfile, int *err)
{
  wtap  *wth;
  gchar *err_info;

  bool perform_two_pass_analysis = false;
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

// NOTE robbed from capture_sync.c
/*
void capture_session_init(capture_session *cap_session, capture_file *cf)
{
    cap_session->cf                              = cf;
    cap_session->fork_child                      = WS_INVALID_PID;
    cap_session->owner                           = getuid();
    cap_session->group                           = getgid();
    cap_session->state                           = CAPTURE_STOPPED;
    cap_session->count                           = 0;
    cap_session->session_started                 = FALSE;
}
*/


static void
tshark_log_handler (const gchar *log_domain, GLogLevelFlags log_level,
    const gchar *message, gpointer user_data)
{

  if ((log_level & (G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_INFO))) {
    const gchar *domains = g_getenv("G_MESSAGES_DEBUG");
    if (domains == NULL ||
       (strcmp(domains, "all") != 0 && !strstr(domains, log_domain))) {
      return;
    }
  }

  g_log_default_handler(log_domain, log_level, message, user_data);
}



static void
get_tshark_compiled_version_info(GString *str)
{
  /* Capture libraries */
  get_compiled_caplibs_version(str);

}

namespace Magnum {

class Ebc: public Platform::Application
{
    public:
        explicit Ebc(const Arguments& arguments);

    private:
        void drawEvent() override;
};

Ebc::Ebc(const Arguments& arguments): Platform::Application{arguments, Configuration{}.setTitle("EvenBetterCap")} {

    volatile int exit_status = EXIT_SUCCESS;

    e_prefs             *prefs_p;
    gboolean prefs_loaded = FALSE;
    print_format_e print_format = PR_FMT_TEXT;
    print_stream_t* print_stream = NULL;
    const char* delimiter_char;

    const char *output_file_name;
    output_fields_t* output_fields;
    const gchar* cf_name;
    volatile int in_file_type = WTAP_TYPE_AUTO;
    int err;

    Magnum::Utility::Arguments args;
    args.addArgument("filename").setHelp("filename","the pcap file to process")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setHelp("Displays the Ethernet Broadcast Domain in 3D.")
        .parse(arguments.argc, arguments.argv);

    std::cout << "hello, world!" << std::endl;

    initialize_funnel_ops();

    /* Get the compile-time version information string */
    GString* comp_info_str = get_compiled_version_info(get_tshark_compiled_version_info,
                                            epan_get_compiled_version_info);


    /* Get the run-time version information string */
    GString* runtime_info_str = get_runtime_version_info(get_runtime_caplibs_version);

    gchar * v = "evenbettercap";

    show_version(v,comp_info_str, runtime_info_str);

    g_string_free(comp_info_str, TRUE);
    g_string_free(runtime_info_str, TRUE);

    int log_flags =
                    G_LOG_LEVEL_ERROR|
                    G_LOG_LEVEL_CRITICAL|
                    G_LOG_LEVEL_WARNING|
                    G_LOG_LEVEL_MESSAGE|
                    G_LOG_LEVEL_INFO|
                    G_LOG_LEVEL_DEBUG|
                    G_LOG_FLAG_FATAL|G_LOG_FLAG_RECURSION;

    g_log_set_handler(NULL,
                    (GLogLevelFlags)log_flags,
                    tshark_log_handler, NULL /* user_data */);
    g_log_set_handler(LOG_DOMAIN_MAIN,
                    (GLogLevelFlags)log_flags,
                    tshark_log_handler, NULL /* user_data */);

    g_log_set_handler(LOG_DOMAIN_CAPTURE,
                    (GLogLevelFlags)log_flags,
                    tshark_log_handler, NULL /* user_data */);
    g_log_set_handler(LOG_DOMAIN_CAPTURE_CHILD,
                    (GLogLevelFlags)log_flags,
                    tshark_log_handler, NULL /* user_data */);

    capture_opts_init(&global_capture_opts);
    //capture_session_init(&global_capture_session, &cfile);

    timestamp_set_type(TS_RELATIVE);
    timestamp_set_precision(TS_PREC_AUTO);
    timestamp_set_seconds_type(TS_SECONDS_DEFAULT);

    std::cout << "YYYY" << std::endl;
    wtap_init(TRUE);
    std::cout << "ZZZZ" << std::endl;
    if (!epan_init(register_all_protocols, register_all_protocol_handoffs, NULL,
                   NULL)) {

      exit_status = INIT_FAILED;
      goto clean_exit;
    }
    std::cout << "XXXX" << std::endl;

    // Register all tap listeners.
    extcap_register_preferences();
    for (tap_reg_t *t = tap_reg_listener; t->cb_func != NULL; t++) {
      t->cb_func();
    }

    /*
    conversation_table_set_gui_info(init_iousers);
    hostlist_table_set_gui_info(init_hostlists);
    srt_table_iterate_tables(register_srt_tables, NULL);
    rtd_table_iterate_tables(register_rtd_tables, NULL);
    stat_tap_iterate_tables(register_simple_stat_tables, NULL);
    */

    std::cout << "hello third world!" << std::endl;

    /* Load libwireshark settings from the current profile. */
    prefs_p = epan_load_settings();
    //write_prefs(NULL);
    prefs_loaded = TRUE;

    read_filter_list(CFILTER_LIST);

    cap_file_init(&cfile);


    /* Print format defaults to this. */
    print_format = PR_FMT_TEXT;
    delimiter_char = " ";

    output_fields = output_fields_new();

    output_file_name = args.value("filename").c_str();
    cf_name = g_strdup(output_file_name);

    prefs_apply_all();

    //start_exportobjects();

    timestamp_set_type(global_dissect_options.time_format);

    if (!setup_enabled_and_disabled_protocols()) {
      exit_status = INVALID_OPTION;
      goto clean_exit;
    }

    print_stream = print_stream_text_stdio_new(stdout);

    if (cf_open(&cfile, cf_name, in_file_type, FALSE, &err) != CF_OK) {
      epan_cleanup();
      extcap_cleanup();
      exit_status = INVALID_FILE;
      goto clean_exit;
    }


clean_exit:
    capture_opts_cleanup(&global_capture_opts);

    epan_cleanup();
    wtap_cleanup();
    //cf_close(&cfile);
    std::exit(exit_status);
}

void Ebc::drawEvent() {
    GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    swapBuffers();
}

}


void packet_handler(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
    printf("%s\n", user);
    fprintf(stdout, "packet %d %d %x\n", h->caplen, h->len, bytes[0]);
}

void pipe_input_set_handler(gint source, gpointer user_data, ws_process_id *child_process, pipe_input_cb_t input_cb)
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


void capture_input_drops(capture_session *cap_session _U_, guint32 dropped) 
{
    return;
}

void capture_input_closed(capture_session *cap_session, gchar *msg) {
    return;
}

void capture_input_cfilter_error_message(capture_session *cap_session, guint i, char *error_message) 
{
    return;
}

MAGNUM_APPLICATION_MAIN(Magnum::Ebc)
