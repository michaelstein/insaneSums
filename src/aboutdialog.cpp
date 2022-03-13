#include <QtGui/QTextDocument>

#include "aboutdialog.h"

AboutDialog::AboutDialog( QWidget *parent, Qt::WindowFlags flags )
	: QDialog( parent, flags )
{
	ui.setupUi( this );

	QString version = tr( "<h3>%1</h3><br/><br/>Version %2" );
	version = version.arg( tr( "insaneSums" ) ).arg( QLatin1String( INSANESUMS_VERSION ) );
	version.append( tr( "<br/>insaneSums is a program for calculating checksum/hash values for files.<br/>" ) );

	QString qtversion = tr( "<a href=\"http://www.qt.io/\">Qt</a> %1" );
	qtversion = qtversion.arg( QLatin1String( QT_VERSION_STR ) );

	QString cryptoppversion = tr( "<a href=\"http://cryptopp.com/\">Crypto++</a> 8.5.0" );

	//: The authors of this program.
	QString author = tr( "Michael Stein and Jonathan Wienß" );

	ui.textLabel->setText( tr("%1<br/>"
				"<br/>Developed by and copyright %2.<br/>"
				"Licensed under the <a href=\"https://mit-license.org/\">MIT License</a>.<br/>"
				"<br/>Uses %3 and %4."
				).arg( version ).arg( author ).arg( qtversion ).arg( cryptoppversion ) );
	ui.textLabel->setWordWrap( true );
	ui.textLabel->setOpenExternalLinks( true );

	setFixedSize( sizeHint() );
}

AboutDialog::~AboutDialog()
{
}
