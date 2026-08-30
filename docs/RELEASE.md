# Release Process

This document explains how to cut a public QuackCraft Launcher release from
the `main` branch. The end-to-end flow takes about 1 day of hands-on time
plus 30-90 minutes of CI build time per platform.

## 1. Prerequisites

Before you can cut a release, the following must be true:

- CI on `main` is **green** for all 4 build targets (Linux, macOS,
  Windows-MinGW, Windows-MSVC). Check <https://github.com/QuackForge-Studio/quackcraft-launcher/actions>.
- A `play.quackforge.xyz` domain is owned and DNS-resolvable, with an
  A/AAAA record pointing to your distribution host (see step 6).
- A GitHub PAT or `GITHUB_TOKEN` with `contents: write` on the
  `QuackForge-Studio/quackcraft-launcher` repo.
- The 4 platform build artifacts from the most recent successful
  `main` workflow run are still in your browser (or you can re-run the
  workflow and download them fresh).

## 2. Pick a version number

QuackCraft Launcher follows `MAJOR.MINOR.PATCH` (semver) starting at
`0.1.0`. For now, before `1.0`, bump MINOR for any feature that ships
and PATCH for bugfix-only releases. Do not break `0.x` API/ABI without
bumping MAJOR.

The first public release should be `0.1.0`.

## 3. Tag the release

```bash
cd Launcher
git fetch origin
git checkout main
git pull --ff-only
git tag -s 0.1.0 -m "QuackCraft Launcher 0.1.0"
git push origin 0.1.0
```

We sign the tag (GPG `-s`) so users can `git verify-tag 0.1.0` if
they clone the repo to build from source. If you don't have a GPG
key set up yet, drop the `-s` for now and add a TODO to set one up
before the 1.0 release.

## 4. Run the release workflow

We do not yet have a `release.yml` workflow — the existing
`build.yml` is what builds binaries. To turn a green `main` build
into release artifacts:

1. Open <https://github.com/QuackForge-Studio/quackcraft-launcher/actions/workflows/build.yml>.
2. Click "Run workflow" → branch `main` → "Run workflow".
3. Wait for all 4 jobs to finish. Total time: 20-60 minutes (vcpkg
   will compile 6 deps from source the first time; subsequent runs
   hit the vcpkg binary cache and drop to ~5-10 minutes per platform).
4. For each green job, click into it and download the
   `quackcraft-<Platform>` artifact at the bottom of the page.

You now have 4 ZIP files locally. Unpack them; each one contains a
single binary (`quackcraftlauncher` on Linux/macOS,
`quackcraftlauncher.exe` on Windows).

## 5. Smoke-test the binaries

Before publishing, install + run each binary on a clean VM (or at
least a clean user account) and verify:

- [ ] App launches in < 2 seconds
- [ ] QuackForge duck icon shows up in the title bar / task bar
- [ ] Theme is green/yellow, not Prism blue
- [ ] `play.quackforge.xyz` is the URL shown in Settings → General
    (Help, News, etc.)
- [ ] First-run wizard runs and finishes
- [ ] You can install a vanilla Minecraft 26.2 instance and launch it
- [ ] Java downloader works (or system Java is detected)

If anything is broken, **do not** publish. Tag a `0.1.1-rc1` on a
fix branch, re-run the matrix, smoke-test again.

## 6. Publish the release

1. Go to <https://github.com/QuackForge-Studio/quackcraft-launcher/releases/new>.
2. Choose tag `0.1.0`.
3. Title: `QuackCraft Launcher 0.1.0 — Initial Public Release`.
4. Description: write release notes in the format below.
5. Upload the 4 binary artifacts (you can drag-drop ZIPs from step 4).
6. Click "Publish release".

**Release notes template:**

```markdown
## QuackCraft Launcher 0.1.0

First public build of QuackCraft Launcher, a fork of
[Prism Launcher](https://prismlauncher.org) (GPL-3.0) repackaged for
the QuackCraft community.

### What works
- Simple Quick Play UI: enter in-game name, pick a Minecraft
  version, hit the big Play button
- Optional offline play (no Microsoft login required)
- Pre-seeded "QuackCraft" instance that joins `play.quackforge.xyz:25565`
  on first run (advanced users can disable or delete it)
- Full Prism Launcher under the hood: open Settings to access
  Fabric / Forge / Quilt mod loader, Modrinth / CurseForge / FTB mod
  browsing, multi-instance management, etc.

### What's not in this build yet
- Automatic update notifications (auto-update will land in 0.2)
- Sparkle / WinSparkle update feed
- Code-signed binaries (we'll start signing Windows in 0.2 and
  macOS in 0.3 — for now, macOS users will need to right-click →
  Open the first time)

### Downloads
| Platform  | File                       | Size   |
|-----------|----------------------------|--------|
| Linux     | quackcraft-Linux.zip       | ~50 MB |
| macOS     | quackcraft-macOS.zip       | ~60 MB |
| Windows   | quackcraft-Windows-MSVC.zip| ~70 MB |

### Notes
- Linux build is glibc 2.39+ (Ubuntu 24.04). If you're on an older
  distro, install the legacy glibc compatibility pack or use the AppImage
  (we'll ship an AppImage starting in 0.2).
- Windows-MSVC is the recommended Windows build. Windows-MinGW is
  provided for users who specifically need it; expect slower startup.
- All builds include Java 21. If you have Java 17 installed system-wide,
  the launcher will use that instead.

GPL-3.0. See [LICENSE](../LICENSE) for full text.
```

## 7. Update the website

If the `Web/` side of the monorepo has a download page that points to
"latest release", make sure the GitHub release URL is what it's
expecting. The default GitHub URL
`https://github.com/QuackForge-Studio/quackcraft-launcher/releases/latest`
is what most sites link to.

## 8. Announce

Post the release in:
- QuackCraft Discord `#launcher-updates` channel
- QuackCraft email newsletter (if you have one)
- A short post on `play.quackforge.xyz` blog

## 9. Post-release monitoring

For the first 48 hours after release, keep an eye on:
- GitHub Issues for crash reports
- Discord `#launcher-help` for installation problems
- Telemetry from `play.quackforge.xyz/launcher-stats` (if you've set it up)

If a P0 bug shows up, cut `0.1.1` immediately and re-run the matrix.

---

## Things still missing before we can ship

This document is written assuming a fully working pipeline. Right now
there are still a few pieces that are not yet in place:

- [ ] **CI is green** — currently the matrix fails at the CMake
      Configure step. We added diagnostics on `a10c65f`; check the
      next CI run to see the actual error.
- [ ] **A working `release.yml` workflow** that produces the 4
      artifacts automatically when a tag is pushed. Right now you
      trigger the build manually and download artifacts by hand.
- [ ] **Code signing** for Windows (Authenticode) and macOS
      (Developer ID). Without it, Windows SmartScreen will warn
      users and macOS will require right-click → Open.
- [ ] **AppImage / NSIS installer packaging** on Linux and Windows.
      We currently ship a raw binary, which is fine for early access
      but not user-friendly.
- [ ] **A `play.quackforge.xyz` landing page** that the launcher can
      pull news/help/issue-tracker URLs from. The rebranding work
      points everything at this domain; the domain itself has to
      exist for the URLs to resolve.

None of these block getting a tarball out to a handful of alpha
testers, but they all need to be done before a public "press the
big green button" release.
