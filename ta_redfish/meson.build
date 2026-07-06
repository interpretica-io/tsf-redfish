# SPDX-License-Identifier: MIT
# Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved.
#
# External TE library (see TE_EXT_REPO): agent-side Redfish client over
# libcurl (HTTP) and jansson (JSON). The agent and its RPC server both
# link it.

headers += files(
    'ta_redfish.h',
)

sources += files(
    'ta_redfish.c',
)

te_libs += [
    'logger_core',
    'tools',
]

cc = meson.get_compiler('c')

curl_dep = dependency('libcurl', required: false)
if not curl_dep.found()
    curl_dep = cc.find_library('curl', has_headers: 'curl/curl.h')
endif

jansson_dep = dependency('jansson', required: false)
if not jansson_dep.found()
    jansson_dep = cc.find_library('jansson', has_headers: 'jansson.h')
endif

deps += [
    curl_dep,
    jansson_dep,
]
