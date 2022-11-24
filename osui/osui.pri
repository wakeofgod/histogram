INCLUDEPATH += $${PWD}

DEPENDPATH += $${PWD}

INCLUDEPATH += $$PWD/widget

FORMS += \
    $$PWD/frmloadfile.ui \
    $$PWD/mainwindow.ui

HEADERS += \
    $$PWD/frmloadfile.h \
    $$PWD/mainwindow.h \
    $$PWD/widget/pltchart.h \
    $$PWD/widget/rbcchart.h \
    $$PWD/widget/scatterchart3d.h \
    $$PWD/widget/wbcchart.h

SOURCES += \
    $$PWD/frmloadfile.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/widget/pltchart.cpp \
    $$PWD/widget/rbcchart.cpp \
    $$PWD/widget/scatterchart3d.cpp \
    $$PWD/widget/wbcchart.cpp
