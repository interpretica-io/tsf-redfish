/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Agent-side Redfish client
 *
 * A minimal Redfish (DMTF server/BMC out-of-band management) client on
 * top of libcurl (HTTP/HTTPS) and jansson (JSON), called in the agent's
 * process - the libraries are linked, nothing is spawned. The agent and
 * its RPC server both link it; the RPCs (see redfish_rpc.x.m4) are thin
 * wrappers over these functions.
 *
 * It does GETs and an optional HTTP Basic authentication; it does not
 * change the managed system (no resets, no power actions). BMCs almost
 * always serve a self-signed certificate, so TLS peer verification is
 * off - this reaches the device, it does not prove the certificate.
 */

#ifndef __TA_REDFISH_H__
#define __TA_REDFISH_H__

#include "te_defs.h"
#include "te_errno.h"
#include "te_string.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * GET a Redfish resource.
 *
 * @param[in]  base_url     Service base, e.g. @c "https://10.0.0.1".
 * @param[in]  path         Resource path, e.g. @c "/redfish/v1".
 * @param[in]  user         Username, or @c NULL / @c "" for no auth.
 * @param[in]  pass         Password, or @c NULL.
 * @param[in]  basic        Use HTTP Basic auth when @p user is set.
 * @param[out] http_code    HTTP status, or @c 0 when none was received.
 * @param[out] body         The response body.
 *
 * @return Status code.
 * @retval TE_ECOMM         The endpoint could not be reached.
 */
extern te_errno ta_redfish_get(const char *base_url, const char *path,
                               const char *user, const char *pass,
                               te_bool basic, int *http_code,
                               te_string *body);

/**
 * GET the service root (@c /redfish/v1) and summarize it.
 *
 * @param[in]  base_url     Service base.
 * @param[in]  user         Username, or @c NULL.
 * @param[in]  pass         Password, or @c NULL.
 * @param[out] http_code    HTTP status.
 * @param[out] info         One @c "field\\tvalue" line per field:
 *                          @c redfish_version, @c vendor, @c product,
 *                          @c uuid (those the root exposes) and
 *                          @c scheme (@c "https" / @c "http").
 *
 * @return Status code.
 */
extern te_errno ta_redfish_root(const char *base_url, const char *user,
                                const char *pass, int *http_code,
                                te_string *info);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TA_REDFISH_H__ */
