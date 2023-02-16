#include "printcontroller.h"

#include <QPainter>
#include <QPrinter>
#include <document.h>
#include <page.h>
#include <tpopover.h>
#include <tprintpopover.h>

struct PrintControllerPrivate {
        QPrinter* printer;
        QSharedPointer<Document> document;

        QWidget* parentWindow;
};

PrintController::PrintController(QSharedPointer<Document> document, QWidget* parent) :
    QObject{parent} {
    d = new PrintControllerPrivate();
    d->parentWindow = parent;
    d->document = document;

    d->printer = new QPrinter(QPrinter::HighResolution);
    d->printer->setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Inch);
    d->printer->setDocName(document->title());
}

PrintController::~PrintController() {
    delete d;
}

void PrintController::confirmAndPerformPrint() {
    tPrintPopover* jp = new tPrintPopover(d->printer);
    connect(jp, &tPrintPopover::paintRequested, this, &PrintController::paintPrinter);
    jp->setTitle(tr("Print %1").arg(QLocale().quoteString(d->document->title())));
    //    jp->setCustomPrintSettingsWidget(d->printSettings);

    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(-200);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &tPrintPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &tPrintPopover::deleteLater);
    connect(popover, &tPopover::dismissed, this, &PrintController::deleteLater);
    popover->show(d->parentWindow->window());
}

void PrintController::paintPrinter(QPrinter* printer) {
    QPainter painter(printer);

    for (auto i = 0; i < d->document->pageCount(); i++) {
        auto page = d->document->page(i);

        auto paperRect = printer->pageLayout().fullRectPixels(printer->resolution());
        auto pageSize = page->pageSize();
        auto outputSize = pageSize.scaled(paperRect.size(), Qt::KeepAspectRatio);
        auto outputRect = QRectF(QPointF(0, 0), outputSize);
        outputRect.moveCenter(paperRect.center());

        auto renderImage = QCoro::waitFor(page->render(printer->resolution() / 144.0));

        painter.drawImage(outputRect, renderImage);

        if (i != d->document->pageCount() - 1) {
            printer->newPage();
        }
    }

    painter.end();
}
