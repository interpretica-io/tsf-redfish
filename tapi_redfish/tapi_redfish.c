/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Talking Redfish to a server/BMC from a test
 *
 * The engine-neutral layer over the redfish_* RPCs: it parses the
 * service-root "field\\tvalue" lines into a #tapi_redfish_root and wraps
 * the GET/auth-check calls.
 */

#define TE_LGR_USER     "TAPI REDFISH"

#include "te_config.h"

#include <string.h>

#include "te_defs.h"
#include "te_errno.h"
#include "te_alloc.h"
#include "te_str.h"
#include "te_string.h"
#include "logger_api.h"

#include "tapi_redfish.h"

/** Pull the value of "field\tvalue\n" line @p field out of @p text. */
static char *
redfish_field(const char *text, const char *field)
{
    const char *line = text;
    size_t flen = strlen(field);

    while (line != NULL && *line != '\0')
    {
        const char *nl = strchr(line, '\n');
        size_t len = nl != NULL ? (size_t)(nl - line) : strlen(line);

        if (len > flen && strncmp(line, field, flen) == 0 &&
            line[flen] == '\t')
        {
            return TE_STRNDUP(line + flen + 1, len - flen - 1);
        }
        line = nl != NULL ? nl + 1 : NULL;
    }

    return NULL;
}

/* See description in tapi_redfish.h */
te_errno
tapi_redfish_get_root(rcf_rpc_server *rpcs, const char *base_url,
                      const char *user, const char *pass,
                      tapi_redfish_root *out, int *http_code)
{
    te_string info = TE_STRING_INIT;
    char *scheme;
    te_errno rc;

    memset(out, 0, sizeof(*out));

    rc = rpc_redfish_root(rpcs, base_url, user, pass, http_code, &info);
    if (rc != 0)
    {
        te_string_free(&info);
        return rc;
    }

    out->redfish_version = redfish_field(info.ptr, "redfish_version");
    out->vendor = redfish_field(info.ptr, "vendor");
    out->product = redfish_field(info.ptr, "product");
    out->uuid = redfish_field(info.ptr, "uuid");
    out->name = redfish_field(info.ptr, "name");
    scheme = redfish_field(info.ptr, "scheme");
    out->secure = scheme != NULL && strcmp(scheme, "https") == 0;
    free(scheme);

    te_string_free(&info);

    return 0;
}

/* See description in tapi_redfish.h */
te_errno
tapi_redfish_get(rcf_rpc_server *rpcs, const char *base_url, const char *path,
                 const char *user, const char *pass, te_bool basic,
                 int *http_code, te_string *body)
{
    return rpc_redfish_get(rpcs, base_url, path, user, pass, basic,
                           http_code, body);
}

/* See description in tapi_redfish.h */
bool
tapi_redfish_auth_check(rcf_rpc_server *rpcs, const char *base_url,
                        const char *user, const char *pass)
{
    te_string body = TE_STRING_INIT;
    int http_code = 0;
    te_errno rc;

    /*
     * The SessionService collection is present on every Redfish service
     * and protected, so a 200 with a credential means it authenticated;
     * 401/403 means it did not.
     */
    rc = rpc_redfish_get(rpcs, base_url, "/redfish/v1/SessionService",
                         user, pass, true, &http_code, &body);
    te_string_free(&body);

    return rc == 0 && http_code == 200;
}

/* See description in tapi_redfish.h */
void
tapi_redfish_root_log(const tapi_redfish_root *root)
{
    RING("Redfish service: %s %s (Redfish %s), %s, UUID %s",
         root->vendor != NULL ? root->vendor : "?",
         root->product != NULL ? root->product : "?",
         root->redfish_version != NULL ? root->redfish_version : "?",
         root->secure ? "https" : "http (no TLS)",
         root->uuid != NULL ? root->uuid : "?");
}

/* See description in tapi_redfish.h */
void
tapi_redfish_root_free(tapi_redfish_root *root)
{
    free(root->redfish_version);
    free(root->vendor);
    free(root->product);
    free(root->uuid);
    free(root->name);
    memset(root, 0, sizeof(*root));
}
