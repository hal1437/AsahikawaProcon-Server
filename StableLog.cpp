#include "StableLog.h"



bool StableLog::Write(const QString& str) const{
    //追加書き込みオープン
    QFile* file = new QFile(filename);
    file->open(QIODevice::Append);
    QTextStream log;
    log.setDevice(file);

    //書いて
    log << str;

    //閉じる
    delete file;
}

const StableLog& StableLog::operator<<(const QString& str)const{
    Write(str);
    return (*this);
}

StableLog::StableLog()
{

}

StableLog::StableLog(QString filename) : filename(filename){

    QFile* file = new QFile(filename);
    QTextStream log;
    file->open(QIODevice::WriteOnly);
    log.setDevice(file);
    //ログファイルオープン
    log.setDevice(file);
    log << "--Stable Log--\n";

    delete file;
}

StableLog::~StableLog()
{

}

