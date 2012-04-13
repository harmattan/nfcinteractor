import QtQuick 1.1
import com.nokia.symbian 1.1 // Symbian Qt Quick components
import Qt.labs.folderlistmodel 1.0

SelectionDialog {
    //id: fileSelectionDialog
    titleText: "Select image..."
    property string selectedFileName;
    property string selectedFilePath;
    property alias initialFolder: folderModel.folder
    property alias nameFilters: folderModel.nameFilters

    model: FolderListModel {
        id: folderModel
        showOnlyReadable: true
        showDirs: true
        showDotAndDotDot: true              // Bug: doesn't work on Symbian
        sortField: FolderListModel.Name
        folder: "file://"
        //nameFilters: ["*.*"]
        nameFilters: [ "*.png", "*.jpg", "*.jpeg", "*.gif" ]
    }

    delegate: fileDelegate

    Component {
        id: fileDelegate

        MenuItem {
            platformInverted: root.platformInverted
            text: folderModel.isFolder(index) ? "<i>" + fileName + "/</i>" : fileName;

            onClicked: {
                selectedIndex = index
                if (folderModel.isFolder(index)) {
                    folderModel.folder = filePath
                } else {
                    selectedFileName = fileName;
                    selectedFilePath = filePath;
                    // Cut away file:///, as this is confusing for the user and also the
                    // image / file loading classes don't find the file if "file:///" is
                    // part of the filename.
                    selectedFilePath = selectedFilePath.replace("file:///", "");
                    accept();
                }
            }
        }
    }
}
