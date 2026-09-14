#include "pathtool.h"
#include <QFileDialog>
#include <QStandardPaths>

QString QPathTool::selectFolder()
{
    // Открываем диалог выбора директории
    QString dir = QFileDialog::getExistingDirectory(
        nullptr,
        tr("Выберите папку с изображениями"),
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    return dir; // Возвращает пустую строку, если пользователь нажал "Отмена"
}
