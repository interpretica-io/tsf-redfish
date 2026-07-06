/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Talking Redfish to a server/BMC from a test
 *
 * @defgroup tapi_redfish Redfish (tapi_redfish)
 * @{
 *
 * Reading a server's or BMC's Redfish (DMTF out-of-band management:
 * HTTP/REST/JSON) service from a Test Agent: the service root and its
 * identity, an arbitrary resource, and whether a credential
 * authenticates. The agent does the HTTP over libcurl; nothing on the
 * managed system is changed (no power or reset actions).
 *
 * - tapi_redfish_get_root() summarizes @c /redfish/v1;
 * - tapi_redfish_get() fetches any resource;
 * - tapi_redfish_auth_check() says whether a user/password works;
 * - @ref tapi_redfish_audit (tapi_redfish_audit.h) reads the service as
 *   a security posture through tsf-cybersec.
 */

#ifndef __TAPI_REDFISH_H__
#define __TAPI_REDFISH_H__

#include "te_defs.h"
#include "te_errno.h"
#include "te_string.h"
#include "rcf_rpc.h"

#include "tapi_redfish_rpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The service root's identity. Fields are @c NULL when not exposed. */
typedef struct tapi_redfish_root {
    /** @c RedfishVersion, e.g. @c "1.18.0". */
    char *redfish_version;
    /** @c Vendor. */
    char *vendor;
    /** @c Product. */
    char *product;
    /** @c UUID of the service. */
    char *uuid;
    /** @c Name. */
    char *name;
    /** @c true when the base URL is @c https. */
    bool secure;
} tapi_redfish_root;

/**
 * Fetch and summarize the service root (@c /redfish/v1).
 *
 * @param[in]  rpcs         RPC server on the agent.
 * @param[in]  base_url     Service base URL, e.g. @c "https://10.0.0.1".
 * @param[in]  user         Username, or @c NULL.
 * @param[in]  pass         Password, or @c NULL.
 * @param[out] out          Root identity; release with
 *                          tapi_redfish_root_free().
 * @param[out] http_code    HTTP status, or @c NULL.
 *
 * @return Status code.
 * @retval TE_ECOMM         The service could not be reached.
 */
extern te_errno tapi_redfish_get_root(rcf_rpc_server *rpcs,
                                      const char *base_url, const char *user,
                                      const char *pass,
                                      tapi_redfish_root *out, int *http_code);

/**
 * GET a Redfish resource.
 *
 * @param[in]  rpcs         RPC server on the agent.
 * @param[in]  base_url     Service base URL.
 * @param[in]  path         Resource path, e.g. @c "/redfish/v1/Systems".
 * @param[in]  user         Username, or @c NULL.
 * @param[in]  pass         Password, or @c NULL.
 * @param[in]  basic        Use HTTP Basic auth when @p user is set.
 * @param[out] http_code    HTTP status, or @c NULL.
 * @param[out] body         Response body, or @c NULL.
 *
 * @return Status code.
 */
extern te_errno tapi_redfish_get(rcf_rpc_server *rpcs, const char *base_url,
                                 const char *path, const char *user,
                                 const char *pass, te_bool basic,
                                 int *http_code, te_string *body);

/**
 * Does a credential authenticate against the service?
 *
 * GETs a protected resource with the credential and reports whether the
 * service accepted it (HTTP 200) rather than rejecting it (401/403).
 *
 * @param rpcs          RPC server on the agent.
 * @param base_url      Service base URL.
 * @param user          Username.
 * @param pass          Password.
 *
 * @return @c true when the credential authenticated.
 */
extern bool tapi_redfish_auth_check(rcf_rpc_server *rpcs,
                                    const char *base_url, const char *user,
                                    const char *pass);

/**
 * Write a service root into the log.
 *
 * @param root          Root.
 */
extern void tapi_redfish_root_log(const tapi_redfish_root *root);

/**
 * Release a service root.
 *
 * @param root          Root.
 */
extern void tapi_redfish_root_free(tapi_redfish_root *root);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TAPI_REDFISH_H__ */

/**@} <!-- END tapi_redfish --> */
