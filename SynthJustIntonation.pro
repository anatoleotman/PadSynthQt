QT += quick \
        charts \
        multimedia\
#        datavisualization

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        PADsynth.cpp \
        envelopegenerator.cpp \
        fftwrapper/FFTwrapper.cpp \
        filter.cpp \
        generator.cpp \
        main.cpp \
        mypadsynth.cpp \
        padsynthalgorithm.cpp \
        padsynthgraph.cpp

RESOURCES += qml.qrc

INCLUDEPATH += c:/Users/jonny/dev_tools_anatole/fftw-3.3.5-dll64 -lfftw3-3 \
                c:/Users/jonny/dev_tools_anatole/armadillo-10.2.2/include

LIBS += -Lc:/Users/jonny/dev_tools_anatole/fftw-3.3.5-dll64 -lfftw3-3 \
#        -Lc:/Users/jonny/dev_tools_anatole/armadillo-10.2.2/examples/lib_win64 -llibopenblas

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    PADsynth.h \
    envelopegenerator.h \
    fftwrapper/FFTwrapper.h \
    filter.h \
    generator.h \
    mypadsynth.h \
    padsynthalgorithm.h \
    padsynthgraph.h

