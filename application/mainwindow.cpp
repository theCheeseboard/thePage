/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>
#include <tapplication.h>
#include <tcsdtools.h>
#include <thelpmenu.h>
#include <tjobmanager.h>
#include <tlogger.h>
#include <twindowtabberbutton.h>

#include "documentviewer.h"
#include "landingpage.h"

struct MainWindowPrivate {
        tCsdTools csd;

        QMap<DocumentViewer*, tWindowTabberButton*> tabButtons;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->jobButtonLayout->addWidget(tJobManager::makeJobButton());

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    QMenu* menu = new QMenu(this);
    menu->addAction(ui->actionOpen);
    menu->addAction(ui->actionClose_Tab);
    menu->addSeparator();
    menu->addMenu(new tHelpMenu(this));
    menu->addAction(ui->actionExit);

    this->setWindowIcon(tApplication::applicationIcon());
    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setIconSize(QSize(24, 24));
    ui->menuButton->setMenu(menu);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    auto landingPage = new LandingPage();
    ui->stackedWidget->setDefaultWidget(landingPage);
    connect(landingPage, &LandingPage::openFile, this, &MainWindow::on_actionOpen_triggered);

    //    Document* doc = DocumentProviderManager::instance()->documentFor("/home/victor/print.pdf");
    //    tDebug("MainWindow") << "Document has " << doc->pageCount() << " pages";
    //    ui->label->setPixmap(QPixmap::fromImage(doc->page(0)->render(2)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::newTab(QUrl file) {
    DocumentViewer* viewer = newTab();
    viewer->openFile(file);
}

void MainWindow::show() {
    QMainWindow::show();
}

void MainWindow::on_actionExit_triggered() {
    tApplication::exit();
}

DocumentViewer* MainWindow::newTab() {
    auto button = new tWindowTabberButton();
    DocumentViewer* viewer = new DocumentViewer();
    d->tabButtons.insert(viewer, button);

    connect(viewer, &DocumentViewer::titleChanged, this, [this, button](QString title) {
        button->setText(title);
        if (button->isSelected()) this->setWindowTitle(QStringLiteral("%1 - thePage").arg(title));
    });
    button->setText(viewer->title());

    ui->windowTabber->addButton(button);
    ui->stackedWidget->addWidget(viewer);

    button->syncWithStackedWidget(ui->stackedWidget, viewer);

    return viewer;
}

void MainWindow::on_stackedWidget_currentChanged(int arg1) {
    QWidget* w = ui->stackedWidget->widget(arg1);
    if (w) {
        this->setWindowTitle(QStringLiteral("%1 - thePage").arg(static_cast<DocumentViewer*>(w)->title()));
    } else {
        this->setWindowTitle(tr("thePage"));
    }
}

void MainWindow::on_actionOpen_triggered() {
    QWidget* w = ui->stackedWidget->currentWidget();
    if (w) {
        DocumentViewer* viewer = static_cast<DocumentViewer*>(w);
        if (!viewer->isDocumentOpen()) {
            viewer->showOpenFileDialog();
            return;
        }
    }

    // Open a document in a new tab
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"PDF Documents (*.pdf)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [this](QString file) {
        DocumentViewer* tab = newTab();
        tab->openFile(QUrl::fromLocalFile(file));
        ui->stackedWidget->setCurrentWidget(tab);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void MainWindow::on_actionClose_Tab_triggered() {
    QWidget* w = ui->stackedWidget->currentWidget();
    if (w) {
        DocumentViewer* tab = static_cast<DocumentViewer*>(w);
        ui->stackedWidget->removeWidget(tab);
        ui->windowTabber->removeButton(d->tabButtons.value(tab));
        d->tabButtons.remove(tab);
        tab->deleteLater();
    }
}
