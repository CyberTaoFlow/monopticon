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

#include "epan/proto.h"

void proto_register_wimaxasncp(void);
void proto_reg_handoff_wimaxasncp(void);

WS_DLL_PUBLIC_DEF const gchar plugin_version[] = PLUGIN_VERSION;
WS_DLL_PUBLIC_DEF const gchar plugin_release[] = VERSION_RELEASE;

WS_DLL_PUBLIC void plugin_register(void);

void plugin_register(void)
{
    static proto_plugin plug_wimaxasncp;

    plug_wimaxasncp.register_protoinfo = proto_register_wimaxasncp;
    plug_wimaxasncp.register_handoff = proto_reg_handoff_wimaxasncp;
    proto_register_plugin(&plug_wimaxasncp);
}
