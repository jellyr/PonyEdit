#include "mainwindow.h"

#include <QDebug>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTime>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ServerConfigDlg dlg(this);
	dlg.exec();
	QString hostname = dlg.getHostname();
	QString login = dlg.getLogin();
	QString password = dlg.getPassword();
	QString filename = dlg.getFilename();

	SshConnection* c = new SshConnection();
	c->connect(hostname.toUtf8(), 22);
	c->authenticatePassword(login.toUtf8(), password.toUtf8());

	mController = new SshRemoteController();
	mController->attach(c);

	QByteArray fileContent = mController->openFile(filename.toUtf8());

	mController->splitThread();

	mEditor = new QTextEdit(this);
	mEditor->setAcceptRichText(false);
	mEditor->setFont(QFont("courier new", 12));
	setCentralWidget(mEditor);

	mCurrentDocument = new QTextDocument(QString(fileContent));
	mEditor->setDocument(mCurrentDocument);
	mCurrentDocument->setDefaultFont(QFont("courier new", 12));

	connect(mCurrentDocument, SIGNAL(contentsChange(int,int,int)), this, SLOT(docChanged(int,int,int)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::docChanged(int position, int charsRemoved, int charsAdded)
{
	QString plainText = mCurrentDocument->toPlainText();

	Push p;
	p.position = position;
	p.remove = charsRemoved;
	p.add = plainText.mid(position, charsAdded);
	mController->push(p);
}
