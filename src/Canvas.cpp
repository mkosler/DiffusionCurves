#include "Canvas.h"

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
  while (!_curves.empty()) {
    delete _curves.back();
    _curves.pop_back();
  }
}

void Canvas::addCurve(Curve<8> *curve)
{
  curve->initialize();
  _curves.push_back(curve);
}

void Canvas::update(float dt)
{
  for (size_t i = 0; i < _curves.size(); i++) {
    _curves[i]->update(dt);
  }
}

void Canvas::draw()
{
  for (size_t i = 0; i < _curves.size(); i++) {
    _curves[i]->draw();
  }
}

std::ostream &operator<<(std::ostream &os, const Canvas &c)
{
  os << "=== Canvas [" << c._curves.size() << "] ===" << std::endl;
  for (size_t i = 0; i < c._curves.size(); i++) {
    os << '\t' << *c._curves[i] << std::endl;
  }
  return os;
}
