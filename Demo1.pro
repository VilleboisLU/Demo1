TARGET = Demo1
TEMPLATE = app
QT += core network sql
CONFIG += console

HEADERS += \
           src/controller/logincontroller.h \
           src/global.h \
           src/requestmapper.h \
           src/controller/dumpcontroller.h \
           src/controller/templatecontroller.h \
           src/controller/formcontroller.h \
           src/controller/fileuploadcontroller.h \
           src/controller/sessioncontroller.h

SOURCES += src/main.cpp \
           src/controller/logincontroller.cpp \
           src/global.cpp \
           src/requestmapper.cpp \
           src/controller/dumpcontroller.cpp \
           src/controller/templatecontroller.cpp \
           src/controller/formcontroller.cpp \
           src/controller/fileuploadcontroller.cpp \
           src/controller/sessioncontroller.cpp

OTHER_FILES += etc/* etc/docroot/* etc/templates/* etc/ssl/* logs/*

DISTFILES += ../CHANGELOG.txt ../LICENSE.txt ../README.txt \
    db.sqlite \
    etc/ssl/README.txt

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../QtWebApp/logging/logging.pri)
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)

