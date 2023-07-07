#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "QVosk.h"
#include "MainDialog.h"

QVosk::QVosk(QObject *parent) : QThread(parent)
{
}

void QVosk::run()
{
	int i;
	int n, Final;
	char buf[BUFFER_SIZE * RING_NUMBER];
	struct pool_t *e;
	struct list_head *pos, *q;
	char out[4096] = {0};
	char *p = 0;

AGAIN:
	pthread_mutex_lock(&mCaptured);
	pthread_cleanup_push(CleanupLock, (void *)&mCaptured);
	while (0 == pCaptured)
		pthread_cond_wait(&cCaptured, &mCaptured);
	n = 0;
	list_for_each_safe(pos, q, &pl.list) {
		e = list_entry(pos, struct pool_t, list);
		memcpy(buf + (n * BUFFER_SIZE), e->b, BUFFER_SIZE);
		list_del(pos); free(e);
		n++;
	}
	pCaptured = 0;
	pthread_cleanup_pop(1);

	if (0 == n)	goto AGAIN;
	Final = vosk_recognizer_accept_waveform(recognizer, buf, n * BUFFER_SIZE);
	if (Final)
		sprintf(out, "%s", vosk_recognizer_final_result(recognizer));
	else
		sprintf(out, "%s", vosk_recognizer_partial_result(recognizer));
	p = out;
	//for (i = 0; i < sizeof(out); i++) {
	//	if (out[i] == 123)	out[i] = 0, p = &out[i + 1];
	//	if (out[i] == 125)	out[i] = 0;
	//}
	emit emitRecognize(Final, QString(static_cast<const char *>(p)));

	goto AGAIN;
	vosk_recognizer_free(recognizer);
	vosk_model_free(model);
	return;
}
