#ifndef _QVOSK_H_
#define _QVOSK_H_

#include <QThread>

class QVosk : public QThread
{
	Q_OBJECT

public:
	QVosk(QObject *parent);
	void run();

protected:

signals:
	void emitRecognize(int Final, QString Message);
};

#endif // _QVOSK_H_
