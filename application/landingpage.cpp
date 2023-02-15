#include "landingpage.h"
#include "ui_landingpage.h"

#include <tapplication.h>
#include <tcontentsizer.h>

LandingPage::LandingPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LandingPage) {
    ui->setupUi(this);

    ui->appNameLabel->setText(tApplication::applicationDisplayName());
    ui->appIconLabel->setPixmap(tApplication::applicationIcon().pixmap(QSize(ui->appNameLabel->sizeHint().height(), ui->appNameLabel->sizeHint().height())));
    new tContentSizer(ui->containerWidget);

    ui->openFileButton->setIcon(QIcon::fromTheme("document-open"));
}

LandingPage::~LandingPage() {
    delete ui;
}

void LandingPage::on_openFileButton_clicked() {
    emit openFile();
}
