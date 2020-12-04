
// Java code to request external storage read/write runtime permissions from the Android Marshmallow (6.0) operating system
package wandizz.usersPermissions;

import android.Manifest;
import android.app.Activity;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;
import android.content.pm.PackageManager;
import java.lang.Runnable;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import java.util.ArrayList;
import java.util.List;
import android.content.Intent;
import android.os.Bundle;
import android.app.FragmentManager;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.os.Build;
public int currentAPIVersion = ANDROID_MIN_SDK_VERSION;
if(currentAPIVersion>=android.os.Build.VERSION_CODES.M)
 {
    if (ContextCompat.checkSelfPermission(context, Manifest.permission.INTERACT_ACROSS_USERS) != PackageManager.PERMISSION_GRANTED) {
        if (ActivityCompat.shouldShowRequestPermissionRationale((Activity) context, Manifest.permission.INTERACT_ACROSS_USERS)) {
            AlertDialog.Builder alertBuilder = new AlertDialog.Builder(context);
            alertBuilder.setCancelable(true);
            alertBuilder.setTitle("Permission necessary");
            alertBuilder.setMessage("Interact across users permission is necessary to this app");
            alertBuilder.setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    ActivityCompat.requestPermissions((Activity)context, new String[]{Manifest.permission.INTERACT_ACROSS_USERS}, MY_PERMISSIONS_REQUEST_INTERACT_ACROSS_USERS);
                }
            });
            AlertDialog alert = alertBuilder.create();
            alert.show();
        } else {
            ActivityCompat.requestPermissions((Activity)context, new String[]{Manifest.permission.INTERACT_ACROSS_USERS}, MY_PERMISSIONS_REQUEST_INTERACT_ACROSS_USERS);
        }
    }
}


 @Override
public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
    switch (requestCode) {
 case MY_PERMISSIONS_REQUEST_INTERACT_ACROSS_USERS:
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
               // do something here...
               Toast.makeText(getApplicationContext(), "Permission granted", Toast.LENGTH_SHORT).show();
        } else {
         //code for deny
         Toast.makeText(getApplicationContext(), "Permission denied", Toast.LENGTH_SHORT).show();

}
         break;
    }
}
