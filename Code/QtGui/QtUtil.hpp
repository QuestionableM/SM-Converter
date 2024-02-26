#pragma once

#include <QWidget>

namespace QtUtil
{
    bool openDirInExplorer(const std::wstring& pathIn);
    bool showFileInExplorer(const std::wstring& pathIn);

    void warningWithSound(QWidget* parent, const QString& title, const QString& message);
    void errorWithSound(QWidget* parent, const QString& title, const QString& message);
    void infoWithSound(QWidget* parent, const QString& title, const QString& message);
}