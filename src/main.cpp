// Copyright (C) 2023 akiaji-k

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        qDebug() << "uiLanguage: " << locale;
        const QString baseName = "filmQA_netOD_normalizer_" + QLocale(locale).name();
        qDebug() << "baseName: " << baseName;

//        qDebug() << "QCoreApplication::applicationDirPath()" << QCoreApplication::applicationDirPath();
//        if (translator.load(":/i18n/" + baseName)) {
           if (translator.load("../../i18n/" + baseName)) {
            qDebug() << "baseName: " << baseName << "loaded.";
            a.removeTranslator(&translator);
            a.installTranslator(&translator);
            break;
        }
    }
    /*
    // translate to Janapnese
    const QString baseName = "filmQA_netOD_normalizer_ja_JP";
    if (translator.load(baseName)) {
        qDebug() << "baseName: " << baseName << "loaded.";
        a.installTranslator(&translator);
    }
*/
    MainWindow w;
    w.show();
    return a.exec();
}
