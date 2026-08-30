// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Instance Seeder implementation.
//
// The full seeding logic is intentionally deferred: the actual instance
// creation must go through PrismLauncher's InstanceTask pipeline, which
// requires resolving a Minecraft version from the version list and
// running a creation task asynchronously. We add that hookup after the
// branding layer is verified to build and run cleanly.
//
// For now, the Seeder's job is to (a) report whether the default
// instance already exists and (b) log the parameters that a future
// implementation will use, so the Quick Play widget can be wired up
// against stable constants (defaultMinecraftVersion() /
// defaultServerAddress()) even before the actual install task is plumbed.
#include "QuackCraftSeeder.h"

#include "InstanceList.h"
#include "BaseInstance.h"

#include <QDebug>

namespace QuackCraft {

Seeder::Seeder(InstanceList* list, QObject* parent) : QObject(parent), m_list(list) {}

bool Seeder::isSeeded() const {
    if (!m_list) return false;
    for (int i = 0; i < m_list->count(); ++i) {
        auto* inst = m_list->at(i);
        if (inst && inst->name() == QLatin1String(defaultInstanceName())) {
            return true;
        }
    }
    return false;
}

void Seeder::seedIfMissing() {
    if (isSeeded()) {
        qInfo() << "QuackCraft: default instance already present, skipping seed.";
        return;
    }
    qInfo() << "QuackCraft: no default instance yet — would seed"
            << defaultInstanceName()
            << "on Minecraft" << defaultMinecraftVersion()
            << "with quick-play server" << defaultServerAddress()
            << "(stub: actual install is wired in a follow-up).";
    // TODO: implement by:
    //   1. Waiting for the version list to finish loading.
    //   2. Looking up the BaseVersion whose descriptor() matches
    //      defaultMinecraftVersion().
    //   3. Creating a VanillaCreationTask with that version and the
    //      instance name from defaultInstanceName().
    //   4. Connecting the task's succeeded() signal to a slot that
    //      sets the instance's "last server" to defaultServerAddress().
    //   5. Kicking the task off via ConcurrentTask.
    // The reason this isn't done here yet is that the version list is
    // populated asynchronously and the right place to do all of this is
    // at the end of the setup wizard, after Prism has finished loading
    // its metadata. That's a different signal than we wire today.
}

}  // namespace QuackCraft
