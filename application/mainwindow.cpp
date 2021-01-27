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

#include <tapplication.h>
#include <thelpmenu.h>
#include <tjobmanager.h>
#include <tcsdtools.h>
#include "pluginmanager.h"
#include <tlogger.h>
#include <QPushButton>
#include <QFileDialog>

#include "documentviewer.h"

struct MainWindowPrivate {
    tCsdTools csd;
    PluginManager plugins;

    QMap<DocumentViewer*, QPushButton*> tabButtons;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
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
    menu->addAction(ui->actionNew_Tab);
    menu->addAction(ui->actionOpen);
    menu->addAction(ui->actionClose_Tab);
    menu->addSeparator();
    menu->addMenu(new tHelpMenu(this));
    menu->addAction(ui->actionExit);

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);

    d->plugins.load();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    newTab();

//    Document* doc = DocumentProviderManager::instance()->documentFor("/home/victor/print.pdf");
//    tDebug("MainWindow") << "Document has " << doc->pageCount() << " pages";
//    ui->label->setPixmap(QPixmap::fromImage(doc->page(0)->render(2)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionExit_triggered() {
    tApplication::exit();
}

void MainWindow::on_actionNew_Tab_triggered() {
    newTab();
}

DocumentViewer* MainWindow::newTab() {
    QPushButton* button = new QPushButton();
    DocumentViewer* viewer = new DocumentViewer();
    d->tabButtons.insert(viewer, button);

    connect(viewer, &DocumentViewer::titleChanged, this, [ = ](QString title) {
        button->setText(title);
        if (button->isChecked()) this->setWindowTitle(QStringLiteral("%1 - thePage").arg(title));
    });
    connect(button, &QPushButton::clicked, this, [ = ] {
        for (QPushButton* button : d->tabButtons.values()) {
            button->setChecked(false);
        }
        button->setChecked(true);
        ui->stackedWidget->setCurrentWidget(viewer);
        this->setWindowTitle(QStringLiteral("%1 - thePage").arg(button->text()));
    });
    button->setText(viewer->title());
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    button->setCheckable(true);

    ui->tabButtonLayout->addWidget(button);
    ui->stackedWidget->addWidget(viewer);
    ui->stackedWidget->setCurrentWidget(viewer);

    return viewer;
}

void MainWindow::on_stackedWidget_currentChanged(int arg1) {
    QWidget* w = ui->stackedWidget->widget(arg1);
    if (w) {
        d->tabButtons.value(static_cast<DocumentViewer*>(w))->click();
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

    //Open a document in a new tab
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"PDF Documents (*.pdf)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [ = ](QString file) {
        DocumentViewer* tab = newTab();
        tab->openFile(QUrl::fromLocalFile(file));
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void MainWindow::on_actionClose_Tab_triggered() {
    QWidget* w = ui->stackedWidget->currentWidget();
    if (w) {
        DocumentViewer* tab = static_cast<DocumentViewer*>(w);
        QPushButton* button = d->tabButtons.take(tab);
        ui->stackedWidget->removeWidget(tab);
        ui->tabButtonLayout->removeWidget(button);
        button->deleteLater();
        tab->deleteLater();

        if (ui->stackedWidget->count() == 0) newTab();
    }
}
