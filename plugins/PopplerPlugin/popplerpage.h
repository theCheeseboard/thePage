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
#ifndef POPPLERPAGE_H
#define POPPLERPAGE_H

#include <page.h>
#include <poppler-qt5.h>

struct PopplerPagePrivate;
class PopplerPage : public Page {
        Q_OBJECT
    public:
        explicit PopplerPage(Poppler::Page* page);
        ~PopplerPage();

    signals:

    private:
        PopplerPagePrivate* d;

        // Page interface
    public:
        QSizeF pageSize();
        tPromise<QImage>* render(double zoom);
        QList<SelectionResult> selectionMade(QRect rect);
        QVariantMap clickAction(QPointF point);
};

#endif // POPPLERPAGE_H
