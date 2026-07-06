/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief What a Redfish service is worth as a security posture
 *
 * @defgroup tapi_redfish_audit Redfish security posture
 * @ingroup tapi_redfish
 * @{
 *
 * A server's/BMC's Redfish service read as a security posture and
 * reported through tsf-cybersec: that it answers at all, that it is
 * served without TLS, and - only when asked - whether a well-known
 * default credential still logs in.
 *
 * | Finding | Severity | Raised when |
 * |---|---|---|
 * | @c redfish.present | info | the service root answers |
 * | @c redfish.http-no-tls | high | the base URL is @c http, not @c https |
 * | @c redfish.default-credentials | critical | a default credential authenticates |
 * | @c redfish.not-assessed | info | the service could not be reached |
 *
 * The default-credential check logs in to a real BMC, so it is OFF by
 * default and gated behind @a attempt_default_creds. Enable it only
 * against a device you are authorized to assess.
 */

#ifndef __TAPI_REDFISH_AUDIT_H__
#define __TAPI_REDFISH_AUDIT_H__

#include "te_errno.h"
#include "rcf_rpc.h"

#include "tapi_cybersec.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A credential to try. */
typedef struct tapi_redfish_cred {
    /** Username (a @c NULL username ends a list). */
    const char *user;
    /** Password. */
    const char *pass;
} tapi_redfish_cred;

/** What the Redfish service is expected to be. */
typedef struct tapi_redfish_audit_policy {
    /** Try default credentials (logs in to the device). Off by default. */
    bool attempt_default_creds;
    /**
     * Credentials to try, ended by a @c {NULL,NULL} entry, or @c NULL
     * for the built-in list of well-known BMC defaults.
     */
    const tapi_redfish_cred *creds;
} tapi_redfish_audit_policy;

/** The default: do NOT attempt credentials; built-in list if enabled. */
extern const tapi_redfish_audit_policy tapi_redfish_default_audit_policy;

/**
 * Read a Redfish service's posture into @p report.
 *
 * @param[in]  rpcs     RPC server on the agent.
 * @param[in]  base_url Service base URL, e.g. @c "https://10.0.0.1".
 * @param[in]  policy   What is expected, or @c NULL for the default.
 * @param[out] report   Report to append findings to.
 *
 * @return Status code of reading the posture, not its verdict.
 */
extern te_errno tapi_redfish_audit(rcf_rpc_server *rpcs, const char *base_url,
                                   const tapi_redfish_audit_policy *policy,
                                   tapi_cybersec_report *report);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TAPI_REDFISH_AUDIT_H__ */

/**@} <!-- END tapi_redfish_audit --> */
