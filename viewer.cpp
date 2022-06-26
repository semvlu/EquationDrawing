#include "viewer.h"
#include "ui_viewer.h"
#include <vector>
#include <cstdlib>//rand
Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Viewer)
{
    
    scale = 20;
    ui->setupUi(this);

    QPen blackPen;
    blackPen.setColor(QColor(Qt::black));
    blackPen.setWidthF(4);
    //----------------------------------------------------------
    //          set QCustomPlot
    //----------------------------------------------------------
    
    // Add QCP Layer for x y data
    ui->qcp->addLayer("base", ui->qcp->layer("grid"), QCustomPlot::limAbove);
    ui->qcp->layer("base")->setMode(QCPLayer::lmBuffered);
    ui->qcp->setCurrentLayer("base");

    ui->qcp->addGraph();
    //ui->qcp->graph(0)->setPen(QPen(Qt::black)); // line color blue for first graph
    //ui->qcp->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    ui->qcp->graph(0)->setPen(blackPen);

    QVector<double> x(2*scale), y(2*scale);
    //x軸
    for (int i=0; i<2*scale; ++i)
    {
      x[i] = i-scale;
      y[i] = 0;
    }
    // pass data points to graphs:
    ui->qcp->graph(0)->setData(x, y);
    ui->qcp->rescaleAxes();

    ui->qcp->addGraph();
    ui->qcp->graph(1)->setPen(blackPen);

    //y軸
    for (int i=0; i<2*scale; ++i)
    {
      x[i] = 0;
      y[i] = i-scale;
    }
    ui->qcp->graph(1)->setData(x, y);
    ui->qcp->rescaleAxes();

    ui->qcp->xAxis2->setVisible(true);
    ui->qcp->xAxis2->setTickLabels(false);
    ui->qcp->yAxis2->setVisible(true);
    ui->qcp->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->qcp->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->qcp->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp->replot();
    
    //----------------------------------------------------------
    //          set functions layout
    //----------------------------------------------------------
    
    // Add QCP Layer for x y data
    ui->qcp->addLayer("top", ui->qcp->layer("grid"), QCustomPlot::limAbove);
    ui->qcp->layer("top")->setMode(QCPLayer::lmBuffered);
    ui->qcp->setCurrentLayer("top");

    connect(ui->createFunction, &QPushButton::clicked, this, [=] {
        addFunction();
        });
    ui->gridLayout->setSizeConstraint(QLayout::SetFixedSize);

    //connect(myMapper, SIGNAL(mapped(int)), this, SLOT(deleteFunction(int)));
    addFunction();
    
}

Viewer::~Viewer()
{
    delete ui;
}

void Viewer::addFunction()
{
    int i = le.size();
    ui->qcp->addGraph();
    qDebug() << ui->qcp->graphCount();
    //qDebug() << "adding i = " << i;
    QLineEdit* lineEdit = new QLineEdit("");
    QPushButton* colorBtn = new QPushButton("c");
    QPushButton* visibleBtn = new QPushButton("v");
    visibleBtn->setCheckable(TRUE);
    visibleBtn->setChecked(TRUE);
    QPushButton* deleteBtn = new QPushButton("d");

    colorBtn->setFixedSize(btnSize);
    visibleBtn->setFixedSize(btnSize);
    deleteBtn->setFixedSize(btnSize);

    // for later access, keep a reference in the vector:
    le.push_back(lineEdit);
    fColor.push_back(colorBtn);
    fVisible.push_back(visibleBtn);
    fDelete.push_back(deleteBtn);

    //connect(deleteBtn[i], &QPushButton::clicked, this, [=] {
        //deleteFunction();
        //});
    //connect(fDelete[i], SIGNAL(clicked()), myMapper, SLOT(map()));
    //myMapper->setMapping(fDelete[i], i);

    srand(time(NULL));

    ui->gridLayout->addWidget(colorBtn, i, 0);
    ui->gridLayout->addWidget(lineEdit, i, 1);
    ui->gridLayout->addWidget(visibleBtn, i, 2);
    ui->gridLayout->addWidget(deleteBtn, i, 3);
    //qDebug() << "nowSize: " << le.size();
    //for (int j = 0; j < le.size(); j++)
        //qDebug() << "[" << j << "] =" << le[j]->text();
    

    connect(fDelete[i], &QPushButton::clicked, this, [=] {
        deleteFunction(i);
        });
    connect(le[i], &QLineEdit::returnPressed, this, [=] {
        qDebug() << i << le[i]->text() ;
        //std::cout << le[i]->text().toStdString();

        ui->qcp->graph(i + 2)->data()->clear();
        Parser temp;
        temp.test(le[i]->text());
        //if (temp.x.size() > 0) std::cout << temp.x[0] << " " << temp.y[0] << std::endl;

        if (temp.getType() == -1)
            fColor[i]->setText("E");
        else
            fColor[i]->setText("c");

        if (i < parser.size())
            parser[i] = temp;
        else
            parser.push_back(temp);
        
        if (temp.x.size() > 0)
        {
            qDebug() <<"drawing "<< i <<" : " << le[i]->text();
            QColor c = QColor(rand() % 200, rand() % 200, rand() % 200, 255);//Qt::GlobalColor(rand()%16);
            if (c.isValid()) {
                QString qss = QString("background-color: %1").arg(c.name());
                fColor[i]->setStyleSheet(qss);
            }
            QPen pen = QPen(c);
            pen.setWidth(2);
            //qDebug() << i<<" "<<ui->qcp->graphCount();
            ui->qcp->graph(i + 2)->data()->clear();
            ui->qcp->graph(i + 2)->setPen(pen); // line color blue for first graph
            ui->qcp->graph(i + 2)->setBrush(QBrush(QColor(255, 255, 255, 0))); // first graph will be filled with translucent blue
            ui->qcp->graph(i + 2)->setData(temp.x, temp.y);
            ui->qcp->rescaleAxes();
        }
        ui->qcp->replot();
        
     });
    
    connect(fVisible[i], &QPushButton::clicked, this, [=] {
        //fVisible[i]->setVisible(false);
        if (fVisible[i]->isChecked())
            ui->qcp->graph(i+2)->setVisible(true);
        else
            ui->qcp->graph(i+2)->setVisible(false);
        ui->qcp->replot();
        });

    
}



//void Viewer::checkFunction()//接收到enterPressEvent -> checkFunction -> 丟給parser check, 回傳一串vector data ----> 用data畫圖

void Viewer::deleteFunction(int i)
{
    qDebug() << "delete " << i;
    //fDelete.erase(i);
    ui->gridLayout->removeWidget(le[i]);
    ui->gridLayout->removeWidget(fColor[i]);
    ui->gridLayout->removeWidget(fVisible[i]);
    ui->gridLayout->removeWidget(fDelete[i]);
    ui->qcp->removeGraph(ui->qcp->graph(i + 2));
    ui->qcp->replot();
}

void Viewer::setXY() {
    //std::vector<QVector<double>> x, y;
    for (int i = 0; i < 251; ++i)
    {
        x[0][i] = i;
        y[0][i] = qCos(i / 10.0); // exponentially decaying cosine
    }
    for (int i = 0; i < 251; ++i)
    {
        x[1][i] = i;
        y[1][i] = 5;
    }
}