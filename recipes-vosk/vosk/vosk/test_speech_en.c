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
#include <vosk_api.h>

#define SAMPLE_RATE     24000

static pthread_t tCapture = 0;
static pthread_cond_t cCaptured;
static int pCaptured = 0;
static pthread_mutex_t mCaptured = PTHREAD_MUTEX_INITIALIZER;

#define BUFFER_FRAMES	(SAMPLE_RATE / 10)
#define BUFFER_SIZE	(BUFFER_FRAMES * 16 / 8)
#define RING_NUMBER	5

static snd_pcm_t *capture_handle = 0;
static int idx = RING_NUMBER - 1;
static char buffer[BUFFER_SIZE * RING_NUMBER] = {0};

static void *CaptureThread(void *param) {
	int err = 0, i;

	pthread_mutex_lock(&mCaptured);
	pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
	idx = RING_NUMBER - 1;
	pthread_cleanup_pop(1);

	for (;;) {
		pthread_mutex_lock(&mCaptured);
		pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
		i = idx;
		pthread_cleanup_pop(1);

		i++, i %= RING_NUMBER;
		err = snd_pcm_readi (capture_handle, buffer + i * BUFFER_SIZE, BUFFER_FRAMES), assert(BUFFER_FRAMES == err);

		pthread_mutex_lock(&mCaptured);
		pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
		idx++, idx %= RING_NUMBER;
		pCaptured = 1;
		pthread_cond_signal(&cCaptured);
		pthread_cleanup_pop(1);
	}
	snd_pcm_close (capture_handle);
	return 0;
}

int main (int argc, char *argv[]) {
	int i, err, rate = SAMPLE_RATE;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
	VoskModel *model;
	VoskRecognizer *recognizer;
	int final;
	char buf[BUFFER_SIZE] = {0};
        long long o, n;
        struct timespec spec;

	model = vosk_model_new("/usr/share/vosk/vosk-model-small-en-us-0.15");
	recognizer = vosk_recognizer_new(model, (float)SAMPLE_RATE);

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

	pthread_create(&tCapture, NULL, CaptureThread, 0);
WAIT:
	pthread_mutex_lock(&mCaptured);
	pthread_cleanup_push(pthread_mutex_unlock, (void *)&mCaptured);
	while (0 == pCaptured)
		pthread_cond_wait(&cCaptured, &mCaptured);
	i = idx;
	memcpy(buf, buffer + (i * BUFFER_SIZE), BUFFER_SIZE);
	pCaptured = 0;
	pthread_cleanup_pop(1);

	clock_gettime(CLOCK_REALTIME, &spec), o  = spec.tv_sec * 1000LL + spec.tv_nsec / 1000000;
	final = vosk_recognizer_accept_waveform(recognizer, buf, BUFFER_SIZE);
	clock_gettime(CLOCK_REALTIME, &spec), n  = spec.tv_sec * 1000LL + spec.tv_nsec / 1000000;
	if (final)
		printf("%s\n", vosk_recognizer_final_result(recognizer));
	else
		printf("%%s\n", vosk_recognizer_partial_result(recognizer));

	goto WAIT;

	vosk_recognizer_free(recognizer);
	vosk_model_free(model);
	return 0;
}

