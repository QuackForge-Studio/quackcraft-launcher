# QuackCraft Launcher

A customized Minecraft launcher for the **QuackCraft** server, forked from
[Prism Launcher](https://prismlauncher.org/) (GPL-3.0).

> **Status:** branding layer applied. Build pipeline not yet exercised.
> See `branding/README.md` for the list of changes vs upstream.

## What is this

QuackCraft Launcher is Prism Launcher with:

- Re-branded UI (QuackCraft green + yellow palette, duck logo)
- Renamed AppID, package, env var, and binary
- Repointed news / wiki / bug-tracker / meta URLs to `quackcraft.studio`
- A stub `branding/QuackCraftSeeder` that detects/creates the default
  "QuackCraft" instance
- Theme override (QuackCraft colors) at `branding/theme.qss`

The Prism Launcher core (Forge/Fabric/Quilt pipelines, Mojang download,
Java detection, modrinth/curseforge, etc.) is **unchanged**.

## Upstream

- **Prism Launcher:** <https://github.com/PrismLauncher/PrismLauncher>
- **License:** GPL-3.0 — see [`LICENSE`](./LICENSE) (the upstream
  PrismLauncher `LICENSE` is preserved verbatim)
- **Attribution:** QuackCraft Launcher is a derivative work. All
  original Prism Launcher, PolyMC, and MultiMC copyright notices remain
  in their respective source files.

Per GPL-3.0 §5, this fork's source is available and modifications are
marked. Per §6, any user receiving this binary is entitled to the
corresponding source.

## License of modifications

All files under `branding/` and the patches to the Prism source tree
are:

```
SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 2026 QuackForge Studio
```

…in addition to (not in place of) the upstream copyright headers,
which are preserved in every file we have modified.

## Building

This project inherits Prism Launcher's build system. See upstream
[build instructions](https://prismlauncher.org/wiki/development/build-instructions) — the
commands are unchanged. You will need:

- Qt 6.4 or newer
- CMake 3.25+
- A C++23 compiler (MSVC 17.6+, GCC 12+, or Clang 15+)
- Platform deps: see upstream wiki

On Windows, the recommended build sequence is:

```cmd
cd Launcher
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Configuration of branding values

Edit [`branding/colors.h`](./branding/colors.h) to change:

| Symbol | Purpose |
|---|---|
| `APP_ID` / `APP_NAME` | Must match `program_info/CMakeLists.txt` |
| `DEFAULT_SERVER_HOST` / `DEFAULT_SERVER_PORT` | "Quick Play" target |
| `DEFAULT_MINECRAFT_VERSION` | The version the default instance installs |
| Color tokens | Palette consumed by `branding/theme.qss` |

After editing, re-run `cmake -B build` so `BuildConfig` regenerates.

## Outstanding work before first public release

1. Replace placeholder MSA client ID and CurseForge API key in
   `CMakeLists.txt` (currently upstream Prism keys — see comments in that
   file).
2. Generate real `.ico` (Windows) and `.icns` (macOS) from
   `branding/quackcraft-logo.svg`.
3. Replace `program_info/studio.quackcraft.launcher_256.png` with a real
   256x256 PNG (currently the SVG was copied as a placeholder).
4. Wire `branding/QuackCraftSeeder` into `Application::createSetupWizard`
   so the default instance is actually created on first run.
5. Build & smoke-test on Windows / macOS / Linux.
6. Publish a GitHub release with the binary artifacts.

## Upstream policy that applies to this fork

We are following the upstream forking policy (preserved verbatim from
the original `README.md` below for attribution):

> You are free to fork, redistribute and provide custom builds as long
> as you follow the terms of the [license](LICENSE), and if you made
> code changes rather than just packaging a custom build, please do
> the following as a basic courtesy:
> - Make it clear that your fork is not Prism Launcher and is not
>   endorsed by or affiliated with the Prism Launcher project.
> - Go through `CMakeLists.txt` and change Prism Launcher's API keys
>   to your own or set them to empty strings (`""`).

## Repository layout

```
Launcher/
├── branding/                    QuackCraft-only additions (NEW)
│   ├── colors.h                 single source of truth for palette + ids
│   ├── theme.qss                QuackCraft Qt stylesheet
│   ├── quackcraft-logo.svg      launcher logo
│   ├── QuackCraftSeeder.h/cpp   first-run default-instance helper (stub)
│   └── README.md
├── launcher/                    Prism source (mostly untouched)
├── program_info/                Re-branded (AppID rename, copyright prepend)
├── libraries/                   Prism vendored libs (unchanged)
├── buildconfig/                 build-time config (unchanged)
├── CMakeLists.txt               URLs + API key notes updated
└── README.md                    this file
```
