#include "batchwindow.h"

class BatchWindow::Private {
public:
	Private()
	{}
};

BatchWindow::BatchWindow( QObject *parent )
	: QObject( parent ), d( new BatchWindow::Private() )
{}

BatchWindow::~BatchWindow()
{}
