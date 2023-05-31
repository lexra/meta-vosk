#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sched.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <vosk_api.h>
#include "list.h"

#define MODEL_PREFIX	"/usr/share/vosk"

#define SAMPLE_RATE     24000
#define BUFFER_FRAMES	(SAMPLE_RATE / 5)
#define BUFFER_SIZE	(BUFFER_FRAMES * 16 / 8)
#define RING_NUMBER	32

static pthread_t tCapture = 0;
static pthread_cond_t cCaptured;
static int pCaptured = 0;
static pthread_mutex_t mCaptured = PTHREAD_MUTEX_INITIALIZER;

static snd_pcm_t *capture_handle = 0;

struct pool_t {
    struct list_head list;
    char c[BUFFER_SIZE];
};
static struct pool_t pl;

static void *CaptureThread(void *param) {
	int err = 0, i = 0, n = 0;
	char buf[BUFFER_SIZE] = {0};
	struct pool_t *e;
	struct list_head *pos, *q;

	for (;;) {
		err = snd_pcm_readi (capture_handle, buf, BUFFER_FRAMES), assert(BUFFER_FRAMES == err);
		pthread_mutex_lock(&mCaptured);
		pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
		n = 0;
		list_for_each(pos, &pl.list) {
			n++;
		}
		if (n < RING_NUMBER) {
			e = (struct pool_t *)malloc(sizeof(struct pool_t));
			memcpy(e->c, buf, BUFFER_SIZE);
			list_add_tail(&e->list, &pl.list);
		} else {
			list_for_each_safe(pos, q, &pl.list) {
				e = list_entry(pos, struct pool_t, list);
				list_del(pos);
				free(e);
			}
		}
		pCaptured = 1;
		pthread_cond_signal(&cCaptured);
		pthread_cleanup_pop(1);
	}
	snd_pcm_close (capture_handle);
	return 0;
}

int main (int argc, char *argv[]) {
	int i = 0, err, rate = SAMPLE_RATE;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
	VoskModel *model;
	char pathname[256] = {0};
	VoskRecognizer *recognizer;
	int final;
	char buf[BUFFER_SIZE * RING_NUMBER];
	struct stat lbuf;

	int n = 0;
	struct pool_t *e;
	struct list_head *pos, *q;

	sprintf(pathname, "%s/%s", MODEL_PREFIX, "vosk-model-small-en-us-0.15");
	if (argc > 1)
		sprintf(pathname, "%s/%s", MODEL_PREFIX, argv[1]);
	if (0 != stat(pathname, &lbuf))
		printf("Model not found!! \n"), exit(1);

	model = vosk_model_new(pathname);
	recognizer = vosk_recognizer_new(model, (float)SAMPLE_RATE);
	memset(buf, 0xff, sizeof(buf));
	//vosk_recognizer_accept_waveform(recognizer, buf, rate * 16 / 8);

	system("amixer cset name='Left Input Mixer L2 Switch' on");
	system("amixer cset name='Right Input Mixer R2 Switch' on");
	system("amixer cset name='Headphone Playback Volume' 100%");
	system("amixer cset name='PCM Volume' 100%");
	system("amixer cset name='Input PGA Volume' 100%");

	err = snd_pcm_open (&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0), assert(0 == err);
	fprintf(stdout, "audio interface opened\n");
	err = snd_pcm_hw_params_malloc (&hw_params), assert(0 == err);
	fprintf(stdout, "hw_params allocated\n");
	err = snd_pcm_hw_params_any (capture_handle, hw_params), assert(0 <= err);
	fprintf(stdout, "hw_params initialized\n");
	err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED), assert(0 == err);
	fprintf(stdout, "hw_params access setted\n");
	err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format), assert(0 == err);
	fprintf(stdout, "hw_params format setted\n");
	err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0), assert(0 == err);
	fprintf(stdout, "hw_params rate setted\n");
	err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1), assert(0 == err);
	fprintf(stdout, "hw_params channels setted\n");
	err = snd_pcm_hw_params (capture_handle, hw_params), assert(0 == err);
	fprintf(stdout, "hw_params setted\n");
	snd_pcm_hw_params_free (hw_params);
	fprintf(stdout, "hw_params freed\n");
	err = snd_pcm_prepare (capture_handle), assert(0 == err);
	fprintf(stdout, "audio interface prepared\n");

	INIT_LIST_HEAD(&pl.list);
	pthread_cond_init(&cCaptured, NULL);
	usleep(1);
	pthread_create(&tCapture, NULL, CaptureThread, 0);

AGAIN:
	pthread_mutex_lock(&mCaptured);
	pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
	while (0 == pCaptured)
		pthread_cond_wait(&cCaptured, &mCaptured);
	n = 0;
	list_for_each_safe(pos, q, &pl.list) {
		e = list_entry(pos, struct pool_t, list);
		memcpy(buf + n * BUFFER_SIZE, &e->c, BUFFER_SIZE);
		list_del(pos); free(e);
		n++;
	}
	pCaptured = 0;
	pthread_cleanup_pop(1);

	if (0 == n)
		goto AGAIN;
	final = vosk_recognizer_accept_waveform(recognizer, buf, n * BUFFER_SIZE);
	if (final)	printf("%s\n", vosk_recognizer_final_result(recognizer));
	else		printf("%s\n", vosk_recognizer_partial_result(recognizer));
	goto AGAIN;

	vosk_recognizer_free(recognizer);
	vosk_model_free(model);
	return 0;
}

