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
#include "pageviewer.h"
#include "ui_pageviewer.h"

#include <QPainter>
#include <page.h>

struct PageViewerPrivate {
    Page* page;
    double zoom = 1;

    QImage pageImage;
};

PageViewer::PageViewer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PageViewer) {
    ui->setupUi(this);

    d = new PageViewerPrivate();
}

PageViewer::~PageViewer() {
    delete ui;
    delete d;
}

void PageViewer::setPage(Page* page) {
    d->page = page;
    updatePageImage();
}

void PageViewer::setZoom(double zoom) {
    d->zoom = zoom;
    updatePageImage();
}

void PageViewer::updatePageImage() {
    d->page->render(d->zoom)->then([ = ](QImage image) {
        d->pageImage = image;
        updateGeometry();
    });
}

QSize PageViewer::sizeHint() const {
    if (!d->page) return QSize();
    return d->pageImage.size();
}

void PageViewer::paintEvent(QPaintEvent* event) {
    if (!d->page) return;

    QPainter painter(this);

    QRect rect;
    rect.setSize(d->pageImage.size());
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));
    painter.drawImage(rect, d->pageImage);
}
