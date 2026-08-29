#!/usr/bin/env python3
"""
Generate QuackCraft launcher icon assets from a single source PNG.

Inputs:
  branding/quackcraft-logo-source.png   1254x1254 RGBA source

Outputs (relative to Launcher/):
  branding/quackcraft-logo.svg          vector wrapper (PNG embedded as <image>)
  program_info/studio.quackcraft.launcher_256.png   256x256 RGBA
  program_info/quackcraftlauncher.ico               multi-size Windows .ico
  program_info/quackcraftlauncher.icns              macOS icon set
  program_info/studio.quackcraft.launcher.svg       big SVG for desktop files
  program_info/studio.quackcraft.launcher.Social.svg   social-card friendly square
  program_info/studio.quackcraft.launcher.bigsur.svg   bigsur-friendly square
  program_info/studio.quackcraft.launcher.logo.svg     1:1 square
  program_info/studio.quackcraft.launcher.logo-darkmode.svg  dark-mode variant
  program_info/studio.quackcraft.launcher.logo.source.svg     source / dev variant

PrismLauncher's SVG files in program_info/ are 1:1 SVGs (not used as actual
icons in the build — the .ico/.icns and .png are). For now we generate
the same set of placeholder SVGs that all reference the same embedded
PNG. This keeps the build happy and the visuals consistent.

Run from the Launcher/ root:
  python branding/generate_icons.py
"""
from __future__ import annotations

import base64
import io
import sys
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "branding" / "quackcraft-logo-source.png"
PROGRAM_INFO = ROOT / "program_info"


def _load_source() -> Image.Image:
    if not SOURCE.is_file():
        sys.exit(f"missing source: {SOURCE}")
    img = Image.open(SOURCE).convert("RGBA")
    print(f"loaded {SOURCE.name}: {img.size} {img.mode}")
    return img


def _write_png(img: Image.Image, path: Path, size: int) -> None:
    resized = img.resize((size, size), Image.LANCZOS)
    resized.save(path, format="PNG", optimize=True)
    print(f"wrote {path.relative_to(ROOT)} ({size}x{size})")


def _write_ico(img: Image.Image, path: Path, sizes: tuple[int, ...]) -> None:
    """Write a multi-size Windows .ico (all sub-images from same source)."""
    images = [img.resize((s, s), Image.LANCZOS) for s in sizes]
    images[0].save(
        path,
        format="ICO",
        sizes=[(s, s) for s in sizes],
        append_images=images[1:],
    )
    print(f"wrote {path.relative_to(ROOT)} (ICO sizes={sizes})")


def _write_icns(img: Image.Image, path: Path) -> None:
    """
    Write a minimal macOS .icns by concatenating PNGs at known sizes.
    Pillow does not natively write ICNS, so we hand-build the container.
    """
    SIZES = (16, 32, 64, 128, 256, 512, 1024)
    PNG_ILOC = 0x69636E70  # 'icp8' / PNG magic — Pillow uses 8-bit direct

    def png_chunk(typ: bytes, data: bytes) -> bytes:
        import struct

        return (
            struct.pack(">I", len(data))
            + typ
            + data
            + struct.pack(">I", 0)  # placeholder CRC; macOS doesn't validate
        )

    blobs: list[bytes] = []
    for s in SIZES:
        buf = io.BytesIO()
        img.resize((s, s), Image.LANCZOS).save(buf, format="PNG", optimize=True)
        ic09 = b"ic09" if s == 512 else (b"ic10" if s == 1024 else (b"ic07" if s == 128 else (b"ic08" if s == 256 else (b"icp4" if s == 16 else (b"icp5" if s == 32 else (b"icp6" if s == 64 else b"ic07"))))))
        # macOS ICNS ostype for raw PNG embedding
        icn_typ = b"ic07" if s <= 256 else (b"ic08" if s == 256 else (b"ic09" if s == 512 else b"ic10"))
        blobs.append(icn_typ + buf.getvalue())

    import struct

    body = b"".join(struct.pack(">I", len(b)) + b for b in blobs)
    header = b"icns" + struct.pack(">I", 8 + len(body))
    path.write_bytes(header + body)
    print(f"wrote {path.relative_to(ROOT)} (ICNS sizes={SIZES})")


def _write_svg_wrapper(out: Path, size: int) -> None:
    """Write a tiny SVG that embeds the source PNG as a base64 <image>."""
    img = Image.open(SOURCE).convert("RGBA").resize((size, size), Image.LANCZOS)
    buf = io.BytesIO()
    img.save(buf, format="PNG", optimize=True)
    b64 = base64.b64encode(buf.getvalue()).decode("ascii")
    svg = (
        f'<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<svg xmlns="http://www.w3.org/2000/svg" '
        f'xmlns:xlink="http://www.w3.org/1999/xlink" '
        f'viewBox="0 0 {size} {size}" width="{size}" height="{size}">\n'
        f'  <image href="data:image/png;base64,{b64}" '
        f'xlink:href="data:image/png;base64,{b64}" '
        f'width="{size}" height="{size}"/>\n'
        f'</svg>\n'
    )
    out.write_text(svg, encoding="utf-8")
    print(f"wrote {out.relative_to(ROOT)} (SVG {size}x{size})")


def main() -> None:
    img = _load_source()
    PROGRAM_INFO.mkdir(parents=True, exist_ok=True)

    # 256 PNG (referenced by program_info/CMakeLists.txt)
    _write_png(img, PROGRAM_INFO / "studio.quackcraft.launcher_256.png", 256)

    # SVG variants (Prism's CMakeLists has templates for each; we keep them
    # all as the same wrapper so the build system is happy)
    for name in (
        "studio.quackcraft.launcher.svg",
        "studio.quackcraft.launcher.logo.svg",
        "studio.quackcraft.launcher.logo-darkmode.svg",
        "studio.quackcraft.launcher.logo.source.svg",
        "studio.quackcraft.launcher.Social.svg",
        "studio.quackcraft.launcher.Source.svg",
        "studio.quackcraft.launcher.bigsur.svg",
    ):
        _write_svg_wrapper(PROGRAM_INFO / name, 256)

    # Big SVG used by Linux hicolor and by the desktop entry
    _write_svg_wrapper(ROOT / "branding" / "quackcraft-logo.svg", 512)

    # Windows .ico (multi-size so it scales nicely in Explorer / taskbar)
    _write_ico(img, PROGRAM_INFO / "quackcraftlauncher.ico", (16, 32, 48, 64, 128, 256))

    # macOS .icns
    _write_icns(img, PROGRAM_INFO / "quackcraftlauncher.icns")

    print("\nall icon assets generated")


if __name__ == "__main__":
    main()
