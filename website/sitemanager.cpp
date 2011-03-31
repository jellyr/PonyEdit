#include <QCoreApplication>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QVariantMap>
#include <QDate>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QDesktopServices>

#include "sitemanager.h"
#include "main/json.h"

SiteManager::SiteManager()
{
	mManager = new QNetworkAccessManager(this);

	#ifdef Q_OS_WIN
		mOS = "WIN";
	#endif
	#ifdef Q_OS_MAC
		mOS = "OSX";
	#endif
	#ifdef Q_OS_LINUX
		mOS = "LIN";
	#endif

	connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReply(QNetworkReply*)));
}

SiteManager::~SiteManager()
{
	//	Clean out any leftover replies
	foreach (QNetworkReply* reply, mReplies)
		delete reply;

	if(mManager)
		delete mManager;

	mManager = NULL;
}

void SiteManager::checkForUpdates(bool forceNotification)
{
	QString version = QString("vmaj=%1&vmin=%2&rev=%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(REVISION);
	QUrl url(QString(SITE_URL) + "version/?" + version + "&u=testuser&key=5555555");
	QNetworkReply* reply = mManager->get(QNetworkRequest(url));

	mReplies.append(reply);
	if(forceNotification)
		mReplyTypes.append(UpdateCheckForcedNotification);
	else
		mReplyTypes.append(UpdateCheck);
}

void SiteManager::checkLicence()
{
	QString version = QString("vmaj=%1&vmin=%2&rev=%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(REVISION);
	QUrl url(QString(SITE_URL) + "licence/?" + version + "&u=testuser&key=5555555");
	QNetworkReply* reply = mManager->get(QNetworkRequest(url));

	mReplies.append(reply);
	mReplyTypes.append(LicenceCheck);
}

void SiteManager::getTrial()
{
	QUrl url(QString(SITE_URL) + "licence/?f=trial");
	QNetworkReply* reply = mManager->get(QNetworkRequest(url));

	mReplies.append(reply);
	mReplyTypes.append(GetTrial);
}

void SiteManager::handleReply(QNetworkReply *reply)
{
	int index = mReplies.indexOf(reply);

	if(index >= 0 && reply->error() == QNetworkReply::NoError)
	{
		QByteArray result = reply->readAll();

		bool ok;
		QVariantMap data = Json::parse(result, ok).toMap();

		if(!ok)
		{
			qDebug() << "Parsing error of website JSON";
			return;
		}

		QVariantMap version;
		QVariantMap changes;

		switch(mReplyTypes[index])
		{
			case UpdateCheck:
			case UpdateCheckForcedNotification:
				version = data[mOS].toMap();
				changes = data["changes"].toMap();
				if(version["major"].toInt() > MAJOR_VERSION ||
						(version["major"].toInt() == MAJOR_VERSION && version["minor"].toInt() > MINOR_VERSION) ||
						(version["major"].toInt() == MAJOR_VERSION && version["minor"].toInt() == MINOR_VERSION && version["revision"].toInt() > REVISION))
					emit updateAvailable(version, changes);
				else if(mReplyTypes[index] == UpdateCheckForcedNotification)
					emit noUpdateAvailable();
				break;

			case LicenceCheck:
				emit licenceStatus(data["valid"].toBool());
				break;

			case GetTrial:
				emit gotTrial(data["key"].toString());
				break;
		}
	}

	if(index >= 0 )
	{
		mReplies.removeAt(index);
		mReplyTypes.removeAt(index);
	}

	reply->deleteLater();
}

void SiteManager::feedbackHappy()
{
	QUrl url(QString(SITE_URL) + "feedback/?feedback=happy&version=" + QCoreApplication::applicationVersion());

	QDesktopServices::openUrl(url);
}

void SiteManager::feedbackSad()
{
	QUrl url(QString(SITE_URL) + "feedback/?feedback=sad&version=" + QCoreApplication::applicationVersion());

	QDesktopServices::openUrl(url);
}

void SiteManager::purchase()
{
	QUrl url(QString(SITE_URL) + "buy/");

	QDesktopServices::openUrl(url);
}
