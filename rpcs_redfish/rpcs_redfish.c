/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Redfish RPC server library
 *
 * The redfish_* RPCs (see redfish_rpc.x.m4) on top of ta_redfish.
 * TARPC_FUNC_STATIC() binds an RPC to the function of the same name,
 * so each RPC has a plain C function first and the wrapper after it.
 */

#define TE_LGR_USER     "RPC REDFISH"

#include "te_config.h"

#include <string.h>

#include "te_defs.h"
#include "te_errno.h"
#include "te_alloc.h"
#include "te_str.h"
#include "te_string.h"
#include "rpc_server.h"

#include "ta_redfish.h"

/* Hand a te_string result over to an RPC string field (never NULL). */
static char *
take(te_string *str)
{
    return str->ptr != NULL ? str->ptr : TE_STRDUP("");
}

static te_errno
redfish_get(const char *base_url, const char *path, const char *user,
            const char *pass, te_bool basic, int *http_code, char **body)
{
    te_string b = TE_STRING_INIT;
    te_errno rc = ta_redfish_get(base_url, path, user, pass, basic,
                                 http_code, &b);

    *body = take(&b);
    return rc;
}

TARPC_FUNC_STATIC(redfish_get, {},
{
    int http_code = 0;

    MAKE_CALL(out->retval = func(in->base_url, in->path, in->user, in->pass,
                                 in->basic, &http_code, &out->body));
    out->http_code = http_code;
    out->common.errno_changed = false;
})

static te_errno
redfish_root(const char *base_url, const char *user, const char *pass,
             int *http_code, char **info)
{
    te_string i = TE_STRING_INIT;
    te_errno rc = ta_redfish_root(base_url, user, pass, http_code, &i);

    *info = take(&i);
    return rc;
}

TARPC_FUNC_STATIC(redfish_root, {},
{
    int http_code = 0;

    MAKE_CALL(out->retval = func(in->base_url, in->user, in->pass,
                                 &http_code, &out->info));
    out->http_code = http_code;
    out->common.errno_changed = false;
})
