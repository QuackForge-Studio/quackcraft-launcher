// SPDX-License-Identifier: GPL-3.0-only
//
// QuackCraft Quick Play page implementation.
//
// This is intentionally the simplest piece of UI in the launcher:
// one text field, one dropdown, two buttons. No tree of accounts,
// no mod loader selector, no news feed. Everything Prism can do
// is still available in the other tabs; Quick Play is just the
// happy path for "I want to play Minecraft, give me a name and
// a version and get me in".
#include "QuickPlayPage.h"
#include "QuackCraftSeeder.h"

#include "Application.h"
#include "BuildConfig.h"
#include "InstanceList.h"
#include "BaseInstance.h"
#include "InstanceTask.h"
#include "QObjectPtr.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/VanillaInstanceCreationTask.h"
#include "meta/Index.h"
#include "meta/VersionList.h"
#include "meta/Version.h"
#include "ui/dialogs/ProgressDialog.h"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QVBoxLayout>

namespace QuackCraft {

namespace {

QIcon makeLogoIcon(int size)
{
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    // Soft yellow ring on green circle, evocative of the QuackCraft
    // duck logo. We don't have access to the resource system here,
    // so we paint a procedural placeholder if the asset isn't on
    // disk yet. The MainWindow swaps in the real PNG later.
    QLinearGradient grad(QPointF(0, 0), QPointF(0, size));
    grad.setColorAt(0.0, QColor("#1f8f4e"));
    grad.setColorAt(1.0, QColor("#0f5e2f"));
    p.setBrush(grad);
    p.setPen(QColor("#ffcf3a"));
    p.drawEllipse(2, 2, size - 4, size - 4);
    p.setPen(QColor("#ffcf3a"));
    QFont f = p.font();
    f.setPointSize(size / 3);
    f.setBold(true);
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, "Q");
    return QIcon(pm);
}

}  // namespace

QuickPlayPage::QuickPlayPage(QWidget* parent) : QWidget(parent)
{
    buildUi();
    populateVersionDropdown();
}

void QuickPlayPage::buildUi()
{
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(48, 48, 48, 48);
    m_rootLayout->setSpacing(24);
    m_rootLayout->setAlignment(Qt::AlignCenter);

    // Top: logo + brand name
    auto* header = new QHBoxLayout();
    header->setAlignment(Qt::AlignCenter);
    m_logoLabel = new QLabel(this);
    m_logoLabel->setPixmap(makeLogoIcon(96).pixmap(96, 96));
    m_logoLabel->setFixedSize(96, 96);
    auto* titleBlock = new QVBoxLayout();
    titleBlock->setSpacing(0);
    auto* title = new QLabel(QStringLiteral("QuackCraft Launcher"), this);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 12);
    titleFont.setBold(true);
    title->setFont(titleFont);
    auto* tagline = new QLabel(
        QStringLiteral("One name. One button. See you in the swamp."),
        this);
    tagline->setStyleSheet("color: rgba(255,255,255,0.6);");
    titleBlock->addWidget(title);
    titleBlock->addWidget(tagline);
    header->addWidget(m_logoLabel);
    header->addSpacing(16);
    header->addLayout(titleBlock);
    m_rootLayout->addLayout(header);

    // Middle: name + version
    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setHorizontalSpacing(20);
    form->setVerticalSpacing(20);

    auto* nameLabel = new QLabel(QStringLiteral("In-game name"), this);
    nameLabel->setStyleSheet("font-weight: 600;");
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("Steve"));
    m_nameEdit->setMaxLength(16);
    m_nameEdit->setMinimumWidth(360);
    QFont nameFont = m_nameEdit->font();
    nameFont.setPointSize(nameFont.pointSize() + 2);
    m_nameEdit->setFont(nameFont);
    m_nameEdit->setClearButtonEnabled(true);
    form->addRow(nameLabel, m_nameEdit);

    auto* versionLabel = new QLabel(QStringLiteral("Minecraft version"), this);
    versionLabel->setStyleSheet("font-weight: 600;");
    m_versionCombo = new QComboBox(this);
    m_versionCombo->setMinimumWidth(360);
    QFont versionFont = m_versionCombo->font();
    versionFont.setPointSize(versionFont.pointSize() + 2);
    m_versionCombo->setFont(versionFont);
    form->addRow(versionLabel, m_versionCombo);

    m_rootLayout->addLayout(form);

    // Bottom: Play + Settings, with the server hint underneath
    auto* bottom = new QHBoxLayout();
    bottom->setAlignment(Qt::AlignCenter);
    bottom->setSpacing(16);

    m_playButton = new QPushButton(QStringLiteral("▶  Play"), this);
    QFont playFont = m_playButton->font();
    playFont.setPointSize(playFont.pointSize() + 4);
    playFont.setBold(true);
    m_playButton->setFont(playFont);
    m_playButton->setMinimumSize(220, 64);
    m_playButton->setCursor(Qt::PointingHandCursor);
    m_playButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1f8f4e; color: white;"
        "  border: 2px solid #ffcf3a; border-radius: 12px;"
        "}"
        "QPushButton:hover  { background-color: #2aa35e; }"
        "QPushButton:pressed{ background-color: #157638; }"
        "QPushButton:disabled{ background-color: #555555; color: #aaaaaa; }"
    );
    connect(m_playButton, &QPushButton::clicked, this, &QuickPlayPage::onPlayClicked);

    m_settingsButton = new QPushButton(QStringLiteral("⚙"), this);
    m_settingsButton->setFixedSize(48, 48);
    m_settingsButton->setToolTip(QStringLiteral("Open Prism settings (instances, Java, mods)"));
    connect(m_settingsButton, &QPushButton::clicked, this, &QuickPlayPage::onSettingsClicked);

    bottom->addWidget(m_playButton);
    bottom->addWidget(m_settingsButton);
    m_rootLayout->addLayout(bottom);

    // Server hint
    m_serverLabel = new QLabel(this);
    m_serverLabel->setAlignment(Qt::AlignCenter);
    m_serverLabel->setText(QStringLiteral("Server: %1").arg(Seeder::defaultServerAddress()));
    m_serverLabel->setStyleSheet("color: rgba(255,255,255,0.55); font-size: 11px;");
    m_rootLayout->addWidget(m_serverLabel);

    // Stretch so the whole thing sits in the middle
    m_rootLayout->addStretch(1);

    // Apply a simple dark background so the white text is legible
    // regardless of what theme Prism is using.
    setStyleSheet(
        "QuickPlayPage {"
        "  background-color: #1a1a1a;"
        "  color: #f0f0f0;"
        "}"
        "QuickPlayPage QLineEdit, QuickPlayPage QComboBox {"
        "  background-color: #2a2a2a; color: #f0f0f0;"
        "  border: 1px solid #3a3a3a; border-radius: 6px; padding: 8px;"
        "  selection-background-color: #1f8f4e;"
        "}"
    );

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void QuickPlayPage::populateVersionDropdown()
{
    if (!m_versionCombo)
        return;
    m_versionCombo->clear();

    // For the MVP, we only seed the dropdown with the configured
    // default version. The version list isn't a clean dependency
    // to pull in from here (it lives in a different module and
    // loads asynchronously on first launch), so we keep the
    // dropdown editable — the user can type any MC version they
    // have a Prism instance for, and we'll just pass the
    // descriptor through to Prism's launch path. A future
    // enhancement can hook into Prism's MetadataManager and
    // populate the full list once it's loaded.
    m_versionCombo->addItem(Seeder::defaultMinecraftVersion());
    m_versionCombo->setEditable(true);
    m_versionCombo->setToolTip(
        QStringLiteral("Type the version descriptor (e.g. 26.2, 1.21.4) of the instance you want to launch."));
}

QString QuickPlayPage::inGameName() const
{
    return m_nameEdit ? m_nameEdit->text().trimmed() : QString();
}

void QuickPlayPage::onVersionListReady()
{
    populateVersionDropdown();
}

QString QuickPlayPage::selectedVersionDescriptor() const
{
    if (!m_versionCombo || m_versionCombo->count() == 0)
        return {};
    return m_versionCombo->currentText();
}

QString QuickPlayPage::selectedVersionDisplayName() const
{
    return selectedVersionDescriptor();
}

void QuickPlayPage::setBusy(bool busy)
{
    m_playButton->setEnabled(!busy);
    m_versionCombo->setEnabled(!busy);
    m_nameEdit->setEnabled(!busy);
    m_playButton->setText(busy ? QStringLiteral("Preparing…") : QStringLiteral("▶  Play"));
}

void QuickPlayPage::onPlayClicked()
{
    if (!APPLICATION || !APPLICATION->instances())
        return;

    const QString name = inGameName();
    if (name.isEmpty()) {
        m_nameEdit->setFocus();
        m_nameEdit->setPlaceholderText(QStringLiteral("Pick a name to continue"));
        return;
    }

    setBusy(true);

    // Find or create the QuackCraft instance. If it doesn't exist yet,
    // we spin up a VanillaCreationTask for the requested MC version and
    // run it in a modal progress dialog (the same dialog Prism uses for
    // "Add Instance"). Once it's done, we launch.
    BaseInstance* inst = nullptr;
    for (int i = 0; i < APPLICATION->instances()->count(); ++i) {
        auto* candidate = APPLICATION->instances()->at(i);
        if (candidate && candidate->name() == QLatin1String(Seeder::defaultInstanceName())) {
            inst = candidate;
            break;
        }
    }

    if (!inst) {
        const QString descriptor = selectedVersionDescriptor();
        if (descriptor.isEmpty()) {
            setBusy(false);
            return;
        }

        auto* index = APPLICATION->metadataIndex();
        if (!index) {
            qWarning() << "QuackCraft: metadata index not ready; cannot create instance.";
            setBusy(false);
            return;
        }
        auto vlist = index->get("net.minecraft");
        if (!vlist) {
            qWarning() << "QuackCraft: net.minecraft version list missing.";
            setBusy(false);
            return;
        }
        auto version = vlist->getVersion(descriptor);

        auto* task = new VanillaCreationTask(version);
        task->setName(QLatin1String(Seeder::defaultInstanceName()));
        task->setIcon("default");

        // wrapInstanceTask handles the instance directory + id assignment.
        unique_qobject_ptr<Task> wrapped(APPLICATION->instances()->wrapInstanceTask(task));
        ProgressDialog loadDialog(window());
        loadDialog.setSkipButton(true, tr("Abort"));
        loadDialog.execWithTask(wrapped.get());
        if (!wrapped->isFinished()) {
            qWarning() << "QuackCraft: instance creation was aborted or failed.";
            setBusy(false);
            return;
        }

        // Re-find the instance now that it should exist.
        for (int i = 0; i < APPLICATION->instances()->count(); ++i) {
            auto* candidate = APPLICATION->instances()->at(i);
            if (candidate && candidate->name() == QLatin1String(Seeder::defaultInstanceName())) {
                inst = candidate;
                break;
            }
        }
        if (!inst) {
            qWarning() << "QuackCraft: instance creation finished but instance not found.";
            setBusy(false);
            return;
        }
    }

    // Hand the launch off to Prism. The "in-game name" param goes via
    // the offline-name flag, which works without a Microsoft login.
    auto* mcInstance = dynamic_cast<MinecraftInstance*>(inst);
    if (!mcInstance) {
        qWarning() << "QuackCraft: instance" << inst->name()
                   << "is not a Minecraft instance; cannot quick-play.";
        setBusy(false);
        return;
    }
    APPLICATION->launch(mcInstance, LaunchMode::Offline, nullptr, nullptr, name);
    setBusy(false);
}

void QuickPlayPage::onSettingsClicked()
{
    // We don't own Prism's settings dialog directly. The simplest
    // hook is to ask Application to show it. Application already has
    // a slot for that on its main window path, but for now we just
    // open a bare SettingsDialog with the current settings handle.
    // A real version would route through MainWindow::onSettings().
    if (APPLICATION && APPLICATION->settings()) {
        // No-op for now; the MainWindow toolbar's Settings action is
        // what users will reach. This button is just a hint that it
        // exists. Marked so we don't add dead UI.
        qInfo() << "QuackCraft: settings button clicked — use the toolbar Settings action.";
    }
}

}  // namespace QuackCraft
