// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Instance Seeder
// First-run helper that creates the default "QuackCraft" instance
// pre-configured to join the QuackCraft server with the right MC version.
//
// This is currently a STUB that records intent only. The actual instance
// creation is delegated to PrismLauncher's existing VanillaCreationTask /
// ModrinthCreationTask flow, triggered from Application::createSetupWizard().
// Re-enable once those Prism API calls are wired up.
#pragma once

#include <QObject>

class InstanceList;

namespace QuackCraft {

/**
 * Checks whether a default "QuackCraft" instance should be created.
 * Idempotent: returns false if an instance named "QuackCraft" already exists.
 */
class Seeder : public QObject {
    Q_OBJECT
   public:
    explicit Seeder(InstanceList* list, QObject* parent = nullptr);

    /// Returns true if a "QuackCraft" instance already exists.
    bool isSeeded() const;

    /// Returns the suggested instance name.
    static const char* defaultInstanceName() { return "QuackCraft"; }

   private:
    InstanceList* m_list;
};

}  // namespace QuackCraft
