#ifndef SHADER_TREE_H
#define SHADER_TREE_H

#include <QWidget>
#include <QAbstractItemModel>
#include "material.h"

namespace Ui {
    class ShaderTreeWindow;
}

class ShaderTreeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ShaderTreeWindow(QWidget *parent = 0);
    void setMaterialModel(QAbstractItemModel* model);
    QList<Material*> selectedMaterials();
    ~ShaderTreeWindow();
public slots:
    void on_materialSelect();
private:
    Ui::ShaderTreeWindow *ui;
};

#endif // SHADER_TREE_H
