#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

MainWindow *w;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("mupen64plus-gui");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("ROM", QCoreApplication::translate("main", "ROM to open."));
    parser.process(a);
    const QStringList args = parser.positionalArguments();

    w = new MainWindow();
    w->show();
    if (args.size() > 0)
        w->openROM(args.at(0));

    return a.exec();
}
