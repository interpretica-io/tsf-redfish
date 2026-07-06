# SPDX-License-Identifier: MIT
# Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved.
#
# External TE library (see TE_EXT_REPO): the redfish_* RPCs for the RPC
# server of an agent. List it in the libraries of the ta_rpcprovider
# application together with ta_redfish, and add
# ta_redfish/redfish_rpc.x.m4 to the rpcxdr definitions of both platforms.

sources += files(
    'rpcs_redfish.c',
)

te_libs += [
    'rpcserver',
    'ta_redfish',
]
