#ifndef PATHTOOL_H
#define PATHTOOL_H

#pragma once
#include <QObject>
#include <QString>

class QPathTool : public QObject
{
    Q_OBJECT
public:
    explicit QPathTool(QObject *parent = nullptr) : QObject(parent) {};
    Q_INVOKABLE QString selectFolder();
};

#endif // PATHTOOL_H
