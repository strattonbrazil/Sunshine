#ifndef NODE_WIDGET_H
#define NODE_WIDGET_H

#include <QWidget>
#include "material.h"

namespace Ui {
    class NodeWidget;
}

class NodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeWidget(QWidget *parent = 0);
    void setShader(Shader* s);
    //void setLabel(QString label);
    //void addInput(QString text);
    ~NodeWidget();

private:
    Ui::NodeWidget *ui;
    Shader* _shader;
};

#endif // NODE_WIDGET_H
