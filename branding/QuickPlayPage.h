// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Quick Play page
// A minimal launcher UI for users who just want to enter a name, pick a
// Minecraft version, and hit Play. No instance management, no mod
// loaders, no Microsoft login — that whole Prism UI is still there in
// the other tabs, but Quick Play is what a brand new user sees first.
//
// On Play, this widget:
//   1. Looks up the MC version the user selected in the Prism version
//      list (we keep the descriptor string, e.g. "26.2").
//   2. If a "QuackCraft" instance is already present (the default
//      seed), uses it. Otherwise creates one on the fly using the
//      VanillaCreationTask, then waits for it to finish.
//   3. Sets the instance's "last server" hint to
//      QuackCraft::Seeder::defaultServerAddress() so it joins the
//      QuackCraft server on launch.
//   4. Calls Application::launch() on the instance.
//
// This file is intentionally self-contained: it knows about
// QuackCraft::Seeder and the few Prism APIs it needs, but does not
// touch any Prism UI plumbing. The MainWindow adds this widget as a
// tab next to the existing instance list.
#pragma once

#include <QString>
#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

namespace QuackCraft {

class QuickPlayPage : public QWidget {
    Q_OBJECT
   public:
    explicit QuickPlayPage(QWidget* parent = nullptr);

    /// Called by the MainWindow when Prism's version list has finished
    /// loading. Refreshes the version dropdown.
    void onVersionListReady();

    /// Returns the in-game name the user typed. Empty string if blank.
    QString inGameName() const { return m_nameEdit ? m_nameEdit->text().trimmed() : QString(); }

    /// Returns the descriptor (e.g. "26.2") of the version the user
    /// selected. Empty string if the version list isn't loaded yet.
    QString selectedVersionDescriptor() const;

    /// Returns the display name of the version the user selected.
    /// Empty string if the version list isn't loaded yet.
    QString selectedVersionDisplayName() const;

   private slots:
    void onPlayClicked();
    void onSettingsClicked();

   private:
    void buildUi();
    void populateVersionDropdown();
    void setBusy(bool busy);

    QLabel* m_logoLabel = nullptr;
    QLabel* m_serverLabel = nullptr;
    QLineEdit* m_nameEdit = nullptr;
    QComboBox* m_versionCombo = nullptr;
    QPushButton* m_playButton = nullptr;
    QPushButton* m_settingsButton = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
};

}  // namespace QuackCraft
