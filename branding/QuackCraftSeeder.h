// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Instance Seeder
// First-run helper that creates the default "QuackCraft" instance
// pre-configured to join the QuackCraft server with the right MC version.
// On the first run, when no instance is named "QuackCraft", it asks the
// Prism Launcher version list to resolve the default MC version and then
// runs a VanillaCreationTask to install the matching vanilla Minecraft.
// Once that finishes, it sets the "server to join" hint on the instance
// so the user can hit Play and connect with one click.
//
// This is the QuackCraft Launcher variant of what PrismLauncher's Setup
// Wizard already does for stock instances, just specialised to our
// server address and version.
#pragma once

#include <QObject>
#include <QString>

class InstanceList;

namespace QuackCraft {

class Seeder : public QObject {
    Q_OBJECT
   public:
    explicit Seeder(InstanceList* list, QObject* parent = nullptr);

    /// Returns true if a "QuackCraft" instance already exists.
    bool isSeeded() const;

    /// Triggers a one-shot creation of the default instance. No-op if it
    /// already exists. The actual install runs asynchronously; safe to
    /// call from the setup-wizard finished slot.
    void seedIfMissing();

    /// Returns the suggested instance name.
    static const char* defaultInstanceName() { return "QuackCraft"; }

    /// The Minecraft version we want to pre-install for the user.
    /// Bump this when the server updates.
    static QString defaultMinecraftVersion() { return QStringLiteral("26.2"); }

    /// The Minecraft server address (host:port) the Quick Play button
    /// should auto-fill on first run.
    static QString defaultServerAddress() { return QStringLiteral("play.quackforge.xyz:25565"); }

   private:
    InstanceList* m_list;
};

}  // namespace QuackCraft
