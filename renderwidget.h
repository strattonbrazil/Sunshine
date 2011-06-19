#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <imageviewer.h>

namespace Ui {
    class RenderWidget;
}

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit               RenderWidget(QWidget *parent = 0);
                           ~RenderWidget();
    void                   open(QString fileName);
private:
    Ui::RenderWidget       *ui;
    ImageViewer*           _viewer;
};

#endif // RENDERWIDGET_H
