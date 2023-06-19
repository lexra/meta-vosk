#ifndef _MAIN_H_
#define _MAIN_H_

#include <pthread.h>
#include <alsa/asoundlib.h>
#include <vosk_api.h>
#include "list.h"

#define MODEL_PREFIX	"/usr/share/vosk"

#define SAMPLE_RATE     24000
#define BUFFER_FRAMES	(SAMPLE_RATE / 10)
#define BUFFER_SIZE	(BUFFER_FRAMES * 16 / 8)
#define RING_NUMBER	128

struct pool_t {
        struct list_head list;
        char *b;
        int bsize;
};

extern VoskRecognizer *recognizer;
extern VoskModel *model;
extern struct pool_t pl;
extern pthread_t tCapture;
extern pthread_cond_t cCaptured;
extern int pCaptured;
extern pthread_mutex_t mCaptured;
extern snd_pcm_t *capture_handle;

extern void CleanupLock(void *param);

#endif // _MAIN_H_
