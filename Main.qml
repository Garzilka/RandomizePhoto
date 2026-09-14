import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    width: 900
    height: 600
    visible: true
    title: qsTr("Nomad")


    readonly property color colorBg: "#121214"       // Глубокий темный фон
    readonly property color colorSurface: "#1e1e24"  // Фон блоков
    readonly property color colorAccent: "#8a2be2"   // Акцентный фиолетовый
    readonly property color colorText: "#e3e3e6"     // Основной текст
    readonly property color colorTextDim: "#888890"  // Приглушенный текст

    color: colorBg

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        RowLayout
        {
            Layout.fillWidth: true
            spacing: 10

            Button
            {
                text: "Добавить путь"

                onClicked:
                {
                    let chosenPath = PathTool.selectFolder()
                    if (chosenPath !== "")
                    {
                        console.log("Пользователь выбрал путь:", chosenPath)
                        ItemManager.addItem(chosenPath);
                    } else {
                        console.log("Выбор пути отменен")
                    }
                }

                contentItem: Text
                {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle
                {
                    implicitWidth: 140
                    implicitHeight: 40
                    color: parent.hovered ? Qt.lighter(colorAccent, 1.2) : colorAccent
                    radius: 8
                }
            }


            Item { Layout.fillWidth: true }

            Text
            {
                text: "Nomad"
                font.pointSize: 24
                color: colorAccent
            }
        }

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 15


            Rectangle
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 7
                color: colorSurface
                radius: 10
                border.color: "#2a2a32"
                clip: true

                ListView
                {
                    id: itemsListView
                    anchors.fill: parent
                    anchors.margins: 10
                    model: ItemManager
                    spacing: 8

                    delegate: Rectangle
                    {
                        id: delegateRoot
                        property var itemObject: model.itemObject

                        width: itemsListView.width
                        height: 50
                        color: index === itemsListView.currentIndex ? "#2a2a36" : "#16161a"
                        radius: 6
                        border.color: index === itemsListView.currentIndex ? colorAccent : "transparent"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 15
                            anchors.rightMargin: 15
                            spacing: 10

                            // Кликабельная зона для текста (выбирает элемент)
                            MouseArea
                            {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                onClicked: {
                                    itemsListView.currentIndex = index
                                    console.log("Выбран элемент с индексом:", index)
                                }

                                Text
                                {
                                    anchors.fill: parent
                                    verticalAlignment: Text.AlignVCenter
                                    // ИСПРАВЛЕНИЕ: Читаем путь из нашей переменной itemObject
                                    text: delegateRoot.itemObject ? delegateRoot.itemObject.path : "Test"
                                    color: colorText
                                    font.pointSize: 11
                                    elide: Text.ElideMiddle
                                }
                            }

                            // Индикатор включенной настройки
                            Rectangle
                            {
                                width: 10
                                height: 10
                                radius: 5
                                // Читаем булевое значение из itemObject
                                color: (delegateRoot.itemObject && delegateRoot.itemObject.enable) ? "#10b981" : "#ef4444"
                                Layout.alignment: Qt.AlignVCenter
                            }

                            // Кнопка удаления элемента
                            Button
                            {
                                id: deleteButton
                                text: "✕"
                                implicitWidth: 30
                                implicitHeight: 30
                                Layout.alignment: Qt.AlignVCenter

                                onClicked:
                                {
                                    console.log("Нажата кнопка удаления для индекса:", index)
                                    ItemManager.removeItem(index)
                                }

                                background: Rectangle
                                {
                                    color: deleteButton.hovered ? "#3b1a1a" : "transparent"
                                    radius: 4
                                }
                                contentItem: Text
                                {
                                    text: deleteButton.text
                                    color: deleteButton.hovered ? "#ef4444" : colorTextDim
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }

            Rectangle
            {
                id: settingsBlock
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 3
                color: colorSurface
                radius: 10
                border.color: "#2a2a32"

                // Создаем удобное свойство-ссылку на текущий выбранный объект C++ QItem
                // Мы безопасно проверяем, есть ли вообще выделенная строка в ListView
                readonly property var currentItemObject: (itemsListView.currentIndex >= 0 && itemsListView.currentItem)
                                                         ? itemsListView.currentItem.itemObject
                                                         : null

                // СОСТОЯНИЕ 1: Если предмет НЕ выбран (список пуст или сброшен)
                Text
                {
                    anchors.centerIn: parent
                    text: "Выберите предмет\nдля настройки"
                    color: colorTextDim
                    font.pointSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    visible: !settingsBlock.currentItemObject // Видно, только если объект null
                }

                ColumnLayout
                {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 15
                    visible: settingsBlock.currentItemObject

                    Text {
                        text: "Параметры"
                        font.pointSize: 14
                        font.bold: true
                        color: colorAccent
                        Layout.fillWidth: true
                    }

                    // Разделительная линия
                    Rectangle
                    {
                        Layout.fillWidth: true
                        height: 1
                        color: "#2a2a32"
                    }

                    // Чекбокс для enable
                    CheckBox
                    {
                        id: enableCheckBox
                        text: "Включен"
                        Layout.fillWidth: true
                        hoverEnabled: false

                        checked: settingsBlock.currentItemObject ? settingsBlock.currentItemObject.enable : false

                        onCheckedChanged:
                        {
                            if (settingsBlock.currentItemObject &&
                                settingsBlock.currentItemObject.enable !== checked)
                            {

                                console.log("QML: enable изменен на ", checked)
                                settingsBlock.currentItemObject.enable = checked
                            }
                        }

                        background: Item
                        {
                            implicitWidth: lightChangeCheckBox.implicitWidth
                            implicitHeight: lightChangeCheckBox.implicitHeight
                        }

                        indicator: Rectangle
                        {
                            implicitWidth: 20
                            implicitHeight: 20
                            x: enableCheckBox.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 4
                            color: colorBg
                            border.color: enableCheckBox.checked ? colorAccent : "#444"

                            Rectangle {
                                width: 12
                                height: 12
                                x: 4
                                y: 4
                                radius: 2
                                color: colorAccent
                                visible: enableCheckBox.checked
                            }
                        }

                        contentItem: Text
                        {
                            text: enableCheckBox.text
                            font.pointSize: 11
                            color: colorText
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: enableCheckBox.indicator.width + enableCheckBox.spacing
                        }
                    }

                    // Чекбокс для lightChange
                    CheckBox
                    {
                        id: lightChangeCheckBox
                        enabled: false
                        text: "Яркость"
                        Layout.fillWidth: true
                        visible: false

                        hoverEnabled: false

                        checked: settingsBlock.currentItemObject ? settingsBlock.currentItemObject.canLightChange : false
                        onCheckedChanged:
                        {
                            if (settingsBlock.currentItemObject &&
                                settingsBlock.currentItemObject.canLightChange !== checked)
                            {
                                console.log("QML: canLightChange изменен на ", checked)
                                settingsBlock.currentItemObject.canLightChange = checked
                            }
                        }

                        background: Item
                        {
                            implicitWidth: lightChangeCheckBox.implicitWidth
                            implicitHeight: lightChangeCheckBox.implicitHeight
                        }

                        indicator: Rectangle {
                            implicitWidth: 20
                            implicitHeight: 20
                            x: lightChangeCheckBox.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 4
                            color: colorBg
                            border.color: lightChangeCheckBox.checked ? colorAccent : "#444"

                            Rectangle {
                                width: 12
                                height: 12
                                x: 4
                                y: 4
                                radius: 2
                                color: colorAccent
                                visible: lightChangeCheckBox.checked
                            }
                        }

                        contentItem: Text
                        {
                            text: lightChangeCheckBox.text
                            font.pointSize: 11
                            color: colorText
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: lightChangeCheckBox.indicator.width + lightChangeCheckBox.spacing
                        }
                    }

                    // Чекбокс для mirrorChange
                    CheckBox
                    {
                        id: mirrorChangeCheckBox
                        text: "Зеркалить"
                        enabled: false
                        visible: false
                        Layout.fillWidth: true
                        hoverEnabled: false

                        checked: settingsBlock.currentItemObject ? settingsBlock.currentItemObject.canMirrorChange : false

                        onCheckedChanged:
                        {
                            if (settingsBlock.currentItemObject &&
                                settingsBlock.currentItemObject.canMirrorChange !== checked)
                            {

                                console.log("QML: canMirrorChange изменен на ", checked)
                                settingsBlock.currentItemObject.canMirrorChange = checked
                            }
                        }

                        indicator: Rectangle
                        {
                            implicitWidth: 20
                            implicitHeight: 20
                            x: mirrorChangeCheckBox.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 4
                            color: colorBg
                            border.color: mirrorChangeCheckBox.checked ? colorAccent : "#444"

                            Rectangle
                            {
                                width: 12
                                height: 12
                                x: 4
                                y: 4
                                radius: 2
                                color: colorAccent
                                visible: mirrorChangeCheckBox.checked
                            }
                        }

                        contentItem: Text
                        {
                            text: mirrorChangeCheckBox.text
                            font.pointSize: 11
                            color: colorText
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: mirrorChangeCheckBox.indicator.width + mirrorChangeCheckBox.spacing
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }

        ColumnLayout
        {
            Layout.fillWidth: true
            spacing: 6


            RowLayout
            {

                Text
                {
                    text: "Всего картинок: "
                    font.pointSize: 12
                    color: colorText
                    textFormat: Text.RichText
                }
                Text
                {
                    text: ItemManager.AllNumImg
                    font.pointSize: 12
                    color: colorText
                    textFormat: Text.RichText
                }
            }

            RowLayout
            {
                Text
                {
                    text: "Требуемое количество фото: "
                    font.pointSize: 12
                    color: colorText
                }
                TextField
                {
                    id: countInputField

                    // 1. ЧТЕНИЕ: Берем значение и превращаем в строку
                    text: ItemManager.targeNumImg.toString()

                    implicitWidth: 80
                    implicitHeight: 28
                    color: "white"

                    // Заставляем поле принимать только целые числа (например, от 1 до 99999)
                    validator: IntValidator { bottom: 1; top: 99999 }

                    // 2. ЗАПИСЬ: Как только текст изменился, отправляем число
                    onTextChanged:
                    {
                        let value = parseInt(text) || 0
                        if (ItemManager.targeNumImg !== value)
                        {
                            ItemManager.targeNumImg = value
                        }
                    }

                    background: Rectangle
                    {
                        color: colorSurface;
                        radius: 4
                        border.color: parent.activeFocus ? colorAccent : "transparent"
                    }
                }
            }

            RowLayout
            {
                Text
                {
                    text: "Стартовый номер: "
                    font.pointSize: 12
                    color: colorText
                }
                TextField
                {
                    id: countStartField

                    // 1. ЧТЕНИЕ: Берем значение и превращаем в строку
                    text: ItemManager.startNumImg.toString()

                    implicitWidth: 80
                    implicitHeight: 28
                    color: "white"

                    // Заставляем поле принимать только целые числа (например, от 1 до 99999)
                    validator: IntValidator { bottom: 1; top: 99999 }

                    // 2. ЗАПИСЬ: Как только текст изменился, отправляем число
                    onTextChanged:
                    {
                        let value = parseInt(text) || 0
                        if (ItemManager.startNumImg !== value)
                        {
                            ItemManager.startNumImg = value
                        }
                    }

                    background: Rectangle {
                        color: colorSurface;
                        radius: 4
                        border.color: parent.activeFocus ? colorAccent : "transparent"
                    }
                }
            }
            RowLayout
            {
                Text
                {
                    text: "Финальный путь: "
                    font.pointSize: 12
                    color: colorText
                    textFormat: Text.RichText
                }
                Button
                {
                    text: "Выбрать"

                    onClicked:
                    {
                        let chosenPath = PathTool.selectFolder()
                        if (chosenPath !== "")
                        {
                            console.log("Пользователь выбрал путь:", chosenPath)
                            ItemManager.setOutPath(chosenPath);
                        } else
                        {
                            console.log("Выбор пути отменен")
                        }
                    }

                    contentItem: Text
                    {
                        text: parent.text
                        color: "white"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle
                    {
                        implicitWidth: 90
                        implicitHeight: 20
                        color: parent.hovered ? Qt.lighter(colorAccent, 1.2) : colorAccent
                        radius: 8
                    }
                }
            }
        }

        RowLayout
        {
            Layout.fillWidth: true
            spacing: 15

            TextField
            {
                Layout.fillWidth: true
                placeholderText: ItemManager.OutPath
                color: colorText
                font.pointSize: 11
                padding: 12
                readOnly: true

                background: Rectangle
                {
                    color: colorSurface
                    radius: 8
                    border.color: parent.activeFocus ? colorAccent : "#2a2a32"
                }
            }

            Button
            {
                text: "Начать"

                onClicked:
                {
                    ItemManager.startprocess();
                }
                contentItem: Text
                {
                    text: parent.text
                    color: "white"
                    font.pointSize: 12
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle
                {
                    implicitWidth: 120
                    implicitHeight: 44
                    color: parent.hovered ? Qt.lighter("#10b981", 1.1) : "#10b981" // Зеленый "Go" цвет
                    radius: 8
                }
            }
        }
    }
}
