/*
 * Do not modify this file. Changes will be overwritten.
 *
 * Generated automatically from /home/synnick/projects/monopticon/tools/make-plugin-reg.py.
 */

#include "config.h"

#include <gmodule.h>

/* plugins are DLLs */
#define WS_BUILD_DLL
#include "ws_symbol_export.h"

#include "wiretap/wtap.h"

void wtap_register_usbdump(void);

WS_DLL_PUBLIC_DEF const gchar plugin_version[] = PLUGIN_VERSION;
WS_DLL_PUBLIC_DEF const gchar plugin_release[] = VERSION_RELEASE;

WS_DLL_PUBLIC void plugin_register(void);

void plugin_register(void)
{
    static wtap_plugin plug_usbdump;

    plug_usbdump.register_wtap_module = wtap_register_usbdump;
    wtap_register_plugin(&plug_usbdump);
}
