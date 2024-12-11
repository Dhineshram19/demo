#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QCustomPlot *customPlot;         // QCustomPlot instance
    QCPColorMap *colormap;          // Colormap for the plot
    QCPColorScale *colorScale;      // Color scale for the colormap

    void generateData();            // Method to generate the matrix data

private slots:
    void showTooltip(QMouseEvent *event); // Slot to display a tooltip for the plot
};

#endif // MAINWINDOW_H
