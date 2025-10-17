# Argon WebKit Client

**Argon WebKit Client** is a native GTK/WebKit2GTK application that hosts **Argon Chat** as a desktop client. It embeds the prebuilt Vue-based Argon frontend directly into the application using GLib resources, eliminating the need for an external web server.

```
┌─────────────────────────────────────┐
│  Argon WebKit Client (C)            │
│  ├─ GTK3 + WebKit2GTK               │
│  ├─ Custom URI scheme: argon://     │
│  └─ Embedded GResource              │
│            ↓                        │
│  ┌───────────────────────────────┐  │
│  │  Argon frontend (Vue build)   │  │
│  │  └─ dist/                     │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

The frontend is loaded locally from embedded resources, enabling fully offline operation without network requests or local HTTP servers.

---

## Build Requirements

Install the necessary build and runtime dependencies:

```bash
sudo apt install build-essential cmake pkg-config \
    libgtk-3-dev libwebkit2gtk-4.1-dev libjavascriptcoregtk-4.1-dev \
    libglib2.0-dev
```

---

## Quick Start

1. **Clone the repository and initialize the frontend submodule:**
   ```bash
   git clone https://github.com/yazmeyaa/argon-webkit-client.git
   cd argon-webkit-client
   git submodule update --init --recursive --remote
   ```

2. **Build the project:**
   ```bash
   make all
   ```

3. **Run the application:**
   ```bash
   ./argon_webkit_client
   ```

The application will launch with the full Argon Chat interface running locally.

---

## Build Process

The `make all` command performs these steps:

1. Updates the frontend submodule (`client/`) to the latest commit on the `build` branch of the [argon-client](https://github.com/yazmeyaa/argon-client) repository
2. Generates `argon.resources.xml` from the `client/dist` directory using `scripts/generate_resources.sh`
3. Compiles the resource bundle with `glib-compile-resources` to create `client.gresource`
4. Builds the native GTK/WebKit binary `argon_webkit_client`

---

## Key Features

- **Self-contained** desktop wrapper for Argon Chat
- Uses **GTK3 + WebKit2GTK 4.1** for reliable web rendering
- Embeds the complete frontend build via GLib resources
- **Fully offline-capable** with no network dependencies
- **Web Inspector** enabled for debugging (right-click → Inspect Element)

---

## Custom URI Scheme

The application registers a custom `argon://` URI handler that maps to embedded frontend resources. For example:

```
argon:///index.html  →  /argon_build/index.html in GResource
```

This enables seamless access to all frontend assets without requiring a web server.

---

## Developer Commands

```bash
make all              # Full build (update submodule + compile resources + binary)
make embed            # Rebuild only the GResource bundle
make update-submodule # Manually update the frontend submodule
make clean            # Remove build artifacts
```

---

## Repository Structure

```
argon-webkit-client/
├── client/           # Git submodule: argon-client (build branch with prebuilt dist/)
├── src/              # C source code
│   └── main.c
├── scripts/          # Build scripts
│   └── generate_resources.sh
├── Makefile          # Automated build system
├── resources.xml.in  # Resource template
└── README.md
```

The `client` directory is a git submodule pointing to the `build` branch of [argon-client](https://github.com/yazmeyaa/argon-client), which contains the prebuilt Vue frontend in `dist/`.

---

## License

Distributed under the same license as the main Argon Chat project unless otherwise specified.
