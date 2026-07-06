/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2026 Interpretica, Unipessoal Lda. All rights reserved. */
/** @file
 * @brief Agent-side Redfish client over libcurl + jansson
 *
 * GETs a Redfish resource (optionally with HTTP Basic auth) and, for
 * the service root, pulls a few identifying fields out of the JSON. The
 * libraries are linked and called in-process. TLS peer verification is
 * disabled because BMCs ship self-signed certificates.
 */

#define TE_LGR_USER     "TA REDFISH"

#include "te_config.h"

#include <string.h>

#include <curl/curl.h>
#include <jansson.h>

#include "te_defs.h"
#include "te_errno.h"
#include "te_alloc.h"
#include "te_str.h"
#include "te_string.h"
#include "logger_api.h"

#include "ta_redfish.h"

/** libcurl write callback: append the body to a te_string. */
static size_t
redfish_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t len = size * nmemb;

    te_string_append((te_string *)userdata, "%.*s", (int)len, ptr);
    return len;
}

/** Shared GET; fills @p body and @p http_code. */
static te_errno
redfish_do_get(const char *base_url, const char *path, const char *user,
               const char *pass, te_bool basic, int *http_code,
               te_string *body)
{
    CURL *curl;
    CURLcode res;
    te_string url = TE_STRING_INIT;
    long code = 0;
    te_errno rc = 0;

    *http_code = 0;

    curl = curl_easy_init();
    if (curl == NULL)
        return TE_RC(TE_TA_UNIX, TE_EFAIL);

    te_string_append(&url, "%s%s", base_url != NULL ? base_url : "",
                     path != NULL ? path : "");

    curl_easy_setopt(curl, CURLOPT_URL, url.ptr);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, redfish_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, body);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    if (basic && user != NULL && user[0] != '\0')
    {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, user);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, pass != NULL ? pass : "");
    }

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        ERROR("Redfish GET %s: %s", url.ptr, curl_easy_strerror(res));
        rc = TE_RC(TE_TA_UNIX, TE_ECOMM);
    }
    else
    {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        *http_code = (int)code;
    }

    curl_easy_cleanup(curl);
    te_string_free(&url);

    return rc;
}

/* See description in ta_redfish.h */
te_errno
ta_redfish_get(const char *base_url, const char *path, const char *user,
               const char *pass, te_bool basic, int *http_code,
               te_string *body)
{
    return redfish_do_get(base_url, path, user, pass, basic, http_code,
                          body);
}

/** Append "field\tvalue\n" when @p obj has string member @p key. */
static void
redfish_field(json_t *obj, const char *key, const char *label,
              te_string *info)
{
    json_t *v = json_object_get(obj, key);

    if (json_is_string(v))
        te_string_append(info, "%s\t%s\n", label, json_string_value(v));
}

/* See description in ta_redfish.h */
te_errno
ta_redfish_root(const char *base_url, const char *user, const char *pass,
                int *http_code, te_string *info)
{
    te_string body = TE_STRING_INIT;
    json_t *root;
    json_error_t err;
    te_errno rc;

    te_string_append(info, "scheme\t%s\n",
                     (base_url != NULL &&
                      strncmp(base_url, "https://", 8) == 0) ?
                     "https" : "http");

    rc = redfish_do_get(base_url, "/redfish/v1", user, pass, true, http_code,
                        &body);
    if (rc != 0)
    {
        te_string_free(&body);
        return rc;
    }

    root = json_loads(te_string_value(&body), 0, &err);
    if (root != NULL)
    {
        redfish_field(root, "RedfishVersion", "redfish_version", info);
        redfish_field(root, "Vendor", "vendor", info);
        redfish_field(root, "Product", "product", info);
        redfish_field(root, "UUID", "uuid", info);
        redfish_field(root, "Name", "name", info);
        json_decref(root);
    }

    te_string_free(&body);

    return 0;
}
