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
#ifndef POPPLERDOCUMENT_H
#define POPPLERDOCUMENT_H

#include <poppler-qt5.h>
#include <document.h>

struct PopplerDocumentPrivate;
class PopplerDocument : public Document {
        Q_OBJECT
    public:
        explicit PopplerDocument(Poppler::Document* document, QUrl filename);
        ~PopplerDocument();

    signals:

    private:
        PopplerDocumentPrivate* d;

        // Document interface
    public:
        QUrl fileName();
        Page* page(int pageNumber);
        int pageCount();
        QString title();
        bool requiresPassword();
        bool providePassword(QString password);
        bool save(QIODevice* device);
};

#endif // POPPLERDOCUMENT_H
