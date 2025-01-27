// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

ApplicationWindow {
    width: 640
    height: 480

    property alias dialog: dialog

    MessageDialog {
        id: dialog
        objectName: "MessageDialog"
        buttons: MessageDialog.Yes | MessageDialog.No | MessageDialog.Discard | MessageDialog.Apply
    }
}
