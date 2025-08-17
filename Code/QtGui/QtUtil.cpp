#include "QtUtil.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"

#include <QDesktopServices>
#include <QMessageBox>
#include <QProcess>

#if defined(_WIN32)
#include "Utils/WinInclude.hpp"
#pragma comment(lib, "Winmm.lib")
#endif

bool QtUtil::openDirInExplorer(const std::wstring_view& pathIn)
{
	return QProcess::startDetached("explorer.exe",
		QStringList{ QString::fromWCharArray(pathIn.data(), pathIn.size()) });
}

bool QtUtil::showFileInExplorer(const std::wstring_view& pathIn)
{
	QStringList v_params;
	v_params += QLatin1String("/select,");
	v_params += QDir::toNativeSeparators(QString::fromWCharArray(pathIn.data(), pathIn.size()));

	return QProcess::startDetached("explorer.exe", v_params);
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

void QtUtil::openItemInSteam(std::uint64_t steam_id)
{
	if (!steam_id)
		return;

	QString v_workshopUrl;
	if (DatabaseConfig::OpenLinksInSteam)
		v_workshopUrl.append("steam://openurl/");

	v_workshopUrl.append("https://steamcommunity.com/sharedfiles/filedetails/?id=");
	v_workshopUrl.append(QString::number(steam_id, 10));

	QDesktopServices::openUrl(v_workshopUrl);
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