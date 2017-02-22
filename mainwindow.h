#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionOpen_ROM_triggered();

    void on_actionPlugin_Paths_triggered();

    void on_actionStop_Game_triggered();

    void on_actionExit_triggered();

    void on_actionPlugin_Settings_triggered();

    void on_actionPause_Game_triggered();

    void on_actionMute_triggered();

    void on_actionHard_Reset_triggered();

    void on_actionSoft_Reset_triggered();

    void on_actionTake_Screenshot_triggered();

    void on_actionSave_State_triggered();

    void on_actionLoad_State_triggered();

    void on_actionToggle_Fullscreen_triggered();

    void on_actionSave_State_To_triggered();

    void on_actionLoad_State_From_triggered();

private:
    void openROM(QString filename);
    void updateOpenRecent();
    void findRecursion(const QString &path, const QString &pattern, QStringList *result);
    Ui::MainWindow *ui;
    QMenu * OpenRecent;
};
extern MainWindow *w;

#endif // MAINWINDOW_H
