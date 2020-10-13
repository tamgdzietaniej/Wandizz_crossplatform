import QtQuick 2.9
import QuickNative 0.1

Item {
    id:itm
    signal completed(msg: string)
    NativeFileDialog {
        selectMultiple: false
        folder: shortcuts.pictures
        Component.onCompleted: {
            open()
        }
        onFileUrlChanged: {
            console.log("FILE PICKUP:"+fileUrl);
            itm.completed(fileUrl);
        }
    }
}
