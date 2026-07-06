/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief What a Redfish service is worth as a security posture
 *
 * Reads the service root and classifies it into tsf-cybersec findings;
 * optionally, and only when asked, tries well-known default credentials.
 */

#define TE_LGR_USER     "TAPI REDFISH AUDIT"

#include "te_config.h"

#include <string.h>

#include "te_defs.h"
#include "te_errno.h"
#include "te_string.h"
#include "logger_api.h"

#include "tapi_redfish.h"
#include "tapi_redfish_audit.h"

/** Well-known BMC default credentials, tried only when enabled. */
static const tapi_redfish_cred redfish_default_creds[] = {
    { "admin",         "password" },
    { "admin",         "admin" },
    { "root",          "calvin" },     /* Dell iDRAC */
    { "Administrator", "admin" },       /* some BMCs */
    { "ADMIN",         "ADMIN" },        /* Supermicro */
    { "root",          "0penBmc" },      /* OpenBMC demo */
    { NULL, NULL },
};

/* See description in tapi_redfish_audit.h */
const tapi_redfish_audit_policy tapi_redfish_default_audit_policy = {
    .attempt_default_creds = false,
    .creds = NULL,
};

/* See description in tapi_redfish_audit.h */
te_errno
tapi_redfish_audit(rcf_rpc_server *rpcs, const char *base_url,
                   const tapi_redfish_audit_policy *policy,
                   tapi_cybersec_report *report)
{
    tapi_redfish_root root;
    const tapi_redfish_cred *creds;
    int http_code = 0;
    te_errno rc;
    size_t i;

    if (policy == NULL)
        policy = &tapi_redfish_default_audit_policy;

    rc = tapi_redfish_get_root(rpcs, base_url, NULL, NULL, &root, &http_code);
    if (rc != 0 || http_code == 0)
    {
        tapi_cybersec_report_add(report, TAPI_CYBERSEC_SEV_INFO,
            "redfish.not-assessed", base_url,
            "the Redfish service could not be reached (%r, http %d)", rc,
            http_code);
        if (rc == 0)
            tapi_redfish_root_free(&root);
        return 0;
    }

    tapi_cybersec_report_add(report, TAPI_CYBERSEC_SEV_INFO,
        "redfish.present", base_url,
        "Redfish service answers: %s %s (Redfish %s), http %d",
        root.vendor != NULL ? root.vendor : "?",
        root.product != NULL ? root.product : "?",
        root.redfish_version != NULL ? root.redfish_version : "?", http_code);

    if (!root.secure)
    {
        tapi_cybersec_report_add(report, TAPI_CYBERSEC_SEV_HIGH,
            "redfish.http-no-tls", base_url,
            "the Redfish service is served over http, not https - "
            "management credentials cross the network in the clear");
    }

    if (policy->attempt_default_creds)
    {
        creds = policy->creds != NULL ? policy->creds : redfish_default_creds;
        for (i = 0; creds[i].user != NULL; i++)
        {
            if (tapi_redfish_auth_check(rpcs, base_url, creds[i].user,
                                        creds[i].pass))
            {
                tapi_cybersec_report_add(report, TAPI_CYBERSEC_SEV_CRITICAL,
                    "redfish.default-credentials", base_url,
                    "a default credential authenticates (user '%s') - the "
                    "BMC grants full out-of-band control", creds[i].user);
                break;
            }
        }
    }

    tapi_redfish_root_free(&root);

    return 0;
}
