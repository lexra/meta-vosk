#ifndef _MAINDIALOG_H_
#define _MAINDIALOG_H_

#include <QVariant>
#include <QMap>
#include <QDialog>
#include <QPlainTextEdit>
#include <QString>
#include "QVosk.h"
#include <vosk_api.h>
#include "list.h"

namespace Ui {
	class MainDialog;
}

class MainDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MainDialog(QWidget *parent = 0);
	MainDialog(const MainDialog&) = delete;
	MainDialog& operator=(const MainDialog&) = delete;
	~MainDialog();

public slots:
	void onRecognize(int Final, QString Result);

protected:
	QVosk *_thread;

private:
	Ui::MainDialog *ui;
};

MainDialog *getMainDialog(void);

#endif // _MAINDIALOG_H_
