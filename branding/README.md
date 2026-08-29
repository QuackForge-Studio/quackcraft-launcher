# QuackCraft Launcher — Branding Layer

This folder contains the QuackCraft-specific branding layer applied on top
of the upstream **Prism Launcher** (GPL-3.0) codebase located in the
parent `Launcher/` directory.

## Files in this folder

| File | Purpose |
|---|---|
| `colors.h` | Brand color tokens + app identity constants (AppID, server host, default MC version). Single source of truth referenced from `program_info/CMakeLists.txt`. |
| `theme.qss` | Qt stylesheet that overrides Prism's default theme with QuackCraft colors. Loaded via *Settings → Themes → Custom theme*. |
| `quackcraft-logo.svg` | Vector logo. Replaces `program_info/org.prismlauncher.PrismLauncher.svg`. |

## What is NOT in this folder

- Patches to Prism source files (`launcher/Application.cpp`, etc.) live in the parent `Launcher/` tree.
- Build configuration overrides live in `program_info/CMakeLists.txt` and the root `CMakeLists.txt`.
- The default QuackCraft instance template lives in `launcher/instances/quackcraft.json` (see implementation plan).

## Updating branding

1. Edit `colors.h` if you change palette tokens, app ID, or default server.
2. Edit `theme.qss` for UI styling. Reload via *Settings → Themes*.
3. Replace `quackcraft-logo.svg` to update the launcher icon. Re-run `cmake` so `*.ico`/`*.icns` regen targets pick it up.

## License

This branding layer is part of **QuackCraft Launcher** and is licensed
under **GPL-3.0** to match the upstream Prism Launcher. See
[`LICENSE`](../LICENSE) at the repo root for the full text and the
required attribution notice.
