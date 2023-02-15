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
#ifndef PAGE_H
#define PAGE_H

#include <QCoroTask>
#include <QImage>
#include <QObject>
#include <texception.h>

class Page : public QObject {
        Q_OBJECT
    public:
        explicit Page();

        struct SelectionResult {
                QString text;
                QImage image;
        };

        virtual QSizeF pageSize() = 0;
        virtual QCoro::Task<QImage> render(double zoom = 1) = 0;
        virtual QList<SelectionResult> selectionMade(QRect rect) = 0;
        virtual QVariantMap clickAction(QPointF point) = 0;

    signals:
};

class LIBCONTEMPORARY_EXPORT ConcurrentRenderException : public tException {
        T_EXCEPTION(ConcurrentRenderException)
};

#endif // PAGE_H
