QT       += core gui network opengl quick qml quickwidgets  gui-private
lessThan(QT_MAJOR_VERSION,6): QT += widgets multimedia multimediawidgets webview positioning
CONFIG +=  mobility localization
CONFIG += c++11
CONFIG += QMAKE_LFLAGS_CONSOLE
CONFIG += QMAKE_CFLAGS_WARN_OFF
TEMPLATE = app
TARGET= WANDIZZ

INSTALLS +=target
DEFINES += QT_DEPRECATED_WARNINGS \
            VER=2349 \
            VER_STRING='2-3-4-9'
SOURCES += main.cpp \
    appinspector.cpp \
    audiorecorder.cpp \
    aws_api.cpp \
    audiolevel.cpp \
    acr_api.cpp \
    browser.cpp \
    circles.cpp \
    downloader.cpp \
    downloaderStd.cpp \
    downloader_kopia.cpp \
    mainwidget.cpp \
    menu.cpp \
    mhelper.cpp \
    myvideosurface.cpp \
    permissions.cpp \
    signin.cpp \
    socialLogin.cpp \
    statcs.cpp \
    syncscreen.cpp \
    favitems.cpp \
    mypushbutton.cpp \
    credentials.cpp \
    navigation.cpp \
    sqlapi.cpp \
    gllabel.cpp \
    carousel2.cpp \
    localization.cpp \
    geofit.cpp \
    profsettings.cpp \
    topmenuswitcher.cpp \
    userPermissions.cpp \
    userprof.cpp \
    widgetgen.cpp \
    write_buffers.cpp# \
   # metasearch.cpp

HEADERS += \
    appinspector.h \
audiolevel.h \
    acr_api.h \
    audiorecorder.h \
    aws_api.h \
    browser.h \
    carousel2.h \
    circles.h \
    credentials.h \
    downloaderStd.h \
    downloader.h \
    downloader_kopia.h \
    favitems.h \
    geofit.h \
    globals.h \
    localization.h \
    mainwidget.h \
    menu.h \
    mhelper.h \
    mypushbutton.h \
    myvideosurface.h \
    navigation.h \
    gllabel.h \
    permissions.h \
    profsettings.h \
    signin.h \
    socialLogin.h \
    sqlapi.h \
    statcs.h \
    syncscreen.h \
    topmenuswitcher.h \
    userPermissions.h \
    userprof.h \
    widgetgen.h \
    write_buffers.h# \
   # metasearch.h

FORMS += \
    menu.ui \
   # metasearch.ui \
    signin.ui \
    splash.ui \
    syncscreen.ui \
    favitems.ui \
    carousel2.ui \
    profsettings.ui \
    userprof2.ui \
    webhelper.ui
    RESOURCES += \
        images.qrc \
        qml.qrc




ios{
HEADERS += shareios.h
SOURCES += shareios.mm
CONFIG +=  iphoneos
QMAKE_IOS_DEPLOYMENT_TARGET = 12.0
QMAKE_INFO_PLIST = inf.plist
QMAKE_TARGET_BUNDLE_PREFIX=WANDIZZ
QMAKE_ASSET_CATALOGS += Media.xcassets
QMAKE_BUNDLE_DATA += ios_icon
QMAKE_BUNDLE=WANDIZZ
CURRENT_IOS_PROJECT_VERSION=6.8
CURRENT_IOS_SDK_VERSION=13.2
#OBJECTIVE_SOURCES += shareios.mm
app_launch_images.files += $$files($$PWD/gui/splash/*)
QMAKE_BUNDLE_DATA += app_launch_images
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android{
     include($$PWD/vendor/vendor.pri)
    QT += androidextras
    HEADERS += share_cross.h
    SOURCES += share_cross.cpp
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_ABIS = armeabi-v7a arm64-v8a
    include(android/ssl/openssl.pri)
    VERSION = 1.2.7
    ANDROID_VERSION_CODE = '8352'
    ANDROID_VERSION_NAME = 'WANDIZZ beta(17)'
    ANDROID_MIN_SDK_VERSION = '15'
    ANDROID_TARGET_SDK_VERSION = '29'

    RESOURCES += \
        images.qrc \
        qml.qrc


DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/src/org/bytran/bytran/RequestPermissions.java \
    vendor/vendor.pri \
    android/src/wandizz/shares.java \
    android/src/wandizz/shares/shares.java
}

DISTFILES += \
    gui/APP ICONS/android/android-contact 13.09.56 13.09.56 13.09.56 13.09.56 13.09.56.png \
    gui/icons/avatar_mini.png \
    main.qml


