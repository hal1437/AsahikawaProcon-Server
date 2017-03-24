#ifndef STABLELOG_H
#define STABLELOG_H
#include <QString>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QTextStream>

class StableLog
{
public:
    QString filename;
public:

    bool Write(const QString& str)const;

    const StableLog& operator<<(const QString& str)const;

    StableLog();
    StableLog(QString filename);
    ~StableLog();
};

#endif // STABLELOG_H
