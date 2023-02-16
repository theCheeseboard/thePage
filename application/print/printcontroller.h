#ifndef PRINTCONTROLLER_H
#define PRINTCONTROLLER_H

#include <QWidget>

class QPrinter;
class Document;
struct PrintControllerPrivate;
class PrintController : public QObject {
        Q_OBJECT
    public:
        explicit PrintController(QSharedPointer<Document> document, QWidget* parent = nullptr);
        ~PrintController();

        void confirmAndPerformPrint();

    signals:

    private:
        PrintControllerPrivate* d;

        void paintPrinter(QPrinter* printer);
};

#endif // PRINTCONTROLLER_H
