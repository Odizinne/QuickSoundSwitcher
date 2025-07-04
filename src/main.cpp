#include "quicksoundswitcher.h"
#include <QApplication>
#include <QProcess>

bool isAnotherInstanceRunning(const QString& processName)
{
    QProcess process;
    process.start("tasklist", QStringList() << "/FI" << QString("IMAGENAME eq %1").arg(processName));
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    int count = output.count(processName, Qt::CaseInsensitive);

    return count > 1;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setOrganizationName("Odizinne");
    a.setApplicationName("QuickSoundSwitcher");

    const QString processName = "QuickSoundSwitcher.exe";
    if (isAnotherInstanceRunning(processName)) {
        qDebug() << "Another instance is already running. Exiting...";
        return 0;
    }

    QuickSoundSwitcher w;

    return a.exec();
}
