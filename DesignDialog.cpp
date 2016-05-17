#include "DesignDialog.h"
#include "ui_DesignDialog.h"

DesignDialog::DesignDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DesignDialog)
{
    ui->setupUi(this);
}

DesignDialog::~DesignDialog()
{
    delete ui;
}
