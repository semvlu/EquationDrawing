#ifndef VIEWER_H
#define VIEWER_H

#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
//#include <QSignalMapper>
#include "Parser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Viewer; }
QT_END_NAMESPACE

class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    ~Viewer();
    void setXY();

private slots:
    void addFunction();
    //void checkFunction();
    void deleteFunction(int);


private:
    Ui::Viewer *ui;
    int scale;

    std::vector<Parser> parser;
    std::vector<QVector<double>> x, y;
    std::vector<QLineEdit*> le;
    std::vector<QPushButton*> fColor, fVisible, fDelete;
    //QSignalMapper* myMapper, *leMapper;

    const QSize btnSize = QSize(25, 25);
};
#endif // VIEWER_H
