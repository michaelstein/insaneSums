#pragma once
#include "QtCore/QObject"

class BatchWindow : public QObject
{
	Q_OBJECT

public:
	BatchWindow( QObject *parent = nullptr );
	~BatchWindow();

private:
	class Private;
	Private *d;
};
