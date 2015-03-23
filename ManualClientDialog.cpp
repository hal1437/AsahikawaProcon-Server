#include "ManualClientDialog.h"
#include "ui_ManualClientDialog.h"


void ManualClientDialog::keyPressEvent(QKeyEvent * event){
    if(event->key()==Qt::Key_W)UPButtonClicked();
    if(event->key()==Qt::Key_S)DOWNButtonClicked();
    if(event->key()==Qt::Key_D)RIGHTButtonClicked();
    if(event->key()==Qt::Key_A)LEFTButtonClicked();
}

GameSystem::Method::ACTION ManualClientDialog::GetAction(){
    if     (this->ui->WalkRadio->isChecked()) return GameSystem::Method::ACTION::WALK;
    else if(this->ui->PutRadio->isChecked())  return GameSystem::Method::ACTION::PUT;
    else if(this->ui->SeachRadio->isChecked())return GameSystem::Method::ACTION::SEACH;
    else if(this->ui->LookRadio->isChecked()) return GameSystem::Method::ACTION::LOOK;
    else                                      return GameSystem::Method::ACTION::UNKNOWN;
}

void ManualClientDialog::UPButtonClicked(){
    next_method.rote = GameSystem::Method::ROTE::UP;
    next_method.action = GetAction();
    emit ReadyAction();
}
void ManualClientDialog::DOWNButtonClicked(){
    next_method.rote = GameSystem::Method::ROTE::DOWN;
    next_method.action = GetAction();
    emit ReadyAction();
}
void ManualClientDialog::RIGHTButtonClicked(){
    next_method.rote = GameSystem::Method::ROTE::RIGHT;
    next_method.action = GetAction();
    emit ReadyAction();
}
void ManualClientDialog::LEFTButtonClicked(){
    next_method.rote = GameSystem::Method::ROTE::LEFT;
    next_method.action = GetAction();
    emit ReadyAction();
}
void ManualClientDialog::AppendLog(const QString& str){
    this->ui->LogEdit->appendPlainText(str);
}

ManualClientDialog::ManualClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualClientDialog)
{
    ui->setupUi(this);
    connect(this->ui->UPButton   ,SIGNAL(clicked()),this,SLOT(UPButtonClicked()));
    connect(this->ui->DOWNButton ,SIGNAL(clicked()),this,SLOT(DOWNButtonClicked()));
    connect(this->ui->RIGHTButton,SIGNAL(clicked()),this,SLOT(RIGHTButtonClicked()));
    connect(this->ui->LEFTButton ,SIGNAL(clicked()),this,SLOT(LEFTButtonClicked()));
}

ManualClientDialog::~ManualClientDialog()
{
    hide();
    delete ui;
}
