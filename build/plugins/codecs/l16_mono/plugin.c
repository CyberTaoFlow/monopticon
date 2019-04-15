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

#include "codecs/codecs.h"

void codec_register_l16(void);

WS_DLL_PUBLIC_DEF const gchar plugin_version[] = PLUGIN_VERSION;
WS_DLL_PUBLIC_DEF const gchar plugin_release[] = VERSION_RELEASE;

WS_DLL_PUBLIC void plugin_register(void);

void plugin_register(void)
{
    static codecs_plugin plug_l16;

    plug_l16.register_codec_module = codec_register_l16;
    codecs_register_plugin(&plug_l16);
}
