# tsf-redfish

Talking Redfish to a server or BMC from a Test Agent, packaged as an
external Test Environment (TE) repository (consumed with the
`TE_EXT_REPO` builder directive). It speaks Redfish over low-level C
libraries — **libcurl** (HTTP/HTTPS) and **jansson** (JSON), no Python,
nothing spawned — for both inventory and a security posture.

Three libraries:

- `ta_redfish` — agent side. A minimal Redfish client over libcurl +
  jansson: GET the service root, GET any resource, and HTTP Basic
  authentication. Read-only — it does not power, reset or reconfigure
  the managed system. The agent and its RPC server both link it.
- `rpcs_redfish` — the `redfish_*` RPCs for the agent's RPC server, thin
  wrappers over `ta_redfish`. The HTTP originates on the agent, which is
  the one that can reach the BMC's management network.
- `tapi_redfish` — engine side. `tapi_redfish.h` gives a test the
  service-root identity, an arbitrary GET, and a credential check;
  `tapi_redfish_audit.h` reads the service as a security posture through
  tsf-cybersec; `tapi_redfish_rpc.h` is the one-per-RPC layer beneath.

TE has no Redfish client of its own.

## Authorized use only

The posture's default-credential check logs in to a real BMC, so it is
**off by default** (`attempt_default_creds`) and must only be enabled
against a device you own or are engaged to assess. Everything else is
read-only.

## What it reads

```c
tapi_redfish_root root;
int http_code;

CHECK_RC(tapi_redfish_get_root(rpcs, "https://10.0.0.1", NULL, NULL,
                               &root, &http_code));
tapi_redfish_root_log(&root);   /* vendor, product, Redfish version, UUID */
tapi_redfish_root_free(&root);
```

`tapi_redfish_get()` fetches any resource (e.g. `/redfish/v1/Systems`);
`tapi_redfish_auth_check()` says whether a user/password authenticates.

## Security posture

`tapi_redfish_audit()` reports through tsf-cybersec's finding model:

| Finding | Severity | Raised when |
|---|---|---|
| `redfish.present` | info | the service root answers |
| `redfish.http-no-tls` | high | the base URL is `http`, not `https` |
| `redfish.default-credentials` | critical | a well-known default (admin/password, root/calvin, …) authenticates — only when `attempt_default_creds` is set |
| `redfish.not-assessed` | info | the service could not be reached |

The subject is the base URL (stable between runs).

## Agent host requirements

- **libcurl** with HTTPS (Debian: `libcurl4-openssl-dev`) and
  **jansson** (`libjansson-dev`). BMCs ship self-signed certificates,
  so the client disables TLS peer verification — it reaches the device,
  it does not vouch for the certificate.

## Usage

Declare the repository in an external libraries catalog and pass it to
`dispatcher.sh --external=<catalog.yml>`:

```yaml
repositories:
  - name: tsf_redfish
    url: https://github.com/interpretica-io/tsf-redfish.git
    ref: <tag>
    libs:
      - ta_redfish
      - rpcs_redfish
      - tapi_redfish
```

In `builder.conf`, bind `tapi_redfish` to the engine, list `ta_redfish`
and `rpcs_redfish` among the RPC server's libraries, and add the RPC
definitions to both platforms:

```
TE_EXT_REPO_USE([tsf_redfish], [], [ta_redfish rpcs_redfish tapi_redfish])
TE_LIB_PARMS([rpcxdr], [${TE_HOST}], [],
             [--with-rpcdefs=tarpc_job.x.m4,../ta_redfish/redfish_rpc.x.m4])
```

`tapi_redfish_audit` reports through tsf-cybersec, so that repository
(and its prerequisites tsf-kernel and tsf-devtool) must be built too.
The RPC program number is **41**.

## What was verified

The agent's **libcurl + jansson usage was syntax-checked against the
real headers** (libcurl 8.5.0, jansson 2.14) with `-fsyntax-only`; every
symbol `ta_redfish.c` uses type-checks. The module builds and runs
through its `tsf-redfish-ts` suite natively (the suite skips cleanly when
no `TSF_REDFISH_URL` is configured). No request was made to a real BMC
here.
