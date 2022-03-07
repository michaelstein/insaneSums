#pragma once
#include <QtWidgets/QDialog>
#include "ui_aboutdialog.h"

#define INSANESUMS_VERSION "1.3"

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog( QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	~AboutDialog();

private:
	Ui::AboutDialogClass ui;
};
