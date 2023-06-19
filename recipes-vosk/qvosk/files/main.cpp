#include <stdlib.h>
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

	err = system("amixer cset name='Left Input Mixer L2 Switch' on");
	err = system("amixer cset name='Right Input Mixer R2 Switch' on");
	err = system("amixer cset name='Headphone Playback Volume' 100%");
	err = system("amixer cset name='PCM Volume' 100%");
	err = system("amixer cset name='Input PGA Volume' 100%");

	QApplication a(argc, argv);

	MainDialog d;
	d.show();
	return a.exec();
}

