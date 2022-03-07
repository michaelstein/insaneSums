#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtCore/QPointer>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QMimeData>

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QColor>

#include <QtWidgets/QFileDialog>

#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

#include "mainwindow.h"
#include "cihash.h"
#include "aboutdialog.h"

MainWindow::MainWindow( QWidget *parent, Qt::WindowFlags flags )
        : QMainWindow( parent, flags )
		, hash( NULL )
{
	ui.setupUi( this );

	// Set application icon.
	setWindowIcon( QIcon( tr( ":/insaneSums/Resources/logo.png" ) ) );

	// Allow Drag and Drop.
	setAcceptDrops( true );

	// Reset hashes when input is changed.
	connect( ui.fileEdit, SIGNAL( textChanged( const QString & ) ),
			 this, SLOT( reset() ) );

	// Connect About dialog.
	connect( ui.actionAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );

	ui.menuHash;

	// Adding hash actions to the hashButton and Hash menu item.
	QAction *hashAction = NULL;
	// SHA224
	hashAction = new QAction( tr( "SHA224" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA224() ) );
	ui.hashButton->addAction( hashAction );
	hashAction = new QAction( tr( "Calculate SHA224" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA224() ) );
	ui.menuHash->addAction( hashAction );
	// SHA256
	hashAction = new QAction( tr( "SHA256" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA256() ) );
	ui.hashButton->setDefaultAction( hashAction );
	hashAction = new QAction( tr( "Calculate SHA256" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA256() ) );
	ui.menuHash->addAction( hashAction );
	// SHA284
	hashAction = new QAction( tr( "SHA384" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA384() ) );
	ui.hashButton->addAction( hashAction );
	hashAction = new QAction( tr( "Calculate SHA384" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA384() ) );
	ui.menuHash->addAction( hashAction );
	// SHA512
	hashAction = new QAction( tr( "SHA512" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA512() ) );
	ui.hashButton->addAction( hashAction );
	hashAction = new QAction( tr( "Calculate SHA512" ), this );
	connect( hashAction, SIGNAL( triggered() ), this, SLOT( processSHA512() ) );
	ui.menuHash->addAction( hashAction );

	// Handle application parameters.
	QStringList args = QCoreApplication::arguments();
	handleArguments( args );

	ui.statusBar->showMessage(
		"Choose a file, then click on the desired hash algorithm." );
}

MainWindow::~MainWindow()
{
	if( hash )
		delete hash;
}

void MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
	if( e->mimeData()->hasFormat( "text/plain" ) ||
		e->mimeData()->hasFormat( "text/uri-list" ) )
		e->acceptProposedAction();
}

void MainWindow::dropEvent( QDropEvent *e )
{
	if( e->mimeData()->hasUrls() ) {
		QList<QUrl> urlList;
		QString fName;
		QFileInfo info;

		urlList = e->mimeData()->urls(); // returns list of QUrls

		// If just text was dropped, urlList is empty (size == 0)
		if ( urlList.size() > 0) { // if at least one QUrl is present in list
			fName = urlList[0].toLocalFile(); // convert first QUrl to local path
			info.setFile( fName ); // information about file
			if ( info.isFile() ) // if is file, setText
				ui.fileEdit->setText( QDir::toNativeSeparators( fName ) );

		}
	} else if( e->mimeData()->hasFormat( "text/plain" ) ) {
		// Text can be dropped directly.
		QString input = e->mimeData()->text();
		QFileInfo info( input.trimmed() );
		if ( info.isFile() ) { // if is file, setText
			ui.fileEdit->setText( QDir::toNativeSeparators( info.absoluteFilePath() ) );
		} else {
			QRegularExpression regex( "[0-9a-fA-F]+" );
			const auto match = regex.match(input);
			if (match.hasMatch()) {
				const auto capture = match.captured(0);
				ui.compEdit->setText(capture);
			}
		}
	}

	e->acceptProposedAction();
}

bool MainWindow::checkPath()
{
	QFileInfo finfo( ui.fileEdit->text().trimmed() );
	if( finfo.isFile() ) {
		ui.fileEdit->setText( QDir::toNativeSeparators( finfo.absoluteFilePath() ) );
		return true;
	}
	return false;
}

void MainWindow::reset()
{
	ui.hashEdit->clear();
	ui.progressBar->setValue( ui.progressBar->minimum() );
	on_compEdit_textChanged();
}

void MainWindow::on_fileButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open File") );
	if( !fileName.isEmpty() )
		ui.fileEdit->setText( QDir::toNativeSeparators( fileName ) );
}

void MainWindow::processHash( CIHash *h )
{
	// Open file and initialize stream.
	if( !checkPath() )
		return;
	QFile *file = new QFile( ui.fileEdit->text() );

	// Deactivate buttons.
	deactivateButtons();

	// Get hash ready.
	if( hash )
		delete hash;
	hash = h;
	hash->setInput( file );
	connect( hash, SIGNAL( progressChanged( float ) ),
			this, SLOT( updateProgress( const float ) ) );
	connect( hash, SIGNAL( digest( QByteArray ) ),
			this, SLOT( setHash( const QByteArray & ) ) );
	connect( hash, SIGNAL( finished() ),
			this, SLOT( activateButtons() ) );

	// Start hashing.
	hash->start();
}

void MainWindow::on_md5Button_clicked()
{
	processHash( CIHash::createMD5() );
}

void MainWindow::on_sha1Button_clicked()
{
	processHash( CIHash::createSHA1() );
}

void MainWindow::on_hashButton_triggered( QAction * a )
{
	hashButtonMutex.lock();
	bool enabled = ui.hashButton->isEnabled();
	ui.hashButton->setDefaultAction( a );
	ui.hashButton->setEnabled( enabled );
	hashButtonMutex.unlock();
}

void MainWindow::processSHA256()
{
	processHash( CIHash::createSHA256() );
}

void MainWindow::processSHA224()
{
	processHash( CIHash::createSHA224() );
}

void MainWindow::processSHA384()
{
	processHash( CIHash::createSHA384() );
}

void MainWindow::processSHA512()
{
	processHash( CIHash::createSHA512() );
}

/*
void MainWindow::processTiger()
{
	processHash( CIHash::createTiger() );
}

void MainWindow::processWhirlpool()
{
	processHash( CIHash::createWhirlpool() );
}

void MainWindow::processRIPEMD160()
{
	processHash( CIHash::createRIPEMD160() );
}

void MainWindow::processRIPEMD320()
{
	processHash( CIHash::createRIPEMD320() );
}

void MainWindow::processRIPEMD128()
{
	processHash( CIHash::createRIPEMD128() );
}

void MainWindow::processRIPEMD256()
{
	processHash( CIHash::createRIPEMD256() );
}

void MainWindow::processCRC32()
{
	processHash( CIHash::createCRC32() );
}*/

void MainWindow::showAbout()
{
	QPointer<AboutDialog> d = new AboutDialog( this );

	d->setWindowTitle( tr( "About insaneSums" ) );
	d->setModal( true );
	d->exec();

	delete d;
}

void MainWindow::on_cancelButton_clicked()
{
	hash->stopProcess();
}

void MainWindow::setHash( const QString & str )
{
	ui.hashEdit->setText( str );
	on_compEdit_textChanged();
}

/**
 * Takes binary byte array and converts to hex string.
 * Then sets string as output.
 */
void MainWindow::setHash( const QByteArray & bytes )
{
	setHash( QString( bytes.toHex() ) );
}

void MainWindow::updateProgress( const float c )
{
	ui.progressBar->setValue( c * ui.progressBar->maximum() );
}

void MainWindow::activateButtons()
{
	setAcceptDrops( true );
	ui.fileEdit->setEnabled( true );
	ui.fileButton->setEnabled( true );
	ui.md5Button->setEnabled( true );
	ui.sha1Button->setEnabled( true );

	hashButtonMutex.lock();
	ui.hashButton->setEnabled( true );
	hashButtonMutex.unlock();

	ui.cancelButton->setEnabled( false );
}

void MainWindow::deactivateButtons()
{
	setAcceptDrops( false );
	ui.fileEdit->setEnabled( false );
	ui.fileButton->setEnabled( false );
	ui.md5Button->setEnabled( false );
	ui.sha1Button->setEnabled( false );

	hashButtonMutex.lock();
	ui.hashButton->setEnabled( false );
	hashButtonMutex.unlock();

	ui.cancelButton->setEnabled( true );
}

void MainWindow::on_compEdit_textChanged(){
	QString inHash = ui.compEdit->text();
	// Remove Symbols that don't fit the Hash Scheme
	QRegularExpression regex("[^(0123456789AaBbCcDdEeFf)]"); // This RegExp describes all non-Hash Symbols
	inHash.remove(regex);
	int cursorPos = ui.compEdit->cursorPosition();
        ui.compEdit->setText( inHash ); // For User Feedback: See your input String strip of disturbing Symbols
        ui.compEdit->setCursorPosition( cursorPos );
        if ( !inHash.isEmpty() ) {
			// Case: User did put nothing in Hash
			if ( ui.hashEdit->text().isEmpty() ) {
				// Case: Hash not calculated yet
				if ( ui.fileEdit->text().isEmpty() ) {
					// Case: No File specified yet
                                        //ui.compLabel->setText(tr("Please select a file."));
                                        ui.statusBar->showMessage( tr("Please select a file.") );
				} else {
					// Case: File already specified
                                        ui.statusBar->showMessage(tr("Please select a hashing algorithm."));
				}
				setInfoColor( QColor( 255, 255, 145 ) );
				return; // Break
			}
			// Ensured: User did put in Hash, File specified
			QString calHash = ui.hashEdit->text();
			if ( inHash.compare(calHash, (Qt::CaseSensitivity) 0) == 0 ) {
                                ui.statusBar->showMessage(tr("The hash is correct."));
				setInfoColor( QColor( 200, 255, 200 ) );
			} else if( calHash.length() > inHash.length() && inHash.compare( calHash.mid(0, inHash.length() ), (Qt::CaseSensitivity) 0 ) == 0 ) {
				// Case: inputted Hash is shorter BUT besides that, equal to calculated Hash (except for the longer rest)
                                ui.statusBar->showMessage(tr("Input hash too short."));
				setInfoColor( QColor( 255, 235, 125 ) );
			} else if( calHash.length() > inHash.length() && inHash.compare( calHash.mid(calHash.length() - inHash.length(), calHash.length() ), (Qt::CaseSensitivity) 0 ) == 0 ) {
				// Case: inputted Hash is shorter BUT besides that, equal to calculated Hash (except for the longer beginning
                                ui.statusBar->showMessage(tr("Input hash too short."));
				setInfoColor( QColor( 255, 235, 125 ) );
			} else if( calHash.length() < inHash.length() && calHash.compare( inHash.mid(0, calHash.length() ), (Qt::CaseSensitivity) 0 ) == 0 ) {
				// Case: inputted Hash is longer BUT besides that, equal to calculated Hash (except for the longer rest)
                                ui.statusBar->showMessage(tr("Input hash too long."));
				setInfoColor( QColor( 255, 235, 125 ) );
			} else if( calHash.length() < inHash.length() && calHash.compare( inHash.mid( inHash.length() - calHash.length() , inHash.length() ), (Qt::CaseSensitivity) 0 ) == 0 ) {
				// Case: inputted Hash is longer BUT besides that, equal to calculated Hash (except for the longer beginning)
                                ui.statusBar->showMessage(tr("Input hash too long."));
				setInfoColor( QColor( 255, 235, 125 ) );
			} else {
                                ui.statusBar->showMessage(tr("The hash is incorrect."));
				setInfoColor( QColor( 255, 200, 200 ) );
				// todo: where is the error?
			}
	} else {
		// Case: User did NOT put in Hash
		if ( ui.hashEdit->text().isEmpty() ) {
			// Case: Hash not calculated yet
			if ( ui.fileEdit->text().isEmpty() ) {
				// Case: No File specified yet
                                ui.statusBar->showMessage(tr("Please select a file."));
			} else {
				// Case: File already specified
                                ui.statusBar->showMessage(tr("Please select a hashing algorithm."));
			}
			return; // Break
		}
		ui.statusBar->showMessage(tr("Please enter the expected hash."));
		setInfoColor( QColor( 255, 255, 145 ) );
	}
}

void MainWindow::setInfoColor( const QColor & col )
{
	QPalette pal( ui.compEdit->palette() );
	pal.setColor( ui.compEdit->backgroundRole(), col );
	ui.compEdit->setPalette( pal );
}

void MainWindow::resetInfoColor()
{
	setInfoColor( Qt::lightGray );
}

void MainWindow::handleArguments( const QStringList & args ) {
    uint count = args.length();
    uint done = 0;
    QString algo;
    QString path;
    QString temp;

    // Retrieve application file name.
    QString appFilePath( QCoreApplication::applicationFilePath() );
    QString appFileName( appFilePath.split( QDir::separator() ).last() );

    // Iterate through arguments.
    for( uint i = 1; i < count; i++ ) {
        // Argument "-a":
        if( args.at(i) == "-a" ) {
            i++; // We are interested in what follows.
            done++;
            if( i < count ) {
                algo = (QString)args.at( i );
                done++;
            }
        } else {
            temp = (QString)args.at( i );
            QFileInfo info( temp );
            if( info.isFile() ) { // In case this is indeed a file...
                path = temp;
            }
            i++;
        }
    }

    // In case we have a file path given...
    if( path.length() > 0 ) {
        // ... set it in the file edit.
        ui.fileEdit->setText( path );

        // ... and calculate a hash if available.
        algo = algo.toLower();
        if( algo == "md5" ) {
            processHash( CIHash::createMD5() );
        } else if( algo == "sha1" ) {
            processHash( CIHash::createSHA1() );
        } else if( algo == "sha224" ) {
            processSHA224();
        } else if( algo == "sha256" ) {
            processSHA256();
        } else if( algo == "sha384" ) {
            processSHA384();
        } else if( algo == "sha512" ) {
            processSHA512();
        }
    }
}



































