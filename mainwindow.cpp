#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <windows.h>


#include <QMessageBox>
#include <QTableWidgetItem>
#include <QApplication>
#include <QElapsedTimer>
#include <QProcess>
#include <QFile>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // Ініціалізація інтерфейсу користувача

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(8);

    ui->lineEdit_A->setText("-0.9");
    ui->lineEdit_B->setText("1");
    ui->lineEdit_step->setText("0.0001");


    connect(ui->priorityBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_priorityBox_currentIndexChanged(int)));

    si = nullptr;
    pi = nullptr;
}

MainWindow::~MainWindow()
{
    // Звільняємо пам'ять і закриваємо процеси
    if (si) delete[] si;
    if (pi) delete[] pi;
    delete ui;
}
void MainWindow::on_createProcesses_clicked()
{
    double a = ui->lineEdit_A->text().toDouble();
    double b = ui->lineEdit_B->text().toDouble();
    double step = (abs(a) + abs(b)) / ui->spinBox->value();
    int processCount = ui->spinBox->value();

    // Ініціалізація масивів для процесів
    si = new STARTUPINFO[processCount];
    pi = new PROCESS_INFORMATION[processCount];

    // Очищення таблиці
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(""));
        }
    }

    for (int i = 0; i < processCount; i++) {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]); // розмір структури STARTUPINFO
        ZeroMemory(&pi[i], sizeof(pi[i]));

        QString cmd = "C:/Users/Admin/Desktop/3/build-os3-Desktop_Qt_6_6_2_MinGW_64_bit-Release/os3.exe "
                      + QString::number(a) + " " + QString::number(a + step) + " " + ui->lineEdit_step->text() + " ";
        a += step;  // Оновлення діапазону

        // Створення процесу
        CreateProcess(NULL, (TCHAR*)cmd.utf16(), NULL, NULL, FALSE,
                      NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL, NULL, &si[i], &pi[i]);

        ui->tableWidget->setItem(i, 2, new QTableWidgetItem("Suspended"));
    }

    showProcessData();
}


void MainWindow::on_priorityBox_currentIndexChanged(int index)
{
    int processIndex = ui->spinBox_2->value();
    QString priorityText = ui->priorityBox->itemText(index);

    if(priorityText == "Idle")
        SetPriorityClass(pi[processIndex].hProcess, IDLE_PRIORITY_CLASS);

    if(priorityText == "Below Normal")
        SetPriorityClass(pi[processIndex].hProcess, BELOW_NORMAL_PRIORITY_CLASS);

    if(priorityText == "Normal")
        SetPriorityClass(pi[processIndex].hProcess, NORMAL_PRIORITY_CLASS);

    if(priorityText == "Above Normal")
        SetPriorityClass(pi[processIndex].hProcess, ABOVE_NORMAL_PRIORITY_CLASS);

    if(priorityText == "High")
        SetPriorityClass(pi[processIndex].hProcess, HIGH_PRIORITY_CLASS);

    if(priorityText == "Real Time")
        SetPriorityClass(pi[processIndex].hProcess, REALTIME_PRIORITY_CLASS);

    ui->tableWidget->setItem(processIndex, 1, new QTableWidgetItem(priorityText));

    showProcessData();  // Оновлюємо дані таблиці
}

void MainWindow::on_Start_clicked()
{
    int index = ui->spinBox_2->value();
    if (index < 0 || index >= ui->spinBox->value()) {
        qDebug() << "Invalid process index!";
        return;
    }

    auto start = std::chrono::steady_clock::now();

    // Встановлюємо статус "Running" і відновлюємо виконання потоку
    ui->tableWidget->setItem(index, 2, new QTableWidgetItem("Running"));
    ResumeThread(pi[index].hThread);

    auto end = std::chrono::steady_clock::now();
    auto elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Показуємо час виконання, але не блокуємо інтерфейс
    ui->label_6->setText("Elapsed time in microseconds: " + QString::number(elapsedMicroseconds.count()));

}


void MainWindow::on_Pause_clicked()
{
    int index = ui->spinBox_2->value();
    if (index < 0 || index >= ui->spinBox->value()) {
        qDebug() << "Invalid process index!";
        return;
    }

    SuspendThread(pi[index].hThread);
    ui->tableWidget->setItem(index, 2, new QTableWidgetItem("Suspended"));

    // Оновлення даних про процеси
    showProcessData();
}
void MainWindow::on_Terminate_clicked()
{
    int index = ui->spinBox_2->value();
    if (index < 0 || index >= ui->spinBox->value()) {
        qDebug() << "Invalid process index!";
        return;
    }

    TerminateProcess(pi[index].hProcess, 0);
    ui->tableWidget->setItem(index, 2, new QTableWidgetItem("Terminated"));

    // Оновлення даних про процеси
    showProcessData();

}






void MainWindow::showProcessData() {
    for (int i = 0; i < ui->spinBox->value(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(pi[i].dwProcessId)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(priority(GetPriorityClass(pi[i].hProcess))));
    }
}

QString MainWindow::priority(DWORD code) {
    switch (code) {
    case ABOVE_NORMAL_PRIORITY_CLASS: return "Above Normal";
    case BELOW_NORMAL_PRIORITY_CLASS: return "Below Normal";
    case NORMAL_PRIORITY_CLASS: return "Normal";
    case IDLE_PRIORITY_CLASS: return "Idle";
    case HIGH_PRIORITY_CLASS: return "High";
    case REALTIME_PRIORITY_CLASS: return "Real Time";
    default: return "Unknown";
    }
}


void MainWindow::on_pushButton_clicked()
{
    int index = ui->spinBox_2->value();
    if (index < 0 || index >= ui->spinBox->value()) {
        qDebug() << "Invalid process index!";
        return;
    }

    auto start = std::chrono::steady_clock::now();

    // Запуск усіх процесів
    for (int i = 0; i < ui->spinBox->value(); i++) {
        ResumeThread(pi[i].hThread);
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem("Running")); // Змінюємо статус на "Running"
    }

    // Чекаємо завершення обраного процесу
    WaitForSingleObject(pi[index].hProcess, INFINITE);
    auto end = std::chrono::steady_clock::now();
    auto elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    ui->label_6->setText("Elapsed time in microseconds: " + QString::number(elapsedMicroseconds.count()));

    // Оновлення статусу всіх процесів
    for (int i = 0; i < ui->spinBox->value(); i++) {
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem("Finished")); // Змінюємо статус на "Finished"
        CloseHandle(pi[i].hProcess); // Закриваємо дескриптори
        CloseHandle(pi[i].hThread);
    }

}



