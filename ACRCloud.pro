QT       += core gui network multimedia positioning opengl quick qml quickwidgets multimediawidgets webview gui-private

CONFIG +=  mobility localization
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app
TARGET= WANDIZZ
CONFIG += c++11

INSTALLS +=target
DEFINES += QT_DEPRECATED_WARNINGS \
            VER=2349 \
            VER_STRING='2-3-4-9'
SOURCES += main.cpp \
    audiorecorder.cpp \
    aws_api.cpp \
    audiolevel.cpp \
    acr_api.cpp \
    browser.cpp \
    circles.cpp \
    downloader.cpp \
    downloaderStd.cpp \
    mainwidget.cpp \
    menu.cpp \
    mhelper.cpp \
    myvideosurface.cpp \
    permissions.cpp \
    signin.cpp \
    socialLogin.cpp \
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
    userPermissions.cpp \
    userprof.cpp \
    widget_mapper.cpp \
    widget_wrapper.cpp \
    write_buffers.cpp

HEADERS += \
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
    syncscreen.h \
    userPermissions.h \
    userprof.h \
    widget_mapper.h \
    widget_wrapper.h \
    write_buffers.h

FORMS += \
    menu.ui \
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
#LIBS += -framework FBSDKCoreKit.framework FBSDKLoginKit.framework UIKit
HEADERS += shareios.h
SOURCES += shareios.mm
CONFIG +=  iphoneos
QMAKE_IOS_DEPLOYMENT_TARGET = 12.0
QMAKE_INFO_PLIST = inf.plist
QMAKE_TARGET_BUNDLE_PREFIX=WANDIZZ
QMAKE_ASSET_CATALOGS += Media.xcassets
QMAKE_BUNDLE_DATA += ios_icon
QMAKE_BUNDLE=WANDIZZ
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
    include(android/ssl/openssl.pri)
    VERSION = 1.2.6
    ANDROID_VERSION_CODE = '16'
    ANDROID_VERSION_NAME = 'WANDIZZ beta'
    ANDROID_MIN_SDK_VERSION = '15'
    ANDROID_TARGET_SDK_VERSION = '26'

    RESOURCES += \
        images.qrc \
        qml.qrc


ANDROID_ABIS = armeabi-v7a arm64-v8a
SUPPORTED_ABIS = armeabi-v7a arm64-v8a
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

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

ANDROID_ABIS = armeabi-v7a
}
