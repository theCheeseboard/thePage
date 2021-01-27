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
#include <QCache>
#include <page.h>
#include <QRubberBand>
#include <QMouseEvent>
#include <tlogger.h>
#include <QMenu>
#include <QClipboard>
#include <document.h>
#include <QMessageBox>
#include <QPushButton>
#include <QDesktopServices>

struct PageViewerPrivate {
    DocumentViewer* viewer;
    Page* page;
    double zoom = 1;

    static QCache<PageViewer*, QImage> pageImages;
    bool pageLoadRequested = false;
    bool mousePressed = false;

    DocumentViewer::DocumentMode documentMode;

    QRubberBand* selectionBand;
    QPoint selectionOrigin;
};

QCache<PageViewer*, QImage> PageViewerPrivate::pageImages(536870912); //512 MiB

PageViewer::PageViewer(DocumentViewer* viewer) :
    QWidget(viewer),
    ui(new Ui::PageViewer) {
    ui->setupUi(this);

    d = new PageViewerPrivate();
    d->viewer = viewer;
    d->selectionBand = new QRubberBand(QRubberBand::Rectangle, this);

    this->setMouseTracking(true);

    connect(viewer, &DocumentViewer::zoomChanged, this, &PageViewer::setZoom);
    connect(viewer, &DocumentViewer::documentModeChanged, this, &PageViewer::setDocumentMode);
    this->setZoom(viewer->zoom());
    this->setDocumentMode(viewer->documentMode());
}

PageViewer::~PageViewer() {
    d->pageImages.remove(this);
    delete ui;
    delete d;
}

void PageViewer::setPage(Page* page) {
    d->page = page;

    d->pageImages.remove(this);
}

void PageViewer::setZoom(double zoom) {
    d->zoom = zoom;
    updateGeometry();

//    d->pageImages.remove(this);
}

void PageViewer::setDocumentMode(DocumentViewer::DocumentMode mode) {
    d->documentMode = mode;
}

void PageViewer::updatePageImage() {
    if (d->pageLoadRequested) return;

    QPointer<QObject> contextObject(this);

    d->pageLoadRequested = true;
    d->page->render(d->zoom)->then([ = ](QImage image) {
        if (!contextObject) return;
        d->pageImages.insert(this, new QImage(image), image.sizeInBytes());
        d->pageLoadRequested = false;
        this->update();
    });
}

QSize PageViewer::sizeHint() const {
    if (!d->page) return QSize();
    return (d->page->pageSize() * d->zoom).toSize();
}

void PageViewer::mousePressEvent(QMouseEvent* event) {
    d->mousePressed = true;
    if (d->documentMode == DocumentViewer::Selection) {
        d->selectionOrigin = event->pos();
        d->selectionBand->setGeometry(QRect(d->selectionOrigin, QSize()));
        d->selectionBand->show();
    }
}

void PageViewer::mouseReleaseEvent(QMouseEvent* event) {
    d->mousePressed = false;

    QRect documentRect;
    documentRect.setSize(this->sizeHint());
    documentRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    if (d->documentMode == DocumentViewer::Selection) {
        QRect bandGeometry = d->selectionBand->geometry();
        if (bandGeometry.height() * bandGeometry.width() < SC_DPI(20)) {
            d->selectionBand->hide();
            return;
        }

        QRect adjustedRect((bandGeometry.topLeft() - documentRect.topLeft()) / d->zoom, bandGeometry.size() / d->zoom);
        QList<Page::SelectionResult> results = d->page->selectionMade(adjustedRect);

        if (results.isEmpty()) {
            d->selectionBand->hide();
            return;
        }
        QMenu* menu = new QMenu(this);
        for (const Page::SelectionResult& result : qAsConst(results)) {
            if (result.text.isEmpty()) {
                //TODO: Images
            } else {
                menu->addSection(tr("For text %1").arg(menu->fontMetrics().elidedText(QLocale().quoteString(result.text).trimmed(), Qt::ElideMiddle, SC_DPI(300))));
                menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy"), [ = ] {
                    if (d->viewer->document()->isDrmEnforced(Document::Copy) && !d->viewer->isDRMBypassed(Document::Copy)) {
                        QMessageBox* messageBox = new QMessageBox(this);
                        messageBox->setWindowTitle(tr("Digital Rights Management"));
                        messageBox->setText(tr("The author of this document prohibits you from copying text and images."));
                        messageBox->setIcon(QMessageBox::Information);
                        messageBox->addButton(QMessageBox::Ok);
                        QPushButton* copyButton = messageBox->addButton(tr("Copy Anyway"), QMessageBox::DestructiveRole);
                        connect(messageBox, &QMessageBox::buttonClicked, this, [ = ](QAbstractButton * button) {
                            if (button == copyButton) {
                                d->viewer->setBypassDRMLimitation(Document::Copy);
                                QApplication::clipboard()->setText(result.text);
                            }
                        });
                        connect(messageBox, &QMessageBox::finished, messageBox, &QMessageBox::deleteLater);
                        messageBox->open();
                        return;
                    }

                    QApplication::clipboard()->setText(result.text);
                });
            }
        }

        connect(menu, &QMenu::aboutToHide, this, [ = ] {
            menu->deleteLater();
            d->selectionBand->hide();
        });
        menu->popup(event->globalPos());
    }

    //See if there is a click action
    QVariantMap action = d->page->clickAction((event->pos() - documentRect.topLeft()) / d->zoom);
    if (action.value("type").toString() == "link") {
        QString linkType = action.value("linkType").toString();
        if (linkType == "viewport") {
            //Go to a different page
            double offsetTop = action.value("offsetTop", -1).toDouble();
            double offsetLeft = action.value("offfsetLeft", -1).toDouble();
            emit navigate(action.value("page").toInt(), offsetTop, offsetLeft);
        } else if (linkType == "url") {
            QUrl url = action.value("url").toUrl();

            this->setCursor(QCursor(Qt::ArrowCursor));

            QMessageBox* messageBox = new QMessageBox(this);
            messageBox->setWindowTitle(tr("Visit Link"));
            messageBox->setText(tr("Visit the link %1?").arg(url.toString()));
            messageBox->setInformativeText(tr("Make sure it's a place you trust; the web can be scary!"));
            messageBox->setIcon(QMessageBox::Information);
            messageBox->addButton(QMessageBox::Cancel);
            QPushButton* acceptButton = messageBox->addButton(tr("Visit"), QMessageBox::AcceptRole);
            connect(messageBox, &QMessageBox::buttonClicked, this, [ = ](QAbstractButton * button) {
                if (button == acceptButton) {
                    QDesktopServices::openUrl(url);
                }
            });
            connect(messageBox, &QMessageBox::finished, messageBox, &QMessageBox::deleteLater);
            messageBox->open();
        }
    }
//    tDebug("PageViewer") << action.value("type").toString();
}

void PageViewer::mouseMoveEvent(QMouseEvent* event) {
    if (d->mousePressed) {
        if (d->documentMode == DocumentViewer::Selection) {
            d->selectionBand->setGeometry(QRect(d->selectionOrigin, event->pos()).normalized());
        }
    } else {
        QRect documentRect;
        documentRect.setSize(this->sizeHint());
        documentRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

        //See if there is a click action
        QVariantMap action = d->page->clickAction((event->pos() - documentRect.topLeft()) / d->zoom);
//        tDebug("PageViewer") << action.value("type").toString();
        if (action.value("type").toString() == "link") {
            this->setCursor(QCursor(Qt::PointingHandCursor));
        } else {
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void PageViewer::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() == Qt::ControlModifier) {
        QPoint pixels = event->pixelDelta();
        QPoint angle = event->angleDelta() / 8;

        if (!pixels.isNull()) {
            emit changeZoom(d->zoom * pow(1.005, event->pixelDelta().y()), event->position().toPoint());
        } else if (!angle.isNull()) {
            emit changeZoom(d->zoom * pow(1.005, event->angleDelta().y() / 8), event->position().toPoint());
        }

        event->accept();
    }
}

void PageViewer::paintEvent(QPaintEvent* event) {
    if (!d->page) return;

    QPainter painter(this);

    bool shouldUpdateImage = false;

    QImage* image = d->pageImages.object(this);
    QRect rect;
    rect.setSize(this->sizeHint());
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    if (image) {
        if (rect.size() != image->size()) shouldUpdateImage = true;
        painter.drawImage(rect, image->scaled(rect.size()));
    } else {
        shouldUpdateImage = true;
        painter.fillRect(rect, Qt::white);
    }

    if (shouldUpdateImage) updatePageImage();
}
