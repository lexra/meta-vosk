QT += core gui
QT += widgets

TARGET = qvosk
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++17 -Wall -Wextra \
	-Wno-unused-but-set-variable -Wno-unused-parameter \
	-Wno-effc++ -Wno-deprecated-copy -Wno-unused-variable \
	-Wno-unused-label -Wno-unused-function

INCLUDEPATH += recipe-sysroot/usr/include/vosk
LIBS += -Lrecipe-sysroot/usr/lib64 -ldl -lpthread -lasound -lvosk

unix {
	QMAKE_CXXFLAGS += -Werror
}

SOURCES += \
	main.cpp \
	MainDialog.cpp \
	QVosk.cpp 

FORMS += \
	MainDialog.ui \

HEADERS += \
	MainDialog.h \
	QVosk.h \
	main.h \
	list.h

