#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QApplication>
#include "MainDialog.h"
#include "main.h"

void CleanupLock(void *param)
{
	pthread_mutex_unlock((pthread_mutex_t *)param);
}

int main(int argc, char *argv[])
{
	int err;
	char enabled[256] = {0};

#if 1
	FILE *f = 0;
	char *p = 0;

	err = system("amixer cset name='Left Input Mixer L2 Switch' on");
	err = system("amixer cset name='Right Input Mixer R2 Switch' on");
	err = system("amixer cset name='Headphone Playback Volume' 100%");
	err = system("amixer cset name='PCM Volume' 90%");
	err = system("amixer cset name='Input PGA Volume' 50%");

	f = fopen("/sys/class/drm/card0-HDMI-A-1/enabled", "r");
	p = fgets(enabled, sizeof(enabled), f);
	fclose(f), f = 0;
	if (0 != strncmp(enabled, "enabled", 7)) {
		printf("Please connect the HDMI port. \n");
		return 1;
	}
#endif

	QApplication a(argc, argv);

	MainDialog d;
	d.setWindowFlags(d.windowFlags() & ~Qt::FramelessWindowHint);
	d.show();
	return a.exec();
}

