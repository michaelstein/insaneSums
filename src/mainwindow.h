#pragma once
#include <QtCore/QMutex>
#include <QtWidgets/QDialog>
#include <QtWidgets/QMenu>
#include <QtGui/QAction>

#include "ui_mainwindow.h"
#include "cihash.h"

class QDragEnterEvent;
class QDropEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~MainWindow();

	void dragEnterEvent( QDragEnterEvent *e );
	void dropEvent( QDropEvent *e );

	bool checkPath();

private:
	Ui::MainWindowClass ui;
	CIHash *hash;
	QMutex hashButtonMutex;

private slots:
        void handleArguments( const QStringList & );

public slots:
	void on_fileButton_clicked();
	void on_md5Button_clicked();
	void on_sha1Button_clicked();
	void on_hashButton_triggered( QAction * );
	void on_cancelButton_clicked();
	void on_compEdit_textChanged();
	void reset();

	void processSHA256();
	void processSHA224();
	void processSHA384();
	void processSHA512();
	/*
	void processTiger();
	void processWhirlpool();
	void processRIPEMD160();
	void processRIPEMD320();
	void processRIPEMD128();
	void processRIPEMD256();
	void processCRC32();
	*/
	
	void showAbout();

	void processHash( CIHash * );
	void setHash( const QString & );
	void setHash( const QByteArray & ); // in binary form
	void updateProgress( const float );

	void activateButtons();
	void deactivateButtons();

	void setInfoColor( const QColor & );
	void resetInfoColor();
};
