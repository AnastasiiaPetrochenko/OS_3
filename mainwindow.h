#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_createProcesses_clicked();
    void on_priorityBox_currentIndexChanged(int index);
    void on_Pause_clicked();
    void on_Terminate_clicked();
    void on_Start_clicked();
    void on_pushButton_clicked();
    void showProcessData();



private:
    Ui::MainWindow *ui;

    // Масиви для зберігання інформації про процеси
    STARTUPINFO *si = nullptr;
    PROCESS_INFORMATION *pi = nullptr;

    // Функція для отримання строкового представлення пріоритету процесу
    QString priority(DWORD code);
};

#endif // MAINWINDOW_H
