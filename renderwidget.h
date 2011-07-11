#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QDialog>
#include <imageviewer.h>

namespace Ui {
    class RenderWidget;
}

class RenderWidget : public QDialog
{
    Q_OBJECT

public:
    explicit               RenderWidget(QDialog *parent = 0);
                           ~RenderWidget();
    void                   open(QString fileName);
private:
    Ui::RenderWidget       *ui;
    ImageViewer*           _viewer;
};

#endif // RENDERWIDGET_H
