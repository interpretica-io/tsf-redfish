/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Redfish TAPI: RPC client wrappers
 *
 * The rcf_rpc_call() boilerplate behind tapi_redfish. The RPCs return
 * te_errno; an RPC transport failure is mapped to TE_ECORRUPTED.
 */

#define TE_LGR_USER     "TAPI REDFISH RPC"

#include "te_config.h"

#include <string.h>

#include "te_defs.h"
#include "te_errno.h"
#include "te_string.h"
#include "logger_api.h"
#include "tapi_rpc_internal.h"
#include "tarpc.h"

#include "tapi_redfish_rpc.h"

#define CHECK_RPC_ERRNO_UNCHANGED(_func, _var) \
    CHECK_RETVAL_VAR_ERR_COND(_func, _var, false,                    \
                              TE_RC(TE_TAPI, TE_ECORRUPTED), false)

/* Append an RPC string result, when there is one. */
static void
take_string(te_string *dst, const char *src)
{
    if (dst != NULL && src != NULL)
        te_string_append(dst, "%s", src);
}

/* See description in tapi_redfish_rpc.h */
te_errno
rpc_redfish_get(rcf_rpc_server *rpcs, const char *base_url, const char *path,
                const char *user, const char *pass, te_bool basic,
                int *http_code, te_string *body)
{
    tarpc_redfish_get_in in;
    tarpc_redfish_get_out out;

    memset(&in, 0, sizeof(in));
    memset(&out, 0, sizeof(out));
    in.base_url = (char *)(base_url != NULL ? base_url : "");
    in.path = (char *)(path != NULL ? path : "");
    in.user = (char *)(user != NULL ? user : "");
    in.pass = (char *)(pass != NULL ? pass : "");
    in.basic = basic;

    rcf_rpc_call(rpcs, "redfish_get", &in, &out);
    CHECK_RPC_ERRNO_UNCHANGED(redfish_get, out.retval);
    TAPI_RPC_LOG(rpcs, redfish_get, "%s%s basic=%d", "%r http=%d",
                 base_url != NULL ? base_url : "", path != NULL ? path : "",
                 basic, out.retval, out.http_code);

    if (http_code != NULL)
        *http_code = out.http_code;
    if (out.retval == 0)
        take_string(body, out.body);
    RETVAL_TE_ERRNO(redfish_get, out.retval);
}

/* See description in tapi_redfish_rpc.h */
te_errno
rpc_redfish_root(rcf_rpc_server *rpcs, const char *base_url, const char *user,
                 const char *pass, int *http_code, te_string *info)
{
    tarpc_redfish_root_in in;
    tarpc_redfish_root_out out;

    memset(&in, 0, sizeof(in));
    memset(&out, 0, sizeof(out));
    in.base_url = (char *)(base_url != NULL ? base_url : "");
    in.user = (char *)(user != NULL ? user : "");
    in.pass = (char *)(pass != NULL ? pass : "");

    rcf_rpc_call(rpcs, "redfish_root", &in, &out);
    CHECK_RPC_ERRNO_UNCHANGED(redfish_root, out.retval);
    TAPI_RPC_LOG(rpcs, redfish_root, "%s", "%r http=%d",
                 base_url != NULL ? base_url : "", out.retval, out.http_code);

    if (http_code != NULL)
        *http_code = out.http_code;
    if (out.retval == 0)
        take_string(info, out.info);
    RETVAL_TE_ERRNO(redfish_root, out.retval);
}
