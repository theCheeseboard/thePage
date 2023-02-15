#ifndef LANDINGPAGE_H
#define LANDINGPAGE_H

#include <QWidget>

namespace Ui {
    class LandingPage;
}

class LandingPage : public QWidget {
        Q_OBJECT

    public:
        explicit LandingPage(QWidget* parent = nullptr);
        ~LandingPage();

    private slots:
        void on_openFileButton_clicked();

    signals:
        void openFile();

    private:
        Ui::LandingPage* ui;
};

#endif // LANDINGPAGE_H
