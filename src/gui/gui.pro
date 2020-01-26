TEMPLATE = app
TARGET = gui
INCLUDEPATH += .
QT += opengl
LIBS += -lrt
SOURCES += main.cpp \
	DisplayWidget.cpp
HEADERS += DisplayWidget.h
