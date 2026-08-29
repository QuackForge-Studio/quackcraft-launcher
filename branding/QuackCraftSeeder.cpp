// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Instance Seeder implementation (stub).
// The full seeding logic is intentionally deferred: the actual instance
// creation must go through PrismLauncher's InstanceTask pipeline, which
// requires resolving a Minecraft version from the version list and
// running a creation task asynchronously. We add that hookup after the
// branding layer is verified to build and run cleanly.
#include "QuackCraftSeeder.h"

#include "InstanceList.h"
#include "BaseInstance.h"

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

}  // namespace QuackCraft
