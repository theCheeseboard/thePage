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
#include "popplerpage.h"

#include <QImage>

struct PopplerPagePrivate {
    uint renders = 0;
    Poppler::Page* page;
};

PopplerPage::PopplerPage(Poppler::Page* page) : Page() {
    d = new PopplerPagePrivate();
    d->page = page;


}

PopplerPage::~PopplerPage() {
    delete d->page;
    delete d;
}


QSizeF PopplerPage::pageSize() {
    return d->page->pageSizeF();
}

tPromise<QImage>* PopplerPage::render(double zoom) {
    uint currentRender = ++d->renders;

    return TPROMISE_CREATE_NEW_THREAD(QImage, {
        if (d->renders != currentRender) {
            rej("Started another render");
            return;
        }

        //Render at more DPI then we need and then scale down to improve picture quality
        QImage image = d->page->renderToImage(144.0 * zoom, 144.0 * zoom, -1, -1, -1, -1, Poppler::Page::Rotate0, [](const QImage&, const QVariant&) {

        }, [](const QVariant&) -> bool {
            return false;
        }, [](const QVariant & metaVariant) -> bool {
            QVariantMap meta = metaVariant.toMap();
            if (meta.value("currentRender").toUInt() != meta.value("this").value<PopplerPage*>()->d->renders) {
                return true;
            }
            return false;
        }, QVariant::fromValue(QVariantMap({
            {"this", QVariant::fromValue(this)},
            {"currentRender", currentRender}
        })));

        if (d->renders != currentRender) {
            rej("Started another render");
            return;
        }

        QSize newSize = image.size() / 2;
        image = image.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        res(image);
    });
}

QList<PopplerPage::SelectionResult> PopplerPage::selectionMade(QRect rect) {
    QList<SelectionResult> results;

    SelectionResult textResult;
    textResult.text = d->page->text(rect);
    if (!textResult.text.isEmpty()) results.append(textResult);

    return results;
}
