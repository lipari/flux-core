
#include <zmq.h>
#include <czmq.h>
#include <json/json.h>

#include "zmsg.h"
#include "route.h"
#include "cmbd.h"
#include "log.h"
#include "util.h"
#include "plugin.h"

/* Copy input arguments to output arguments and respond to RPC.
 */
static void mecho_recv (plugin_ctx_t *p, zmsg_t **zmsg, zmsg_type_t type)
{
    json_object *request = NULL;
    json_object *inarg = NULL;
    flux_mrpc_t f = NULL;

    if (type != ZMSG_EVENT) {
        plugin_log (p, LOG_ERR, "ignoring non-event message");
        goto done;
    }
    if (cmb_msg_decode (*zmsg, NULL, &request) < 0) {
        plugin_log (p, LOG_ERR, "cmb_msg_decode: %s", strerror (errno));
        goto done;
    }
    if (!request) {
        plugin_log (p, LOG_ERR, "missing JSON part");
        goto done;
    }
    if (!(f = flux_mrpc_create_fromevent (p, request))) {
        if (errno != EINVAL) /* EINVAL == not addressed to me */
            plugin_log (p, LOG_ERR, "flux_mrpc_create_fromevent: %s",
                                    strerror (errno));
        goto done;
    }
    if (flux_mrpc_get_inarg (f, &inarg) < 0) {
        plugin_log (p, LOG_ERR, "flux_mrpc_get_inarg: %s", strerror (errno));
        goto done;
    }
    flux_mrpc_put_outarg (f, inarg);
    if (flux_mrpc_respond (f) < 0) {
        plugin_log (p, LOG_ERR, "flux_mrpc_respond: %s", strerror (errno));
        goto done;
    }
done:
    if (request)
        json_object_put (request);
    if (inarg)
        json_object_put (inarg);
    if (f)
        flux_mrpc_destroy (f);
    zmsg_destroy (zmsg);
}

static void mecho_init (plugin_ctx_t *p)
{
    if (flux_event_subscribe (p, "mrpc.mecho") < 0)
        err_exit ("%s: flux_event_subscribe", __FUNCTION__);
}

static void mecho_fini (plugin_ctx_t *p)
{
    if (flux_event_unsubscribe (p, "mrpc.mecho") < 0)
        err_exit ("%s: flux_event_unsubscribe", __FUNCTION__);
}

struct plugin_struct mechosrv = {
    .name = "mecho",
    .recvFn = mecho_recv,
    .initFn = mecho_init,
    .finiFn = mecho_fini,
};

/*
 * vi: ts=4 sw=4 expandtab
 */
