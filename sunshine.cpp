#include "sunshine.h"
#include "ui_sunshine.h"

Sunshine::Sunshine(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sunshine)
{
    ui->setupUi(this);
}

Sunshine::~Sunshine()
{
    delete ui;
}

void Sunshine::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
