TEMPLATE     = app
TARGET       = QtdSync
LANGUAGE     = C++
INCLUDEPATH += ./include
CONFIG      += qt warn_on debug_and_release
#CONFIG     += console
QT 			+= xml network
DEFINES     += QTD_FORCE_API_EXPORT

greaterThan(QT_MAJOR_VERSION, 4) {
    QT      += widgets
}

#CONFIG     += qtd_upx
CONFIG      += qtd_noupdateclient

win32 {
LIBS        += version.lib
CONFIG      += WIN32
QMAKE_CXXFLAGS -= -Zc:strictStrings 
}

CONFIG(debug, debug|release) {
	win32 {
    	DESTDIR = debug
    } else {
    	DESTDIR = /tmp/qtdsync_make/debug
    	OBJECTS_DIR = /tmp/qtdsync_make/debug
    }
    	
    win32-msvc2005 {
        QMAKE_CXXFLAGS += /Fddebug\\QtdSync.pdb
    }
    INCLUDEPATH += debug
} else {
    win32 {
        CONFIG  += qtdbase_lib
        #CONFIG  += qtdclasses_lib
    	DESTDIR = release
        win32-msvc2005 {
            QMAKE_CXXFLAGS  += /Fdrelease\\QtdSync.pdb
            QMAKE_LFLAGS += /MANIFEST:NO
        }
    } else {
        DESTDIR = /tmp/qtdsync_make/release
        OBJECTS_DIR = /tmp/qtdsync_make/release
    }
    INCLUDEPATH     += release
}

SOURCES     =   src/main.cpp \
                src/QtdSync.BackupEngine.cpp \
                src/QtdSync.BackupEngine.Rsync.cpp \
                src/QtdSync.Location.cpp \
                src/QtdSync.Location.Local.cpp \
                src/QtdSync.Location.Rsync.cpp

HEADERS     =   include/QtdSync.BackupEngine.h \
                include/QtdSync.BackupEngine.Rsync.h \
                include/QtdSync.Location.h \
                include/QtdSync.Location.Local.h \
                include/QtdSync.Location.Rsync.h \
                include/QtdAppInfo.h

#RESOURCES   =   QtdSync.qrc



win32 {
    RC_FILE   =  QtdSync.rc
#    RESOURCES += QtdSync_Windows.qrc
#    win32-msvc2005 {
#        RESOURCES += QtdSyncTranslationHelp.qrc
#    }
} else {
#    RESOURCES += QtdSync_Linux.qrc
}

TRANSLATIONS +=  translations/qtdsync_de_DE.ts \
                 translations/qtdsync_en_US.ts \
                 translations/qtdsync_fr_FR.ts
                 
include($(QTD_SHARED)/QtdClasses/QtdClasses.pri)
include($(QTD_SHARED)/QtdBase/QtdBase.pri)
