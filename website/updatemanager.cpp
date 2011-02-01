#include <QMessageBox>
#include <QDebug>
#include <QVariantMap>
#include <QCoreApplication>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QApplication>

#include "sitemanager.h"
#include "updatemanager.h"
#include "updatenotificationdialog.h"
#include "file/openfilemanager.h"

UpdateManager::UpdateManager(QObject *parent) :
    QObject(parent)
{
	connect(gSiteManager, SIGNAL(updateAvailable(const QString&, const QVariantMap&)), this, SLOT(updateFound(const QString&, const QVariantMap&)));
	connect(gSiteManager, SIGNAL(noUpdateAvailable()), this, SLOT(noUpdateFound()));
}

void UpdateManager::updateFound(const QString& version, const QVariantMap& changes)
{
	QString url = QString("%1download/").arg(SITE_URL);
	QString ext;
#ifdef Q_OS_WIN
	ext = "exe";
#elif defined Q_OS_OSX
	ext = "dmg";
#endif

	QString fullURL = QString("%1files/PonyEdit-%2.%3").arg(SITE_URL, version, ext);

	mNotificationDlg = new UpdateNotificationDialog();

	connect(mNotificationDlg, SIGNAL(downloadAndInstall(QString)), this, SLOT(startDownload(QString)));

	QVariantMap relevantChanges;

	QMapIterator<QString, QVariant> ii(changes);
	while(ii.hasNext())
	{
		ii.next();
		if(ii.key() > QCoreApplication::applicationVersion())
			relevantChanges.insert(ii.key(), ii.value());
	}

	mNotificationDlg->setNewVersion(version);
	mNotificationDlg->setChanges(relevantChanges);
	mNotificationDlg->setDownloadURL(url, fullURL);

	mNotificationDlg->exec();
}

void UpdateManager::noUpdateFound()
{
	QMessageBox msg;
	msg.setWindowTitle(tr("No updates available."));
	msg.setText(tr("No updates available."));
	msg.setInformativeText(tr("Your installation of PonyEdit is up to date."));
	msg.setStandardButtons(QMessageBox::Ok);

	msg.exec();
}

void UpdateManager::startDownload(QString file)
{
	QProgressBar* progressBar = mNotificationDlg->getProgressBar();
	progressBar->show();

	QLabel* progressLabel = mNotificationDlg->getProgressLabel();
	progressLabel->show();

	QWidget* buttonWrapper = mNotificationDlg->getButtonWrapper();
	buttonWrapper->hide();

	connect(&mNetManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(downloadAuth(QNetworkReply*,QAuthenticator*)));

	QUrl download(file);

	mTempFile.setFileName(QDir::tempPath() + "/" + QFileInfo(download.path()).fileName());
	mTempFile.open(QIODevice::WriteOnly);

	QNetworkRequest request(download);

	mDownload = mNetManager.get(request);

	connect(mDownload, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
	connect(mDownload, SIGNAL(finished()), this, SLOT(downloadFinished()));
	connect(mDownload, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));

}

void UpdateManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	QProgressBar* progressBar = mNotificationDlg->getProgressBar();
	progressBar->setMaximum(bytesTotal);
	progressBar->setValue(bytesReceived);

	QLabel* progressLabel = mNotificationDlg->getProgressLabel();

	double rec = bytesReceived;
	double total = bytesTotal;

	QStringList units;
	units << "B" << "KiB" << "MiB";
	int unit;

	for(unit = 0; rec > 1024 && unit < 2; unit++)
		rec /= 1024;

	total /= 1024*1024;

	progressLabel->setText(QString("%1 %2 of %3 MiB").arg(rec, 0, 'f', 1).arg(units[unit]).arg(total, 0, 'f', 1));
}

void UpdateManager::downloadFinished()
{
	mTempFile.close();

	if (!gOpenFileManager.closeAllFiles())
		return;

	QString cmd;
	QStringList args;
#ifdef Q_OS_WIN
	QFileInfo info(mTempFile);
	cmd = info.filePath();
	args << "/verysilent" << "/suppressmsgboxes" << "/norestart";

	QProcess *installProc = new QProcess();
	installProc->startDetached(cmd, args);

	QApplication::exit();
#endif
}

void UpdateManager::downloadReadyRead()
{
	mTempFile.write(mDownload->readAll());
}

void UpdateManager::downloadAuth(QNetworkReply *reply, QAuthenticator *authenticator)
{
	authenticator->setUser("prealpha");
	authenticator->setPassword("letmein");
}
