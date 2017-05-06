#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>
#include <QTime>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showMessage(QString text);
    void clear();
    void errorAnalysis();
    void sleep(unsigned int msec);

    QString file;

private slots:
    void on_open_clicked();

    void on_save_clicked();

    void on_error_clicked();

    void on_compile_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
