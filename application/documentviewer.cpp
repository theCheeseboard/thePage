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
#include "documentviewer.h"
#include "ui_documentviewer.h"

#include <QFileDialog>
#include <documentprovidermanager.h>
#include <document.h>
#include <page.h>
#include "pageviewer.h"
#include <terrorflash.h>

struct DocumentViewerPrivate {
    Document* currentDocument = nullptr;
    QList<PageViewer*> viewers;
};

DocumentViewer::DocumentViewer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DocumentViewer) {
    ui->setupUi(this);

    d = new DocumentViewerPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    updateCurrentView();
}

DocumentViewer::~DocumentViewer() {
    delete d;
    delete ui;
}

void DocumentViewer::openFile(QString file) {
    for (PageViewer* viewer : d->viewers) {
        ui->pagesLayout->removeWidget(viewer);
        viewer->deleteLater();
    }

    Document* document = DocumentProviderManager::instance()->documentFor(file);
    if (!document) {
        //Error Error!
        return;
    }
    d->currentDocument = document;

    updateCurrentView();
    ui->pageSpin->setValue(1);

    emit titleChanged(title());
}

QString DocumentViewer::title() {
    if (d->currentDocument) {
        return d->currentDocument->title();
    } else {
        return tr("New Tab");
    }
}

void DocumentViewer::on_openButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"PDF Documents (*.pdf)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [ = ](QString file) {
        openFile(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void DocumentViewer::on_pageSpin_valueChanged(int arg1) {
    ui->renderArea->update();
}

void DocumentViewer::on_zoomBox_valueChanged(double arg1) {
    for (PageViewer* viewer : qAsConst(d->viewers)) {
        viewer->setZoom(arg1 / 100);
    }
}

void DocumentViewer::on_unlockButton_clicked() {
    bool success = d->currentDocument->providePassword(ui->passwordBox->text());
    if (!success || d->currentDocument->requiresPassword()) {
        tErrorFlash::flashError(ui->passwordContainer);
    }

    updateCurrentView();
}

void DocumentViewer::updateCurrentView() {
    if (!d->currentDocument) {
        ui->stackedWidget->setCurrentWidget(ui->landingPage);
    } else if (d->currentDocument->requiresPassword()) {
        ui->stackedWidget->setCurrentWidget(ui->passwordPage);
    } else {
        for (PageViewer* viewer : d->viewers) {
            ui->pagesLayout->removeWidget(viewer);
            viewer->deleteLater();
        }

        for (int i = 0; i < d->currentDocument->pageCount(); i++) {
            PageViewer* viewer = new PageViewer(this);
            viewer->setPage(d->currentDocument->page(i));
            viewer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            viewer->setZoom(ui->zoomBox->value() / 100);
            ui->pagesLayout->addWidget(viewer);
            d->viewers.append(viewer);
        }

        ui->pageSpin->setMaximum(d->currentDocument->pageCount());
        ui->stackedWidget->setCurrentWidget(ui->documentPage);
    }
}
