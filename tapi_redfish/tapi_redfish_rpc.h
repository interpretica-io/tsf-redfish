/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Redfish TAPI: RPC client wrappers
 *
 * Client wrappers of the redfish_* RPCs, see redfish_rpc.x.m4. Tests
 * use tapi_redfish.h; these are the calls behind it, one per RPC.
 */

#ifndef __TAPI_REDFISH_RPC_H__
#define __TAPI_REDFISH_RPC_H__

#include "te_defs.h"
#include "te_errno.h"
#include "te_string.h"
#include "rcf_rpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * GET a Redfish resource on the agent.
 *
 * @param[in]  rpcs       RPC server on the agent.
 * @param[in]  base_url   Service base URL.
 * @param[in]  path       Resource path.
 * @param[in]  user       Username, or @c NULL.
 * @param[in]  pass       Password, or @c NULL.
 * @param[in]  basic      Use HTTP Basic auth.
 * @param[out] http_code  HTTP status, or @c NULL.
 * @param[out] body       Response body, or @c NULL.
 *
 * @return Status code.
 */
extern te_errno rpc_redfish_get(rcf_rpc_server *rpcs, const char *base_url,
                                const char *path, const char *user,
                                const char *pass, te_bool basic,
                                int *http_code, te_string *body);

/**
 * GET and summarize the service root on the agent.
 *
 * @param[in]  rpcs       RPC server on the agent.
 * @param[in]  base_url   Service base URL.
 * @param[in]  user       Username, or @c NULL.
 * @param[in]  pass       Password, or @c NULL.
 * @param[out] http_code  HTTP status, or @c NULL.
 * @param[out] info       "field\\tvalue" lines, or @c NULL.
 *
 * @return Status code.
 */
extern te_errno rpc_redfish_root(rcf_rpc_server *rpcs, const char *base_url,
                                 const char *user, const char *pass,
                                 int *http_code, te_string *info);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TAPI_REDFISH_RPC_H__ */
