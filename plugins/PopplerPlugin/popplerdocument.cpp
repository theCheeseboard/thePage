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
#include "popplerdocument.h"

#include "popplerpage.h"

struct PopplerDocumentPrivate {
    Poppler::Document* document;

    QHash<int, PopplerPage*> pages;
};

PopplerDocument::PopplerDocument(Poppler::Document* document) : Document() {
    d = new PopplerDocumentPrivate();
    d->document = document;
}

PopplerDocument::~PopplerDocument() {
    for (PopplerPage* page : d->pages.values()) {
        delete page;
    }

    delete d->document;
    delete d;
}


Page* PopplerDocument::page(int pageNumber) {
    PopplerPage* page = d->pages.value(pageNumber, nullptr);
    if (page == nullptr) {
        page = new PopplerPage(d->document->page(pageNumber));
        if (page == nullptr) return nullptr;
        d->pages.insert(pageNumber, page);
    }
    return page;
}

int PopplerDocument::pageCount() {
    if (d->document->isLocked()) return 0;
    return d->document->numPages();
}

QString PopplerDocument::title() {
    QString title = d->document->title();
    if (title.isEmpty()) {
        title = tr("PDF Document");
    }
    return title;
}

bool PopplerDocument::requiresPassword() {
    return d->document->isLocked();
}

bool PopplerDocument::providePassword(QString password) {
    return d->document->unlock(QByteArray(), password.toUtf8());
}
