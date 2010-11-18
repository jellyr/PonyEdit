#ifndef LOCATION_H
#define LOCATION_H

#include <QString>
#include <QVariant>
#include <QDateTime>

class SshHost;
class BaseFile;
class LocationShared;
class SshRemoteController;

class Location
{
	friend class LocationShared;
	friend class SshRequest_ls;
	friend class SshRequest_open;

public:
	enum Type { Unknown = 0, File = 1, Directory = 2 };
	enum Protocol { Local = 0, Ssh = 1 };

public:
	Location();
	Location(const Location& other);
	Location& operator=(const Location& other);
	Location(const QString& path);
	~Location();

	QString getDisplayPath() const;
	const QString& getPath() const;
	const QString& getLabel() const;
	QIcon getIcon() const;
	Type getType() const;
	int getSize() const;
	const QDateTime& getLastModified() const;
	Protocol getProtocol() const;

	const Location& getParent();
	QString getParentPath() const;
	QString getRemotePath() const;
	SshHost* getRemoteHost() const;

	bool isNull() const;
	bool isHidden() const;
	bool isDirectory() const;

	BaseFile* getFile();

	void asyncGetChildren();

private:
	Location(const Location& parent, const QString& path, Type type, int size, QDateTime lastModified);
	Location(LocationShared* data);

	void sshChildLoadResponse(const QList<Location>& children);
	void childLoadError(const QString& error);

	void sshFileOpenResponse(SshRemoteController* controller, quint32 bufferId, const QByteArray& data);
	void fileOpenError(const QString& error);

	LocationShared* mData;
};

class LocationShared
{
	friend class Location;

public:
	static void cleanupIconProvider();

private:
	LocationShared();
	static void initIconProvider();

	void setPath(const QString& path);
	bool ensureConnected();

	void emitListLoadedSignal();
	void emitListLoadError(const QString& error);
	void localLoadSelf();
	void localLoadListing();
	void sshLoadListing();

	int mReferences;
	QString mPath;
	QString mLabel;

	Location::Type mType;
	Location::Protocol mProtocol;
	QDateTime mLastModified;
	QList<Location> mChildren;
	Location mParent;
	bool mSelfLoaded;
	bool mListLoaded;
	bool mLoading;
	int mSize;

	QString mRemoteHostName;
	QString mRemoteUserName;
	QString mRemotePath;
	SshHost* mRemoteHost;
};

Q_DECLARE_METATYPE (Location);

#endif // LOCATION_H
