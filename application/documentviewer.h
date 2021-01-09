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
#ifndef DOCUMENTVIEWER_H
#define DOCUMENTVIEWER_H

#include <QWidget>
#include <document.h>

namespace Ui {
    class DocumentViewer;
}

struct DocumentViewerPrivate;
class DocumentViewer : public QWidget {
        Q_OBJECT

    public:
        explicit DocumentViewer(QWidget* parent = nullptr);
        ~DocumentViewer();

        enum DocumentMode {
            Scrolling,
            Selection
        };

        void openFile(QUrl file);
        void showOpenFileDialog();
        bool isDocumentOpen();

        double zoom();
        DocumentMode documentMode();

        Document* document();
        void setBypassDRMLimitation(Document::DRMLimitation type);
        bool isDRMBypassed(Document::DRMLimitation type);

        QString title();

    private slots:
        void on_openButton_clicked();

        void on_pageSpin_valueChanged(int arg1);

        void on_zoomBox_valueChanged(double arg1);

        void on_unlockButton_clicked();

        void updateCurrentPageNumber();

        void on_prevPageButton_clicked();

        void on_nextPageButton_clicked();

        void on_scrollingButton_toggled(bool checked);

        void on_selectionButton_toggled(bool checked);

    signals:
        void titleChanged(QString title);
        void zoomChanged(double zoom);
        void documentModeChanged(DocumentMode mode);

    private:
        Ui::DocumentViewer* ui;
        DocumentViewerPrivate* d;

        void updateCurrentView();
        void updateMouseMode();
};

#endif // DOCUMENTVIEWER_H
