#include "QtUtil.hpp"

#include <QMessageBox>
#include <QProcess>

#if defined(_WIN32)
#include "Utils/WinInclude.hpp"
#pragma comment(lib, "Winmm.lib")
#endif

bool QtUtil::openDirInExplorer(const std::wstring& pathIn)
{
	return QProcess::startDetached("explorer.exe", QStringList{ QString::fromStdWString(pathIn) });
}

bool QtUtil::showFileInExplorer(const std::wstring& pathIn)
{
	return QProcess::startDetached("explorer.exe", QStringList{ "/select", ",", QString::fromStdWString(pathIn) });
}

void QtUtil::warningWithSound(QWidget* parent, const QString& title, const QString& message)
{
#if defined(_WIN32)
	PlaySoundA("SystemAsterisk", NULL, SND_ASYNC);
#endif

	QMessageBox::warning(parent, title, message, QMessageBox::StandardButton::Ok);
}

void QtUtil::errorWithSound(QWidget* parent, const QString& title, const QString& message)
{
#if defined(_WIN32)
	PlaySoundA("SystemHand", NULL, SND_ASYNC);
#endif

	QMessageBox::critical(parent, title, message, QMessageBox::StandardButton::Ok);
}

void QtUtil::infoWithSound(QWidget* parent, const QString& title, const QString& message)
{
#if defined(_WIN32)
	PlaySoundA("SystemAsterisk", NULL, SND_ASYNC);
#endif

	QMessageBox::information(parent, title, message, QMessageBox::StandardButton::Ok);
}

bool QtUtil::fileDialog(
	QWidget* parent,
	const QString& title,
	QFileDialog::FileMode file_mode,
	QString& out_file)
{
	QFileDialog v_dialog(parent);
	v_dialog.setWindowTitle(title);
	v_dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
	v_dialog.setFileMode(file_mode);

	if (v_dialog.exec() != QDialog::Accepted)
		return false;

	QStringList v_sel_files = v_dialog.selectedFiles();
	if (v_sel_files.empty())
		return false;

	out_file = v_sel_files[0];
	return true;
}