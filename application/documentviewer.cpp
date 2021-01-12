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
#include <page.h>
#include "pageviewer.h"
#include <terrorflash.h>
#include <ttoast.h>
#include <tlogger.h>
#include <QScrollBar>
#include <QScroller>

struct DocumentViewerPrivate {
    Document* currentDocument = nullptr;
    QList<PageViewer*> viewers;
    QSet<Document::DRMLimitation> drmBypassed;

    DocumentViewer::DocumentMode documentMode = DocumentViewer::Scrolling;
    double fixateHScrollbar = -1, fixateVScrollbar = -1;
    int fixateHScrollbarOffset, fixateVScrollbarOffset;
};

DocumentViewer::DocumentViewer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DocumentViewer) {
    ui->setupUi(this);

    d = new DocumentViewerPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->pagesLayout->setSpacing(SC_DPI(9));

    ui->stackedWidget->setCurrentWidget(ui->landingPage, false);
    updateCurrentView();

    ui->scrollArea->setProperty("X-Contemporary-NoInstallScroller", true);
    ui->scrollArea->viewport()->installEventFilter(this);
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [ = ] {
        if (QScroller::scroller(ui->scrollArea)->state() == QScroller::Inactive) updateCurrentPageNumber();
    });
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this, [ = ](int min, int max) {
        if (d->fixateVScrollbar >= 0) {
            ui->scrollArea->verticalScrollBar()->setValue(max * d->fixateVScrollbar - d->fixateVScrollbarOffset);
            d->fixateVScrollbar = -1;
        }
    });
    connect(ui->scrollArea->horizontalScrollBar(), &QScrollBar::rangeChanged, this, [ = ](int min, int max) {
        if (d->fixateHScrollbar >= 0) {
            ui->scrollArea->horizontalScrollBar()->setValue(max * d->fixateHScrollbar - d->fixateHScrollbarOffset);
            d->fixateHScrollbar = -1;
        }
    });

    connect(QScroller::scroller(ui->scrollArea), &QScroller::stateChanged, this, [ = ](QScroller::State state) {
        if (state == QScroller::Inactive) updateCurrentPageNumber();
    });

    updateMouseMode();
}

DocumentViewer::~DocumentViewer() {
    delete d;
    delete ui;
}

void DocumentViewer::openFile(QUrl file) {
    for (PageViewer* viewer : qAsConst(d->viewers)) {
        ui->pagesLayout->removeWidget(viewer);
        viewer->deleteLater();
    }
    d->viewers.clear();

    d->drmBypassed.clear();

    Document* document = DocumentProviderManager::instance()->documentFor(file);
    if (!document) {
        //Error Error!
        tToast* toast = new tToast(this);
        toast->setTitle(tr("File Not Readable"));
        toast->setText(tr("Sorry, that file can't be opened."));
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(this);
        return;
    }
    d->currentDocument = document;

    updateCurrentView();
    ui->pageSpin->setValue(1);

    emit titleChanged(title());
}

void DocumentViewer::showOpenFileDialog() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"PDF Documents (*.pdf)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [ = ](QString file) {
        openFile(QUrl::fromLocalFile(file));
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

bool DocumentViewer::isDocumentOpen() {
    return ui->stackedWidget->currentWidget() != ui->landingPage;
}

double DocumentViewer::zoom() {
    return ui->zoomBox->value() / 100;
}

DocumentViewer::DocumentMode DocumentViewer::documentMode() {
    return d->documentMode;
}

Document* DocumentViewer::document() {
    return d->currentDocument;
}

void DocumentViewer::setBypassDRMLimitation(Document::DRMLimitation type) {
    d->drmBypassed.insert(type);
}

bool DocumentViewer::isDRMBypassed(Document::DRMLimitation type) {
    return d->drmBypassed.contains(type);
}

QString DocumentViewer::title() {
    if (d->currentDocument) {
        return d->currentDocument->title();
    } else {
        return tr("New Tab");
    }
}

void DocumentViewer::on_openButton_clicked() {
    showOpenFileDialog();
}

void DocumentViewer::on_pageSpin_valueChanged(int arg1) {
    QScroller::scroller(ui->scrollArea)->scrollTo(QPointF(0, d->viewers.at(arg1 - 1)->geometry().top()), 500);
    ui->renderArea->update();
}

void DocumentViewer::on_zoomBox_valueChanged(double arg1) {
    d->fixateHScrollbar = static_cast<double>(ui->scrollArea->horizontalScrollBar()->value()) / ui->scrollArea->horizontalScrollBar()->maximum();
    d->fixateVScrollbar = static_cast<double>(ui->scrollArea->verticalScrollBar()->value()) / ui->scrollArea->verticalScrollBar()->maximum();
    d->fixateHScrollbarOffset = d->fixateVScrollbarOffset = 0;
    emit zoomChanged(arg1 / 100);
}

void DocumentViewer::on_unlockButton_clicked() {
    bool success = d->currentDocument->providePassword(ui->passwordBox->text());
    if (!success || d->currentDocument->requiresPassword()) {
        tErrorFlash::flashError(ui->passwordContainer);
    }

    updateCurrentView();
}

void DocumentViewer::updateCurrentPageNumber() {
    if (d->viewers.isEmpty()) return;
    int pageNumber = 0;
    for (PageViewer* viewer : qAsConst(d->viewers)) {
        if (viewer->geometry().top() > ui->scrollArea->verticalScrollBar()->value() + ui->scrollArea->height() / 2) break;
        pageNumber++;
    }

    QSignalBlocker blocker(ui->pageSpin);
    ui->pageSpin->setValue(pageNumber);
}

void DocumentViewer::updateCurrentView() {
    if (!d->currentDocument) {
        ui->stackedWidget->setCurrentWidget(ui->landingPage);
    } else if (d->currentDocument->requiresPassword()) {
        ui->stackedWidget->setCurrentWidget(ui->passwordPage);
    } else {
        for (PageViewer* viewer : qAsConst(d->viewers)) {
            ui->pagesLayout->removeWidget(viewer);
            viewer->deleteLater();
        }
        d->viewers.clear();

        for (int i = 0; i < d->currentDocument->pageCount(); i++) {
            PageViewer* viewer = new PageViewer(this);
            viewer->setPage(d->currentDocument->page(i));
            viewer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            connect(viewer, &PageViewer::navigate, this, [ = ](int page, double offsetTop, double offsetLeft) {
                QPointF scrollPoint(d->viewers.at(page)->geometry().left(), d->viewers.at(page)->geometry().top());
                if (offsetTop > 0) scrollPoint.ry() += d->viewers.at(page)->geometry().height() * offsetTop;
                if (offsetLeft > 0) scrollPoint.rx() += d->viewers.at(page)->geometry().width() * offsetLeft;
                QScroller::scroller(ui->scrollArea)->scrollTo(scrollPoint, 500);
            });
            connect(viewer, &PageViewer::changeZoom, this, [ = ](double zoom, QPoint fixationPoint) {
                QPoint viewerPoint = ui->scrollAreaWidgetContents->mapFromGlobal(viewer->mapToGlobal(fixationPoint));
                QPoint scrollerPoint = ui->scrollArea->mapFromGlobal(viewer->mapToGlobal(fixationPoint));
                d->fixateHScrollbar = static_cast<double>(viewerPoint.x()) / ui->scrollArea->horizontalScrollBar()->maximum();
                d->fixateVScrollbar = static_cast<double>(viewerPoint.y()) / ui->scrollArea->verticalScrollBar()->maximum();
                d->fixateHScrollbarOffset = scrollerPoint.x();
                d->fixateVScrollbarOffset = scrollerPoint.y();
                emit zoomChanged(zoom);

                QSignalBlocker blocker(ui->zoomBox);
                ui->zoomBox->setValue(zoom * 100);
            });
            ui->pagesLayout->addWidget(viewer);
            d->viewers.append(viewer);
        }

        ui->pageSpin->setMaximum(d->currentDocument->pageCount());
        ui->pageCountBox->setText(QStringLiteral("/ %1").arg(d->currentDocument->pageCount()));
        ui->stackedWidget->setCurrentWidget(ui->documentPage);
    }
}

void DocumentViewer::updateMouseMode() {
    QScroller::ungrabGesture(ui->scrollArea->viewport());
    if (ui->scrollingButton->isChecked()) {
        QScroller::grabGesture(ui->scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
        d->documentMode = Scrolling;
    } else if (ui->selectionButton->isChecked()) {
        d->documentMode = Selection;
    }

    emit documentModeChanged(d->documentMode);
}

void DocumentViewer::on_prevPageButton_clicked() {
    ui->pageSpin->setValue(ui->pageSpin->value() - 1);
}

void DocumentViewer::on_nextPageButton_clicked() {
    ui->pageSpin->setValue(ui->pageSpin->value() + 1);
}

void DocumentViewer::on_scrollingButton_toggled(bool checked) {
    if (checked) updateMouseMode();
}

void DocumentViewer::on_selectionButton_toggled(bool checked) {
    if (checked) updateMouseMode();
}
