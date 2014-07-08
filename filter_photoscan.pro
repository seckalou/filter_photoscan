include (../../shared.pri)


SOURCES = filter_photoscan.cpp \
    photoscancamerareader.cpp


HEADERS = filter_photoscan.h \
    photoscancamerareader.h

TARGET = filter_photoscan

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../opencv-2.4.9/release/lib/release/ -lopencv_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../opencv-2.4.9/release/lib/debug/ -lopencv_core
else:symbian: LIBS += -lopencv_core
else:unix: LIBS += -L$$PWD/../../../../../opencv-2.4.9/release/lib/ -lopencv_core

INCLUDEPATH += $$PWD/../../../../../opencv-2.4.9/release/include
DEPENDPATH += $$PWD/../../../../../opencv-2.4.9/release/include

INCLUDEPATH += /home/alou/rapidxml-1.13
