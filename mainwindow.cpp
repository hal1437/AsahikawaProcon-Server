#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::keyPressEvent(QKeyEvent * event){
    //縦に比率を合わせる
    if(event->key()==Qt::Key_F){
        int left_margin=0,right_margin=0;
        this->ui->centralWidget->layout()->getContentsMargins(&left_margin,nullptr,&right_margin,nullptr);
        this->ui->Field->resize((this->ui->Field->size().height()/15.0f)*17.0f,this->ui->Field->size().height());
        this->resize(QSize(this->ui->Field->width() + left_margin + right_margin,this->size().height()));
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->startup = new StartupDialog(this);

    if(this->startup->exec()){
        this->ui->Field->field = this->startup->map;
    }else{
        exit(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
