# HTTPServer (libOskServer)

A personal project to build an HTTP server library in C, from scratch, with **no third-party dependencies** — just the C standard library and Winsock2.

> ⚠️ **Windows only.** The library is built directly on `winsock2.h` and the Makefile uses Windows shell commands (`del`, `if not exist`, etc.), so it currently only builds and runs on Windows.

## Why

This is a learning project aimed at understanding what actually happens under the hood of an HTTP server — parsing raw request bytes off a socket, building responses by hand, and wiring up routing — without reaching for libcurl, libuv, or any other library.

## Features

- Raw TCP socket handling via Winsock2
- HTTP request line, header, and body parsing
- HTTP response construction and serialisation
- Simple routing:
  - Exact-path endpoints (`AddEndpoint`)
  - Single-file endpoints (`AddFileEndpoint`)
  - Static directory serving (`AddStaticDirectoyEndpoint`)
- Configurable logging (`LOG_DEBUG` → `LOG_ERROR`) to any `FILE*` stream
- Ships as a static library (`libOskServer.a`) you link into your own app

## Requirements

- Windows
- [Clang](https://clang.llvm.org/) (the Makefile invokes `clang` directly)
- GNU Make
- Winsock2 (`ws2_32`, part of Windows — linked automatically by the Makefile)

## Building

```sh
# Build the static library (libOskServer.a)
make lib

# Build and run the test suite
make run_tests

# Build the example programs (BasicServer, StaticSite)
make examples

# Clean build artifacts
make clean
```

## Usage

Include the public header and link against `libOskServer.a`:

```c
#include "libOskServer.h"
```

### Minimal server

```c
int OkCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context) {
    (void)server; (void)request; (void)context;
    SetStatus(response, OK);
    return 0;
}

int main() {
    HTTPServer server = {0};

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    AddEndpoint(&server, "/ok", GET, OkCallback);

    StartServer(&server);
    StopServer(&server);

    return 0;
}
```

### Returning HTML

```c
int HTMLCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context) {
    (void)server; (void)request; (void)context;

    char* html =
        "<!DOCTYPE html>\r\n"
        "<html><body><h1>Hello from my C HTTP Server!</h1></body></html>\r\n";

    SetBody(response, html, strlen(html));

    AddHeader(&response->Headers, (FieldLine){
        .FieldName  = (StringView){ .Content = "Content-Type", .Count = 12 },
        .FieldValue = (StringView){ .Content = "text/html",    .Count = 9  }
    });

    SetStatus(response, OK);
    return 0;
}
```

### Echoing a request body (POST)

```c
int PostCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context) {
    (void)server; (void)context;
    SetBody(response, request->Body.Content, request->Body.Count);
    SetStatus(response, OK);
    return 0;
}

// AddEndpoint(&server, "/echo", POST, PostCallback);
```

### Serving a static directory

```c
int main() {
    HTTPServer server = {0};

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    // Files under bin/www are served at /static/<filepath>
    AddStaticDirectoyEndpoint(&server, "/static", "bin/www");

    StartServer(&server);
    StopServer(&server);
}
```

More complete, runnable examples live in [`examples/`](examples/): [`BasicServer.c`](examples/BasicServer.c) and [`StaticSite.c`](examples/StaticSite.c).

## API overview

| Function | Purpose |
|---|---|
| `InitialiseServer(server, port)` | Set up the listening socket on a given port |
| `StartServer(server)` | Start the accept/handle loop (blocking) |
| `StopServer(server)` | Clean up sockets and WSA resources |
| `SetLogging(server, stream, minimumLogLevel)` | Configure where and how much gets logged |
| `AddEndpoint(server, path, method, callback)` | Register a callback for an exact path + method |
| `AddFileEndpoint(server, path, method, filePath)` | Serve a single file from a path |
| `AddStaticDirectoyEndpoint(server, path, directory)` | Serve an entire directory as static files |
| `SetStatus(response, status)` | Set the HTTP status of a response |
| `SetBody(response, body, size)` | Set the response body |
| `AddHeader(headers, fieldLine)` | Add a header to a request/response |
| `GetHeaderValue(headers, name, nameLength)` | Look up a header by name |

See [`include/libOskServer.h`](include/libOskServer.h) for the full public API and struct definitions.

## Project layout

```
include/    Public and internal headers
src/        Library implementation
examples/   Example servers built on top of the library
tests/      Unit tests (parsing, response serialisation, message helpers)
bin/www/    Sample static site used by the StaticSite example
```

## Testing

```sh
make run_tests
```

This builds and runs `tests/TestRunner.c`, which covers HTTP request parsing, response serialisation, and message helpers.

## Status

This is an early-stage personal/learning project. Notable current limitations:

- Windows-only (Winsock2 + Windows batch commands in the Makefile)
- Only `GET`, `POST`, `PUT`, and `DELETE` methods are defined so far (see the `TODO` in `libOskServer.h`)
- `StartServer` currently runs an unbreakable loop — there's no built-in way to gracefully stop the accept loop short of a crash or killing the process

Contributions, issues, and suggestions are welcome.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
