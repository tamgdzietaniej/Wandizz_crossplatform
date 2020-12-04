#include "userPermissions.h"
#include <QMessageBox>
#include <QApplication>

userPermissions::userPermissions(QObject *parent) : QObject(parent)
{
}

// Method to request permissions
void userPermissions::requestPermission()
{
    #if defined(Q_OS_ANDROID)

        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.INTERACT_BETWEEN_USERS");
        if (request == QtAndroid::PermissionResult::Denied)
        {
            QtAndroid::requestPermissionsSync(QStringList() <<  "android.permission.INTERACT_BETWEEN_USERS");
            request = QtAndroid::checkPermission("android.permission.INTERACT_BETWEEN_USERS");

            if (request == QtAndroid::PermissionResult::Denied)
            {
                this->permissionResult = false;
                if (QtAndroid::shouldShowRequestPermissionRationale("android.permission.INTERACT_BETWEEN_USERS"))
                {
                    ShowPermissionRationale = QAndroidJniObject("wandizz",
                                                                "(Landroid/app/Activity;)V",
                                                                 QtAndroid::androidActivity().object<jobject>()
                                                               );

                    // Checking for errors in the JNI
                    QAndroidJniEnvironment env;
                    if (env->ExceptionCheck()) {
                        // Handle exception here.
                        env->ExceptionClear();
                    }
                }
            }
            else { this->permissionResult = true; }
        }
        else { this->permissionResult = true; }

    #else

        this->permissionResult = false;

    #endif
}

// Method to get the permission granted state
bool userPermissions::getPermissionResult() { return permissionResult; }
