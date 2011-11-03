#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <imageviewer.h>
#include <bindable.h>

namespace Ui {
    class RenderWidget;
}

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = 0);
    ~RenderWidget();
    void                   open(QString fileName);
private:
    Ui::RenderWidget *ui;
    ImageViewer*           _viewer;
};

class RenderSettings : public Bindable
{
public:
    RenderSettings();
};

#endif // RENDERWIDGET_H
