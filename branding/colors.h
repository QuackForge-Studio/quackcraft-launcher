// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Launcher Branding
// Based on Prism Launcher (GPL-3.0)
// Copyright (C) 2026 QuackForge Studio
//
#pragma once

#include <QString>

namespace QuackCraft {

// QuackCraft brand color tokens
// Primary: deep quack green (rubber-duck inspired)
// Accent:  bright yellow (quack accent)
namespace Colors {
inline constexpr const char* PRIMARY       = "#0E7C66";  // deep teal-green
inline constexpr const char* PRIMARY_DARK  = "#075C4A";  // darker variant
inline constexpr const char* PRIMARY_LIGHT = "#3FA38B";  // hover / highlight
inline constexpr const char* ACCENT        = "#F5B700";  // duck yellow
inline constexpr const char* ACCENT_DARK   = "#C49000";
inline constexpr const char* BG_DARK       = "#0F1419";  // near-black
inline constexpr const char* BG_PANEL      = "#1A1F26";  // panel surface
inline constexpr const char* FG_PRIMARY    = "#F5F7FA";  // text
inline constexpr const char* FG_MUTED      = "#9BA4B0";  // muted text
inline constexpr const char* BORDER        = "#2A323D";  // borders
inline constexpr const char* SUCCESS       = "#3FB950";
inline constexpr const char* WARNING       = "#D29922";
inline constexpr const char* DANGER        = "#F85149";
}  // namespace Colors

// App identity (must stay in sync with program_info/CMakeLists.txt)
inline constexpr const char* APP_ID          = "studio.quackcraft.launcher";
inline constexpr const char* APP_NAME        = "QuackCraft Launcher";
inline constexpr const char* APP_SHORT_NAME  = "QuackCraft";
inline constexpr const char* APP_DOMAIN      = "quackcraft.studio";

// QuackCraft default server (change when production server is ready)
inline constexpr const char* DEFAULT_SERVER_HOST = "play.quackcraft.studio";
inline constexpr int           DEFAULT_SERVER_PORT = 25565;

// Default Minecraft version for the QuackCraft instance.
// MUST match the Paper/Purpur server version in /Server.
// Update both sides when bumping.
inline constexpr const char* DEFAULT_MINECRAFT_VERSION = "1.21.1";

}  // namespace QuackCraft
