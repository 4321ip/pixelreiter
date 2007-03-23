# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: .
# Das Target ist eine Anwendung:  

HEADERS += ColorCount.h \
           MainWindow.h \
           SelWin.h \
           ZoomWidget.h \
           ColorCountModel.h 
SOURCES += ColorCount.cpp \
           main.cpp \
           MainWindow.cpp \
           SelWin.cpp \
           ZoomWidget.cpp \
           ColorCountModel.cpp 
CONFIG += debug \
qt \
warn_on
TEMPLATE = app
TARGET += 
DEPENDPATH += .
MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = ui
DISTFILES += COPYING
QT += core \
gui
INCLUDEPATH += .

