package upgrader.upgrader;

import org.qtproject.qt5.android.QtNative;

import java.lang.String;
import java.io.File;
import android.content.Intent;
import android.util.Log;
import android.net.Uri;

import android.content.ContentValues;
import android.content.Context;

public class upgrader
{
    public static int installApp(String appPackageName) {
        if (QtNative.activity() == null)
            return -1;
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.setDataAndType(Uri.fromFile(new File(appPackageName)),"application/vnd.android.package-archive");
            QtNative.activity().startActivity(intent);
            return 0;
        } catch (android.content.ActivityNotFoundException anfe) {
            return -3;
        }
    }

}
