#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_clicked()
{
   // ui->textBrowser->clear();
    file= QFileDialog::getOpenFileName(this);

    if(file.length()<=2){
        QMessageBox *fail=new QMessageBox;
        fail->setWindowTitle("提示");
        fail->setText("请选择文件！");
        fail->exec();
    }

    else if(!file.endsWith(".txt")){
        QMessageBox *fail=new QMessageBox;
        fail->setWindowTitle("提示");
        fail->setText("请打开txt文件！");
        fail->exec();
    }

    else{
        QFile file(this->file);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
             qDebug()<<"Can't open the file!"<<endl;
         }
         while(!file.atEnd()) {
             QByteArray line = file.readLine();
             QString str(line);
             str.remove("\n");
             ui->edit->append(str);
         }
    }
}

void MainWindow::on_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Open Config"),
            "",
            tr("保存类型(*.txt)"));

        if (!fileName.isNull())
        {
            //fileName是文件名
            qDebug()<<fileName;
            QFile file(fileName);

            file.open(QFile::WriteOnly);

            QTextStream writefiletext(&file);

            QString str=ui->edit->toPlainText();

            writefiletext<<str;

            file.close();
        }
        else
        {
            showMessage("保存取消");
        }
}

void MainWindow::showMessage(QString text){
    QMessageBox* mbox=new QMessageBox();
    mbox->setText(text);
    mbox->setWindowTitle("提示");
    mbox->show();
}

void MainWindow::on_error_clicked()
{
   clear();
   errorAnalysis();


//   QProcess *myProcess = new QProcess();
//   myProcess->start("protest.exe");
}

void MainWindow::on_compile_clicked()
{
    clear();
    errorAnalysis();

    qDebug()<<ui->state->toPlainText();
    if(!(ui->state->toPlainText().contains("success"))){
        showMessage("有错误，请检查！");
        return;
    }
    //词法分析
    QFile file("./work/word.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qDebug()<<"Can't open the file!"<<endl;
     }
     while(!file.atEnd()) {
         QByteArray line = file.readLine();
         QString str(line);
         str.remove("\n");
         ui->cifa->append(str);
     }
     file.close();

     //四元式
     QFile file2("./work/four.txt");
     if(!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
          qDebug()<<"Can't open the file!"<<endl;
      }
      while(!file2.atEnd()) {
          QByteArray line = file2.readLine();
          QString str(line);
          str.remove("\n");
          ui->siyuanshi->append(str);
      }
      file2.close();

      //汇编
       QFile file3("./work/huibian.txt");
      if(!file3.open(QIODevice::ReadOnly | QIODevice::Text)) {
           qDebug()<<"Can't open the file!"<<endl;
       }
       while(!file3.atEnd()) {
           QByteArray line = file3.readLine();
           QString str(line);
           str.remove("\n");
           ui->huibian->append(str);
       }
       file3.close();


}

 void MainWindow::clear(){
     ui->cifa->clear();
     ui->state->clear();
     ui->siyuanshi->clear();
     ui->huibian->clear();
 }

 void MainWindow::errorAnalysis(){
     QString fileName ="./work/input.txt";
     if (!fileName.isNull())
     {
         //fileName是文件名
         qDebug()<<fileName;
         QFile file(fileName);
         file.open(QFile::WriteOnly);
         QTextStream writefiletext(&file);
         QString str=ui->edit->toPlainText();
         writefiletext<<str;
         file.close();
     }
     else
     {
         showMessage("保存错误");
     }

     QProcess::execute("protest.exe");

    qDebug()<<"fuck";
     QFile file("./work/error.txt");
     if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          qDebug()<<"Can't open the file!"<<endl;
      }
      while(!file.atEnd()) {

          QTextCodec *tc = QTextCodec::codecForName("GBK");
          QByteArray ba = file.readLine();
          QString str = tc->toUnicode(ba);
          str.remove("\n");
          ui->state->append(str);

//          QByteArray line = file.readLine();
//          QString str(line);
//          str.remove("\n");
//          ui->state->append(str);
      }

      file.close();
 }


 void MainWindow::sleep(unsigned int msec){
     QTime reachTime =QTime::currentTime().addMSecs(msec);
     while(QTime::currentTime()<reachTime){
         QCoreApplication::processEvents(QEventLoop::AllEvents,100);
     }
 }
