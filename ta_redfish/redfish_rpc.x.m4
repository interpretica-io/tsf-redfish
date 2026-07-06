/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief RPC for Redfish client operations
 *
 * The RPCs of rpcs_redfish, a thin layer over ta_redfish, which talks
 * Redfish from the RPC server process over libcurl + jansson. Add this
 * file to the rpcxdr definitions of the engine platform and of the
 * agent platform:
 *
 *   TE_LIB_PARMS([rpcxdr], [<platform>], [],
 *                [--with-rpcdefs=tarpc_job.x.m4,../ta_redfish/redfish_rpc.x.m4])
 *
 * No handle survives between calls: each request is whole in, verdict
 * out. The root RPC returns a "field \t value" per line record.
 */

/* redfish_get(): GET base_url+path, optional HTTP Basic auth. */
struct tarpc_redfish_get_in {
    struct tarpc_in_arg common;

    string          base_url<>;
    string          path<>;
    string          user<>;
    string          pass<>;
    tarpc_bool      basic;
};

struct tarpc_redfish_get_out {
    struct tarpc_out_arg common;

    tarpc_int       retval;
    tarpc_int       http_code;
    string          body<>;
};

/* redfish_root(): GET /redfish/v1 and summarize it (field/value lines). */
struct tarpc_redfish_root_in {
    struct tarpc_in_arg common;

    string          base_url<>;
    string          user<>;
    string          pass<>;
};

struct tarpc_redfish_root_out {
    struct tarpc_out_arg common;

    tarpc_int       retval;
    tarpc_int       http_code;
    string          info<>;
};

program redfish
{
    version ver0
    {
        RPC_DEF(redfish_get)
        RPC_DEF(redfish_root)
    } = 1;
} = 41;
