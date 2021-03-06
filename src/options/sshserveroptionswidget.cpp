#include <QListWidgetItem>

#include "main/tools.h"
#include "ssh2/serverconfigwidget.h"
#include "ssh2/sshhost.h"

#include "sshserveroptionswidget.h"
#include "ui_sshserveroptionswidget.h"

SshServerOptionsWidget::SshServerOptionsWidget( QWidget *parent ) :
	OptionsDialogPage( parent ),
	ui( new Ui::SshServerOptionsWidget ) {
	ui->setupUi( this );

	mParent = ( OptionsDialog * ) parent;

	QListWidgetItem *first = populateServers();

	connect( ui->addNewButton, SIGNAL( clicked() ), this, SLOT( newServer() ) );
	connect( ui->deleteButton, SIGNAL( clicked() ), this, SLOT( deleteServer() ) );

	connect( mParent, SIGNAL( accepted() ), this, SLOT( accept() ) );
	connect( mParent, SIGNAL( rejected() ), this, SLOT( reject() ) );

	connect( ui->serversList,
	         SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
	         this,
	         SLOT( serverClicked( QListWidgetItem *, QListWidgetItem * ) ) );

	ui->serversList->setCurrentItem( first );
}

SshServerOptionsWidget::~SshServerOptionsWidget() {
	delete ui;
}

void SshServerOptionsWidget::accept() {
	for ( int ii = 0; ii < ui->serversList->count(); ii++ ) {
		QListWidgetItem *item = ui->serversList->item( ii );
		SshHost *host = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();
		if ( item->isHidden() ) {
			for ( int jj = 0; jj < mConfigWidgets.length(); jj++ ) {
				if ( mConfigWidgets[ jj ]->getEditHost() == host ) {
					disconnect( this, 0, mConfigWidgets[ jj ], 0 );
				}
			}
			delete host;
		}
	}

	emit accepted();

	Tools::saveServers();
}

void SshServerOptionsWidget::reject() {
	emit rejected();
}

QListWidgetItem *SshServerOptionsWidget::populateServers() {
	// Take a quick inventory of the servers in the list now...
	QMap< SshHost *, bool > currentList;
	for ( int i = 0; i < ui->serversList->count(); i++ ) {
		QListWidgetItem *item = ui->serversList->item( i );
		SshHost *host = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();
		currentList.insert( host, false );
	}

	// Go through the list of servers that should be there. Add new entries, mark existing ones as "ok to keep"
	QList< SshHost * > knownHosts = SshHost::getKnownHosts();
	foreach ( SshHost *host, knownHosts ) {
		if ( currentList.contains( host ) ) {
			currentList.insert( host, true );
		} else {
			QListWidgetItem *item = new QListWidgetItem();
			item->setText( host->getName() );
			item->setData( Qt::UserRole, QVariant::fromValue< void * >( host ) );
			ui->serversList->addItem( item );
		}
	}

	mConfigWidgets.clear();

	// Remove the list entries that have not been marked as "ok to keep"
	for ( int i = 0; i < ui->serversList->count(); i++ ) {
		QListWidgetItem *item = ui->serversList->item( i );
		SshHost *host = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();
		if ( ! currentList.value( host, true ) ) {
			i--;
			delete item;
		} else {
			mConfigWidgets.insert( i, new ServerConfigWidget() );
			mConfigWidgets[ i ]->setEditHost( host );

			connect( mConfigWidgets[ i ], SIGNAL( accepted() ), mParent, SLOT( accept() ) );
			connect( mConfigWidgets[ i ], SIGNAL( rejected() ), mParent, SLOT( reject() ) );
			connect( mConfigWidgets[ i ],
			         SIGNAL( nameUpdated( QString ) ),
			         this,
			         SLOT( serverNameUpdated( QString ) ) );

			connect( this, SIGNAL( accepted() ), mConfigWidgets[ i ], SLOT( acceptedHandler() ) );

			mConfigWidgets[ i ]->hide();
		}
	}

	return ui->serversList->item( 0 );
}

void SshServerOptionsWidget::serverClicked( QListWidgetItem *item, QListWidgetItem *previous ) {
	if ( previous ) {
		SshHost *prevHost = ( SshHost * ) previous->data( Qt::UserRole ).value< void * >();
		for ( int ii = 0; ii < mConfigWidgets.length(); ii++ ) {
			if ( mConfigWidgets[ ii ]->getEditHost() == prevHost ) {
				ui->configContainer->layout()->removeWidget( mConfigWidgets[ ii ] );
				mConfigWidgets[ ii ]->hide();
				break;
			}
		}
	}
	SshHost *host = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();
	for ( int ii = 0; ii < mConfigWidgets.length(); ii++ ) {
		if ( mConfigWidgets[ ii ]->getEditHost() == host ) {
			ui->configContainer->layout()->addWidget( mConfigWidgets[ ii ] );
			mConfigWidgets[ ii ]->show();
			break;
		}
	}
}

void SshServerOptionsWidget::newServer() {
	SshHost *host = SshHost::getBlankHost( true );

	QListWidgetItem *item = new QListWidgetItem();
	item->setText( host->getName() );
	item->setData( Qt::UserRole, QVariant::fromValue< void * >( host ) );
	ui->serversList->addItem( item );

	int row = ui->serversList->row( item );
	mConfigWidgets.insert( row, new ServerConfigWidget() );
	mConfigWidgets[ row ]->setEditHost( host );

	connect( mConfigWidgets[ row ], SIGNAL( accepted() ), mParent, SLOT( accept() ) );
	connect( mConfigWidgets[ row ], SIGNAL( rejected() ), mParent, SLOT( reject() ) );
	connect( mConfigWidgets[ row ], SIGNAL( nameUpdated( QString ) ), this, SLOT( serverNameUpdated( QString ) ) );

	connect( this, SIGNAL( accepted() ), mConfigWidgets[ row ], SLOT( acceptedHandler() ) );

	ui->serversList->setCurrentItem( item );
}

void SshServerOptionsWidget::deleteServer() {
	QListWidgetItem *item = ui->serversList->currentItem();

	SshHost *host = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();

	host->setSaveHost( false );
	item->setHidden( true );
}

void SshServerOptionsWidget::serverNameUpdated( const QString &newName ) {
	ServerConfigWidget *widget = static_cast< ServerConfigWidget * >( sender() );
	SshHost *host = widget->getEditHost();

	for ( int ii = 0; ii < ui->serversList->count(); ii++ ) {
		QListWidgetItem *item = ui->serversList->item( ii );
		SshHost *testHost = ( SshHost * ) item->data( Qt::UserRole ).value< void * >();
		if ( testHost == host ) {
			item->setText( newName );
			break;
		}
	}
}
