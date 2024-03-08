#pragma once

#include <QFileDialog>
#include <QWidget>

namespace QtUtil
{
    bool openDirInExplorer(const std::wstring& pathIn);
    bool showFileInExplorer(const std::wstring& pathIn);

    void warningWithSound(QWidget* parent, const QString& title, const QString& message);
    void errorWithSound(QWidget* parent, const QString& title, const QString& message);
    void infoWithSound(QWidget* parent, const QString& title, const QString& message);

    void openItemInSteam(std::uint64_t steam_id);

	bool fileDialog(
        QWidget* parent,
        const QString& title,
        QFileDialog::FileMode file_mode,
        QString& out_file);
}