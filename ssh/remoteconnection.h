#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QPixmap>
#include <QWidget>
#include <QWaitCondition>
#include <QMutex>
#include <QFlags>
#include <QVariant>
#include <QDebug>
#include "remotechannel.h"
#include "main/statuswidget.h"
#include "file/location.h"

class RemoteConnection;
class ConnectionStatusWidget;
class RemoteConnectionThread;
class SlaveChannel;
class FTPChannel;
class SshHost;
class ISshConnectionCallback;

typedef void(*DialogFunction)(ConnectionStatusWidget*, RemoteConnection*, QWidget*, QVariant);
typedef bool(*DialogCallback)(ConnectionStatusWidget*, RemoteConnection*, StatusWidget::Button, QVariant);

class RawChannelHandle {};

class RemoteConnection : public QObject
{
	friend class RemoteConnectionThread;
	Q_OBJECT

public:
	enum Status
	{
		Uninitialized   = 0x0000,
		Connecting      = 0x0001,
		Authenticating  = 0x0002,
		OpeningChannels = 0x0004,
		Connected       = 0x0008,
		Disconnecting   = 0x0010,
		Disconnected    = 0x0020,
		Error           = 0x0040,
		BaseStatusMask  = 0x0FFF,

		WaitingOnInput  = 0x1000
	};

public:
	RemoteConnection(SshHost* host);
	~RemoteConnection();

	virtual RemoteChannel* openChannel(RemoteChannel::Type /*type*/) { return NULL; }
	RemoteChannel* getChannel(RemoteChannel::Type type);
	SlaveChannel* getSlaveChannel();
	SlaveChannel* getSudoChannel();
	FTPChannel* getFtpChannel();

	void registerNewChannel(RemoteChannel* channel);	//	Only call from RemoteChannel constructor
	virtual RawChannelHandle* createRawSlaveChannel(bool /*sudo*/) { throw(tr("Invalid operation")); }
	virtual RawChannelHandle* createRawFTPChannel() { throw(tr("Invalid operation")); }
	virtual void sendLine(RawChannelHandle* /*handle*/, const QByteArray& /*data*/) { throw(tr("Invalid operation")); }
	virtual QByteArray readLine(RawChannelHandle* /*handle*/) { throw(tr("Invalid operation")); }
	virtual QList<Location> cthGetFTPListing(RawChannelHandle* /*handle*/, const Location& /*parent*/, bool /*includeHidden*/) { throw (tr("Invalid operation")); }
	virtual QByteArray cthReadFTPFile(RawChannelHandle* /*handle*/, const Location& /*location*/, ISshConnectionCallback* /*callback*/) { throw (tr("Invalid operation")); }
	virtual void cthWriteFTPFile(RawChannelHandle* /*handle*/, const Location& /*location*/, const QByteArray& /*content*/) { throw(tr("Invalid operation")); }

	inline void setHomeDirectory(const QString& homeDirectory) { mHomeDirectory = homeDirectory; }
	inline const QString& getHomeDirectory() { return mHomeDirectory; }

	inline Status getStatus() { return mStatus; }
	inline Status getBaseStatus() { return static_cast<Status>(mStatus & BaseStatusMask); }
	QString getStatusString();
	QPixmap getStatusIcon();

	inline bool isDeliberatelyDisconnecting() { return mDeliberatelyDisconnecting; }
	inline bool isConnected() { return mStatus & (Connected | OpeningChannels); }
	bool isDisconnecting();
	inline bool isWaitingOnInput() { return mStatus & WaitingOnInput; }

	void disconnect(bool deliberate);
	void setErrorStatus(const QString& errorMessage);
	void setDisconnected() { setStatus(Disconnected); }

	virtual QString getName() = 0;

	void waitForInput(DialogFunction dialogFunction, DialogCallback callbackFunction = DefaultDialogCallback, QVariant param = QVariant());
	inline void populateInputDialog(ConnectionStatusWidget* statusWidget, QWidget* target) { mInputDialog(statusWidget, this, target, mDialogParameter); }
	inline bool inputDialogCallback(ConnectionStatusWidget* statusWidget, StatusWidget::Button button) { return mDialogCallback(statusWidget, this, button, mDialogParameter); }
	static bool DefaultDialogCallback(ConnectionStatusWidget*, RemoteConnection*, StatusWidget::Button, QVariant) { return true; }
	inline void inputDialogCompleted() { mInputDialogWait.wakeAll(); }

	bool waitUntilOpen(bool waitForChannels = true);
	inline int getConnectionId() { return mConnectionId; }

	void recordChannelOpening();
	void recordChannelOpen();

	void channelStateChanged(RemoteChannel* channel);

	bool hasReasonToLive();		//	Returns true if something (eg; a file) is dependant on this connection. Used when deciding whether to reconnect or just die.

	void log(const QString& message);
	const QString& getLog() const;

signals:
	void statusChanged();
	void logUpdated(QString newLine);

protected:
	void startConnectionThread();		//	Always called once during the sub-class constructor
	virtual bool threadConnect() = 0;
	virtual RemoteChannel* threadOpenPrimaryChannel() = 0;
	void killThread();

	void setStatus(Status newStatus);
	void setBaseStatus(Status baseStatus) { setStatus(static_cast<Status>((mStatus & ~BaseStatusMask) | baseStatus)); }

protected:
	RemoteConnectionThread* mThread;
	QList<RemoteChannel*> mOpenChannels;
	QString mHomeDirectory;
	SshHost* mHost;

	bool mDeliberatelyDisconnecting;
	int mConnectionId;
	int mChannelsOpening;

	QWaitCondition mStatusWaiter;
	Status mStatus;
	QString mErrorMessage;

	QVariant mDialogParameter;
	DialogFunction mInputDialog;
	DialogCallback mDialogCallback;
	QWaitCondition mInputDialogWait;

	QString mLog;
};

#endif // REMOTECONNECTION_H
