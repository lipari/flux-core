/*****************************************************************************\
 *  Copyright (c) 2014 Lawrence Livermore National Security, LLC.  Produced at
 *  the Lawrence Livermore National Laboratory (cf, AUTHORS, DISCLAIMER.LLNS).
 *  LLNL-CODE-658032 All rights reserved.
 *
 *  This file is part of the Flux resource manager framework.
 *  For details, see https://github.com/flux-framework.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the license, or (at your option)
 *  any later version.
 *
 *  Flux is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the terms and conditions of the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *  See also:  http://www.gnu.org/licenses/
\*****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <lua.h>
#include <lauxlib.h>
#include <zmq.h>
#include <czmq.h>
#include <errno.h>
#include <string.h>

#include "flux/core.h"
#include "json-lua.h"
#include "zmsg-lua.h"
#include "lutil.h"


flux_msg_t *l_cmb_zmsg_encode (lua_State *L)
{
	const char *tag = lua_tostring (L, 1);
	json_object *o;

    lua_value_to_json (L, 2, &o);
	if ((o == NULL) || (tag == NULL))
		return NULL;

    flux_msg_t *msg = flux_msg_create (FLUX_MSGTYPE_REQUEST);
    const char *json_str = json_object_to_json_string (o);
    if (!msg || flux_msg_set_topic (msg, tag) < 0
              || flux_msg_set_json (msg, json_str) < 0) {
        flux_msg_destroy (msg);
        return NULL;
    }
    return (msg);
}

static int l_zi_resp_cb (lua_State *L,
    struct zmsg_info *zi, json_object *resp, void *arg)
{
    flux_msg_t **old = zmsg_info_zmsg (zi);
    flux_msg_t **msg = malloc (sizeof (*msg));
    *msg = flux_msg_copy (*old, true);

    const char *json_str = NULL;
    if (resp)
        json_str = json_object_to_json_string (resp);
    if (flux_msg_set_json (*msg, json_str) < 0) {
        flux_msg_destroy (*msg);
        free (msg);
        return lua_pusherror (L, "flux_msg_set_json: %s", strerror (errno));
    }

    return lua_push_zmsg_info (L, zmsg_info_create (msg, FLUX_MSGTYPE_RESPONSE));
}

static int l_cmb_zmsg_create_type (lua_State *L, int type)
{
    struct zmsg_info *zi;
	flux_msg_t **msg = malloc (sizeof (*msg));
	if ((*msg = l_cmb_zmsg_encode (L)) == NULL) {
        free (msg);
        return luaL_error (L, "Failed to encode zmsg");
    }
    zi = zmsg_info_create (msg, type);
    zmsg_info_register_resp_cb (zi, l_zi_resp_cb, NULL);

	return lua_push_zmsg_info (L, zi);
}

static int l_cmb_zmsg_create_response (lua_State *L)
{
    return l_cmb_zmsg_create_type (L, FLUX_MSGTYPE_RESPONSE);
}

/*
 *  Send new-API-style response with errnum.
 */
static int l_cmb_zmsg_create_response_with_error (lua_State *L)
{
    struct zmsg_info *zi;
    int errnum;
    const char *tag;
    flux_msg_t *msg = flux_msg_create (FLUX_MSGTYPE_RESPONSE);
    if (!msg)
        return lua_pusherror (L, "flux_msg_create: %s", strerror (errno));

    tag = lua_tostring (L, 1);
    errnum = lua_tointeger (L, 2);

    if (flux_msg_set_topic (msg, tag) < 0)
        return lua_pusherror (L, "flux_msg_set_topic: %s", strerror (errno));
    if (flux_msg_set_errnum (msg, errnum) < 0)
        return lua_pusherror (L, "flux_msg_set_errnum: %s", strerror (errno));
    if (flux_msg_set_payload (msg, 0, NULL, 0))
        return lua_pusherror (L, "flux_msg_set_payload: %s", strerror (errno));

    zi = zmsg_info_create (&msg, FLUX_MSGTYPE_RESPONSE);
    flux_msg_destroy (msg);

    return lua_push_zmsg_info (L, zi);
}

static int l_cmb_zmsg_create_request (lua_State *L)
{
    return l_cmb_zmsg_create_type (L, FLUX_MSGTYPE_REQUEST);
}

static int l_cmb_zmsg_create_event (lua_State *L)
{
    return l_cmb_zmsg_create_type (L, FLUX_MSGTYPE_EVENT);
}

static const struct luaL_Reg zmsg_info_test_functions [] = {
	{ "req",       l_cmb_zmsg_create_request   },
	{ "resp",      l_cmb_zmsg_create_response  },
	{ "resp_err",  l_cmb_zmsg_create_response_with_error },
	{ "event",     l_cmb_zmsg_create_event     },
	{ NULL,        NULL              }
};

int luaopen_zmsgtest (lua_State *L)
{
    l_zmsg_info_register_metatable (L);
    lua_newtable (L);
    luaL_setfuncs (L, zmsg_info_test_functions, 0);
	return (1);
}

/*
 * vi: ts=4 sw=4 expandtab
 */
