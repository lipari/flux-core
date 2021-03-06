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
#include <flux/core.h>

int kvs_get_version (flux_t *h, int *versionp)
{
    flux_future_t *f;
    int version;
    int rc = -1;

    if (!(f = flux_rpc (h, "kvs.getroot", NULL, FLUX_NODEID_ANY, 0)))
        goto done;
    if (flux_rpc_get_unpack (f, "{ s:i }", "rootseq", &version) < 0)
        goto done;
    if (versionp)
        *versionp = version;
    rc = 0;
done:
    flux_future_destroy (f);
    return rc;
}

int kvs_wait_version (flux_t *h, int version)
{
    flux_future_t *f;
    int ret = -1;

    if (!(f = flux_rpc_pack (h, "kvs.sync", FLUX_NODEID_ANY, 0, "{ s:i }",
                             "rootseq", version)))
        goto done;
    /* N.B. response contains (rootseq, rootdir) but we don't need it.
     */
    if (flux_future_get (f, NULL) < 0)
        goto done;
    ret = 0;
done:
    flux_future_destroy (f);
    return ret;
}

int kvs_dropcache (flux_t *h)
{
    flux_future_t *f;
    int rc = -1;

    if (!(f = flux_rpc (h, "kvs.dropcache", NULL, FLUX_NODEID_ANY, 0)))
        goto done;
    if (flux_future_get (f, NULL) < 0)
        goto done;
    rc = 0;
done:
    flux_future_destroy (f);
    return rc;
}

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
