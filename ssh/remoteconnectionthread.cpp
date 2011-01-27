#include "remoteconnectionthread.h"
#include "remoteconnection.h"
#include <QDebug>

RemoteConnectionThread::RemoteConnectionThread(RemoteConnection* connection) :
	QThread(0)
{
	mConnection = connection;
}

void RemoteConnectionThread::connect()
{
	qDebug() << "Attempting to connect...";

	mConnection->setStatus(RemoteConnection::Connecting);
	mConnection->mConnectionId++;

	if (mConnection->threadConnect())
	{
		mConnection->setStatus(RemoteConnection::OpeningChannels);

		if (mConnection->mOpenChannels.length() == 0)
		{
			//	Always open Primary channel on first successful connection
			mConnection->threadOpenPrimaryChannel();
		}

		//	Wait for all of my channels to reconnect (or fail to) before changing state to "Connected"
		foreach (RemoteChannel* channel, mConnection->mOpenChannels)
			channel->waitUntilOpen(mConnection->mConnectionId);

		mConnection->setStatus(RemoteConnection::Connected);
	}
	else
		qDebug() << "Connection failed";
}

void RemoteConnectionThread::run()
{
	while (!mConnection->isDeliberatelyDisconnecting())
	{
		QMutex mutex;
		mutex.lock();

		if (!mConnection->isConnected())
			connect();

		mSleeper.wait(&mutex);

		qDebug() << "ZZzzzzz--** RemoteConnectionThread woken from its sleep. Status = " << mConnection->mStatus;
	}
}

void RemoteConnectionThread::wake()
{
	mSleeper.wakeAll();
}















