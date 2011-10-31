#include "attribute_editor.h"

#include <QStyledItemDelegate>
#include <QLineEdit>

// editor should check if its a C++-managed attribute and get/set it accordingly
//

AttributeEditor::AttributeEditor(QWidget *parent) :
    QStandardItemModel(parent), _instance(0)
{

}

void AttributeEditor::update(BindableP instance)
{
    _instance = instance;
    clear();

    for (int i = 0; i < _instance->attributeCount(); i++) {
        Attribute attribute = _instance->at(i);

        QList<QStandardItem*> attributeRow;
        QStandardItem* nameItem = new QStandardItem(attribute->property("name").toString());
        attributeRow << nameItem;

        QList<QList<QStandardItem*> > subRows;

        if (attribute->property("getter").isValid()) {
            //std::cout << attribute->type().toStdString() << std::endl;
            QString value = "?";
            if (attribute->type() == "point3") {
                QVector3D p = getBoundValue<QVector3D>(_instance, attribute);
                value = QString("(%1, %2, %3)").arg(p.x()).arg(p.y()).arg(p.z());

                // make sub-components
                QString subNames[] = { QString("X"), QString("Y"), QString("Z") };
                float channelValues[] = { p.x(), p.y(), p.z() };

                for (int i = 0; i < 3; i++) {
                    QStandardItem* subNameItem = new QStandardItem(subNames[i]);
                    QStandardItem* subChannelItem = new QStandardItem(QString::number(channelValues[i]));
                    QList<QStandardItem*> subRow;
                    subRow << subNameItem << subChannelItem;
                    subRows << subRow;
                }
            }
            //std::cout << getBoundValue<Point3>(_instance, attribute) << std::endl;
            attributeRow << new QStandardItem(value);
            appendRow(attributeRow);

            // append sub-rows
            foreach(QList<QStandardItem*> subRow, subRows) {
                nameItem->appendRow(subRow);
            }

        }
        else {
            attributeRow << new QStandardItem(attribute->property("value").toString());
            appendRow(attributeRow);
        }
    }

    QStringList headers;
    headers << "Property" << "Value";
    setHorizontalHeaderLabels(headers);
}

Qt::ItemFlags AttributeEditor::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    // only material editable
    if (index.column() == 1)
        defaultFlags |= Qt::ItemIsEditable;
    else
        defaultFlags ^= Qt::ItemIsEditable;

    return defaultFlags;
}

bool AttributeEditor::setData(const QModelIndex &index, const QVariant &inValue, int role=Qt::EditRole)
{
    QString attrName;// = data(index.sibling(index.row(), 0)).toString();
    if (index.parent().isValid())
        attrName = data(index.parent().sibling(index.parent().row(), 0)).toString();
    else
        attrName = data(index.sibling(index.row(), 0)).toString();
    Attribute attribute = _instance->attributeByName(attrName);
    /*
    Attribute attribute;
    if (index.parent().isValid())
        attribute = _instance->attribute(model->data(index.parent().sibling(index.parent().row(), 0)).toString());
    else
        attribute = _instance->attribute(model->data(index.sibling(index.row(), 0)).toString());
        */

    // perform any necessary clamping or adjustments
    QVariant outValue = inValue;
    if (attribute->property("type").toString() == "float") {
        float val = std::max(attribute->property("min").toFloat(), inValue.toFloat());
        outValue = QVariant(std::min(attribute->property("max").toFloat(), val));
    }
    else if (attribute->property("type").toString() == "int") {
        int val = std::max(attribute->property("min").toInt(), inValue.toInt());
        outValue = QVariant(std::min(attribute->property("max").toInt(), val));
    }
    else if (attribute->property("type").toString() == "color") {
    }

    if (attribute->property("getter").isValid() && attribute->property("setter").isValid()) {
        if (!(index.parent().isValid())) { // don't call setter for sub-channels
            AttributeEditor* model = (AttributeEditor*)(index.model());
            if (attribute->type() == "point3")
                setBoundValue<QVector3D>(model->instance(), attribute, outValue.value<Point3>());
        }
    }
    else if (attribute->property("getter").isValid())
        std::cerr << "warning: attempting to set bound value with invalid setter" << std::endl;
    else
        attribute->setProperty("value", outValue);

    return QStandardItemModel::setData(index, outValue, role);
}

#include <QColorDialog>

QWidget* AttributeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    AttributeEditor* model = (AttributeEditor*)(index.model());
    Attribute attribute;
    if (index.parent().isValid())
        attribute = model->attribute(model->data(index.parent().sibling(index.parent().row(), 0)).toString());
    else
        attribute = model->attribute(model->data(index.sibling(index.row(), 0)).toString());

    if (attribute->type() == "color") {
        QColorDialog* dialog = new QColorDialog();
        dialog->setWindowModality(Qt::ApplicationModal);
        dialog->setModal(TRUE);
        return new QColorDialog(dialog);
    }
    else if (attribute->type() == "point3") {
        if (index.parent().isValid())
            return QStyledItemDelegate::createEditor(parent, option, index);
        else
            return 0;
    }
    else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    /*
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(100);
    */

    //return editor;
}

void AttributeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    AttributeEditor* model = (AttributeEditor*)(index.model());
    Attribute attribute;
    if (index.parent().isValid())
        attribute = model->attribute(model->data(index.parent().sibling(index.parent().row(), 0)).toString());
    else
        attribute = model->attribute(model->data(index.sibling(index.row(), 0)).toString());


//    if (index.parent().isValid()) // subcomponents
  //      QStyledItemDelegate::setEditorData(editor, index);

    if (attribute->property("type").toString() == "color") {
        QColorDialog* dialog = qobject_cast<QColorDialog*>(editor);
        dialog->setCurrentColor(attribute->property("value").value<QColor>());
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }

}

#include "sunshine.h"

void AttributeItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    AttributeEditor* attributeModel = (AttributeEditor*)model;
    Attribute attribute;
    if (index.parent().isValid())
        attribute = attributeModel->attribute(model->data(index.parent().sibling(index.parent().row(), 0)).toString());
    else
        attribute = attributeModel->attribute(model->data(index.sibling(index.row(), 0)).toString());

    if (attribute->property("type").toString() == "color") {
        QColorDialog* dialog = qobject_cast<QColorDialog*>(editor);
        QColor selectedColor = dialog->currentColor();
        model->setData(index, selectedColor);
        //QItemDelegate::setModelData(editor, model, index);
    }
    else if (attribute->type() == "point3") {

        //if (!n.isEmpty())
        //    model->setData(index, editor->property(n), Qt::EditRole);

        // use QItemEditorFactory for mapping of QVariant type to QWidget
        QLineEdit* w = qobject_cast<QLineEdit*>(editor);
        float channelVal = w->text().toFloat();

        // get the current point and replace it with the changed value
        QVector3D p = getBoundValue<QVector3D>(attributeModel->instance(), attribute);
        if (index.row() == 0) p.setX(channelVal);
        else if (index.row() == 1) p.setY(channelVal);
        else p.setZ(channelVal);

        //std::cout << QVariant(p).value<QVector3D>() << std::endl;

        //QLineEdit w;
        //w.setText(QString("(%1, %2, %3)").arg(p.x()).arg(p.y()).arg(p.z()));
        model->setData(index.parent().sibling(index.parent().row(), 1), QVariant(p));
        model->setData(index, QVariant(channelVal));
        //QVector3D previous = qVariantValue<QVector3D>(model->data(index.parent().sibling(index.parent().row(), 1)));
        //std::cout << previous << std::endl;
        //model->setData(index.parent(), p);
        //QStyledItemDelegate::setModelData(editor, model, index.parent());
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }

    SunshineUi::updatePanels();
}

void AttributeItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    //editor->setGeometry(option.rect);
}

#include <QPainter>
#include <QApplication>

void AttributeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    AttributeEditor* model = (AttributeEditor*)(index.model());
    Attribute attribute;
    if (index.parent().isValid())
        attribute = model->attribute(model->data(index.parent().sibling(index.parent().row(), 0)).toString());
    else
        attribute = model->attribute(model->data(index.sibling(index.row(), 0)).toString());

    //if (option.state & QStyle::State_Selected)
      //  painter->fillRect(option.rect, option.palette.highlight());

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    if (index.column() == 0) {
        QStyleOptionViewItemV2 alignedOption = option;
        alignedOption.displayAlignment = Qt::AlignRight;
        QStyledItemDelegate::paint(painter, alignedOption, index);
    }
    else if (attribute->type() == "color") {
        QStyledItemDelegate::paint(painter, option, index);

        QRect borderRect = option.rect.adjusted(1,1,-1,-1);
        painter->fillRect(borderRect, Qt::black);
        QColor color = attribute->property("value").value<QColor>();
        QRect colorRect = option.rect.adjusted(2,2,-2,-2);
        painter->fillRect(colorRect, color);
    }
    else if (attribute->type() == "point3") {
        QVector3D p;
        if (attribute->property("getter").isValid())
            p = getBoundValue<QVector3D>(model->instance(), attribute);
        else
            p = QVector3D(-999,-999,-999);


        QStyleOptionViewItemV4 options = option;
        initStyleOption(&options, index);

        QStyle *style = QApplication::style();

        if (index.parent().isValid()) {

            //QStyle::drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const
            float channels[3] = { p.x(), p.y(), p.z() };



            options.text = QString::number(channels[index.row()]);
            style->drawControl(QStyle::CE_ItemViewItem, &options, painter);

            //QStyle *style = QApplication::style();
            //style->drawItemText(painter, option.rect, Qt::AlignLeft, , true, "blah");
        }
        else {
            options.text = QString("(%1, %2, %3)").arg(p.x()).arg(p.y()).arg(p.z());
            style->drawControl(QStyle::CE_ItemViewItem, &options, painter);
            //QStyledItemDelegate::paint(painter, option, index);
        }
        /*
        int spacing = 2;
        int channelWidth = 40;
        QStyle *style = QApplication::style();
        QStyleOptionViewItem tmpOption(option);

        QVector3D p;
        if (attribute->property("getter").isValid())
            p = getBoundValue<QVector3D>(model->instance(), attribute);
        else
            p = QVector3D(-999,-999,-999);

        float channels[3] = { p.x(), p.y(), p.z() };
        for (int i = 0; i < 3; i++) {
            QRect channelRect(option.rect.x()+i*(spacing+channelWidth), option.rect.y()+1, channelWidth, option.rect.height()-2);
            tmpOption.rect = channelRect;
            //style->drawPrimitive(QStyle::PE_FrameLineEdit, &tmpOption, painter);

            QLineEdit foo;
            foo.setText(QString::number(channels[i]));
            foo.resize(channelRect.width(), channelRect.height());

            QPixmap map = QPixmap::grabWidget(&foo);
            painter->drawPixmap(tmpOption.rect, map);
            //QImage img = map.toImage();


            //img.save("/tmp/widget.png");


            //QRect r = style->subElementRect(QStyle::SE_LineEditContents, &panel);
            //painter.setClipRect(r);
        }
        */

    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
