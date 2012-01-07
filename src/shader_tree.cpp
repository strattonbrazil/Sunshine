#include "shader_tree.h"
#include "ui_shader_tree_window.h"
#include "sunshine.h"
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include "node_widget.h"

#include <iostream>


class ShaderWorkspace : public QWidget
{
public:
    ShaderWorkspace(ShaderTreeWindow* window);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    QList<Shader*> zSortedNodes();
private:
    ShaderTreeWindow* _window;
    Shader* _dragShader;
    QPoint pick;
};

ShaderWorkspace::ShaderWorkspace(ShaderTreeWindow* window) : _window(window)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _dragShader = 0;
}

void ShaderWorkspace::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        _dragShader = 0;
        pick = event->pos();
        QList<Shader*> nodes = zSortedNodes();
        for (int i = nodes.size()-1; i >= 0; i--) {
            Shader* shader = nodes[i];
            if (QRect(shader->position(), shader->nodeSize).contains(pick)) {
                _dragShader = shader;
                break;
            }
        }
    }
}

void ShaderWorkspace::mouseReleaseEvent(QMouseEvent *event)
{
    _dragShader = 0;
}

void ShaderWorkspace::mouseMoveEvent(QMouseEvent* event)
{
    if (_dragShader != 0) {
        _dragShader->workspacePos += event->pos() - pick;
        pick = event->pos();

        update();
    }
}


void ShaderWorkspace::paintEvent(QPaintEvent *event)
{
    QColor bgColor(60,60,60);
    QColor shadowColor(40,40,40);
    QColor nodeColor(220,220,220);
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), bgColor);



    QList<Shader*> nodes = zSortedNodes();

    // draw all the nodes
    int zCount = 0;
    foreach(Shader* shader, nodes) { // should be sorted by zOrder
        const int PREVIEW_WIDTH = 64;
        const int NODE_WIDTH = 100 + PREVIEW_WIDTH;
        const int NODE_HEIGHT = 200;

        NodeWidget widget;
        QString shaderName = SunshineUi::activeScene()->assetName(shader);
        widget.setShader(shader);





        // add the inputs
        /*
        foreach(Attribute attribute, shader->inputs()) {
            widget.addInput(attribute->property("name").toString());
        }
        */
        widget.resize(widget.minimumSizeHint());

        shader->nodeSize = widget.size();
        shader->zOrder = zCount++;


        /*

        QWidget widget;
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setMargin(0);
        widget.setLayout(layout);

        // label the shader
        QString shaderName = SunshineUi::activeScene()->assetName(shader);
        QLabel* label = new QLabel(shaderName);
        label->setStyleSheet("QLabel { background-color: #ccffcc; font-size: 20px; }");
        layout->addWidget(label);

        // add preview and shader
        QHBoxLayout* subLayout = new QHBoxLayout();
        subLayout->addWidget(new QLabel(new QIcon(":/icons/mesh_icon.png")));
        layout->addWidget(subLayout);



        layout->addStretch();
        */
        //widget.resize(NODE_WIDTH, NODE_HEIGHT);

        QPixmap wImage = QPixmap::grabWidget(&widget);

        QRect shadowRect(shader->position().x()+10, shader->position().y()+10,
                         widget.width(), widget.height());
        painter.fillRect(shadowRect,shadowColor);
        //painter.fillRect(nodeRect, nodeColor);
        painter.drawPixmap(shader->position().x(), shader->position().y(), wImage);


    }
}

QList<Shader*> ShaderWorkspace::zSortedNodes()
{
    QList<Material*> materialNodes = _window->selectedMaterials();
    QSet<ShaderConnection> nodeLinks;
    QSet<Shader*> nodes;
    for (int i = 0; i < materialNodes.size(); i++) {
        Material* material = materialNodes[i];
        nodes.insert(material);
        foreach(ShaderConnection c, material->nodeLinks) {
            nodeLinks.insert(c);
        }
    }

    // get a list of all shaders
    //
    bool nodeAdded = true;
    while (nodeAdded) {
        nodeAdded = false;
        foreach(ShaderConnection c, nodeLinks) {
            if (!nodes.contains(c.fromNode)) {
                nodes.insert(c.fromNode);
                nodeAdded = true;
            }
            if (!nodes.contains(c.toNode)) {
                nodes.insert(c.toNode);
                nodeAdded = true;
            }
        }
    }

    QList<Shader*> sortedShaders = nodes.toList();
    qSort(sortedShaders);

    return nodes.toList();
}

ShaderTreeWindow::ShaderTreeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShaderTreeWindow)
{
    ui->setupUi(this);

    ui->workspaceFrame->layout()->addWidget(new ShaderWorkspace(this));

    connect(ui->materialList, SIGNAL(clicked(QModelIndex)), SLOT(on_materialSelect()));

    ui->shader2DList->addItem(new QListWidgetItem(QIcon(":/icons/file_node.png"), "File"));
    ui->shader2DList->addItem(new QListWidgetItem(QIcon(":/icons/checker_node.png"), "Checker"));

    ui->shaderUtilList->addItem(new QListWidgetItem("UV Lookup 2D"));
    //ui->shaderList2D->addItem("File");
    //ui->shaderList2D->addItem("Checker");
}

void ShaderTreeWindow::setMaterialModel(QAbstractItemModel *model)
{
    ui->materialList->setModel(model);


}

QList<Material*> ShaderTreeWindow::selectedMaterials()
{
    QList<Material*> materials;
    foreach(QModelIndex index, ui->materialList->selectionModel()->selectedIndexes()) {
        QString materialName = ui->materialList->model()->data(index).toString();
        materials.append(SunshineUi::activeScene()->material(materialName));
    }

    return materials;
}

ShaderTreeWindow::~ShaderTreeWindow()
{
    delete ui;
}

void ShaderTreeWindow::on_materialSelect()
{
    update();
}
