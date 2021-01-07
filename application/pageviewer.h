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
#ifndef PAGEVIEWER_H
#define PAGEVIEWER_H

#include <QWidget>

namespace Ui {
    class PageViewer;
}

struct PageViewerPrivate;
class Page;
class PageViewer : public QWidget {
        Q_OBJECT

    public:
        explicit PageViewer(QWidget* parent = nullptr);
        ~PageViewer();

        void setPage(Page* page);
        void setZoom(double zoom);

    private:
        Ui::PageViewer* ui;
        PageViewerPrivate* d;

        void updatePageImage();

        // QWidget interface
    public:
        QSize sizeHint() const;

    protected:
        void paintEvent(QPaintEvent* event);
};

#endif // PAGEVIEWER_H
