#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <iostream>

#include "main.h"
#include "MainDialog.h"
#include "ui_MainDialog.h"

VoskRecognizer *recognizer = 0;
VoskModel *model = 0;
struct pool_t pl;
pthread_t tCapture = 0;
pthread_cond_t cCaptured;
int pCaptured = 0;
pthread_mutex_t mCaptured = PTHREAD_MUTEX_INITIALIZER;
snd_pcm_t *capture_handle = 0;

static void *CaptureThread(void *param) {
	int err = 0, i = 0, n = 0;
	char buf[BUFFER_SIZE] = {0};
	struct pool_t *e;
	struct list_head *pos, *q;

	static int I = 0;
	static char RingBuffer[BUFFER_SIZE * RING_NUMBER];
	
	unsigned int rate = SAMPLE_RATE;

	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	err = snd_pcm_open (&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0), assert(0 == err);
	err = snd_pcm_hw_params_malloc (&hw_params), assert(0 == err);
	err = snd_pcm_hw_params_any (capture_handle, hw_params), assert(0 <= err);
	err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED), assert(0 == err);
	err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format), assert(0 == err);
	err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0), assert(0 == err);
	err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1), assert(0 == err);
	err = snd_pcm_hw_params (capture_handle, hw_params), assert(0 == err);
	snd_pcm_hw_params_free (hw_params);
	err = snd_pcm_prepare (capture_handle), assert(0 == err);

	for (;;) {
		err = snd_pcm_readi (capture_handle, buf, BUFFER_FRAMES), assert(BUFFER_FRAMES == err);

		pthread_mutex_lock(&mCaptured);
		pthread_cleanup_push(CleanupLock, (void *)&mCaptured);
		n = 0;
		list_for_each(pos, &pl.list) {
			n++;
		}
		if (n < RING_NUMBER) {
			e = (struct pool_t *)malloc(sizeof(struct pool_t));
			e->b = RingBuffer + (I * BUFFER_SIZE), I++, I %= RING_NUMBER;
			memcpy(e->b, buf, BUFFER_SIZE), e->bsize = BUFFER_SIZE;
			list_add_tail(&e->list, &pl.list);
			pCaptured = 1;
			pthread_cond_signal(&cCaptured);
		} else {
		}
		pthread_cleanup_pop(1);
	}
	snd_pcm_close (capture_handle);
	return 0;
}

MainDialog::MainDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MainDialog)
{
	int rate = SAMPLE_RATE;
	char pathname[256] = {0};
	struct stat lbuf;
	QPlainTextEdit *textEdit;

	sprintf(pathname, "%s/%s", MODEL_PREFIX, "vosk-model-small-en-us-0.15");
	assert(0 == stat(pathname, &lbuf));
	model = vosk_model_new(pathname);
	recognizer = vosk_recognizer_new(model, (float)rate);

	ui->setupUi(this);
	textEdit = ui->textEdit;
	textEdit->setReadOnly(true);
	textEdit->setPlainText(QString(""));

	INIT_LIST_HEAD(&pl.list);
	pthread_cond_init(&cCaptured, NULL);

	_thread = new QVosk(this);
	connect(_thread, SIGNAL(emitRecognize(int, QString)), this, SLOT(onRecognize(int, QString)));
	_thread->start();
	sleep(1);
	pthread_create(&tCapture, NULL, CaptureThread, this);
}

MainDialog::~MainDialog()
{
	delete ui;
}


void MainDialog::onRecognize(int Final, QString Result)
{
	QPlainTextEdit *textEdit;

	textEdit = ui->textEdit;
	textEdit->setPlainText(Result);
	std::cout << Result.toStdString() << std::endl;
}
