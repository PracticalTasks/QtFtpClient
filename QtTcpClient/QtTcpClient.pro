# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = QtTcpClient
DESTDIR = ../x64/Debug
CONFIG += debug
LIBS += -L"../../../socket_wrapper/Debug" \
    -lsocket-wrapper \
    -lwsock32 \
    -lws2_32 \
    -lkernel32 \
    -luser32 \
    -lgdi32 \
    -lwinspool \
    -lshell32 \
    -lole32 \
    -loleaut32 \
    -luuid \
    -lcomdlg32 \
    -ladvapi32
DEPENDPATH += .
MOC_DIR += .
OBJECTS_DIR += debug
UI_DIR += .
RCC_DIR += .
HEADERS += ./PaintWdg.h \
    ./QtTcpClient.h
SOURCES += ./PaintWdg.cpp \
    ./QtTcpClient.cpp \
    ./main.cpp
FORMS += ./QtTcpClient.ui
RESOURCES += QtTcpClient.qrc
