#include "sunshineui.h"

SunshineUi::SunshineUi(Scene* scene, QWidget *parent) :
    QMainWindow(parent)
{
    _scene = scene;
}

SunshineUi::SunshineUi(const SunshineUi &ui)
{
    _scene = ui.scene();
}
