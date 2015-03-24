#include "MapEditerDialog.h"
#include "ui_MapEditerDialog.h"

MapEditerDialog::MapEditerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapEditerDialog)
{
    ui->setupUi(this);
}

MapEditerDialog::~MapEditerDialog()
{
    delete ui;
}
