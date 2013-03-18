#include <fstream>

#include "Bezier.h"
#include "Canvas.h"

Canvas::Canvas()
  : _arePointsVisible(true)
{
}

Canvas::~Canvas()
{
  clear();
}

void Canvas::addCurve(Curve<8> *curve)
{
  curve->initialize();
  _curves.push_back(curve);
}

void Canvas::togglePointVisibility()
{
  _arePointsVisible = !_arePointsVisible;
}

void Canvas::clear()
{
  _arePointsVisible = true;
  while (!_curves.empty()) {
    delete _curves.back();
    _curves.pop_back();
  }
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
    if (_arePointsVisible) {
      _curves[i]->drawControlPoints();
    }
    _curves[i]->drawCurve();
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

void Canvas::load(std::string filename, Canvas &canvas)
{
  canvas.clear();

  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
  }

  size_t curvesCount, pointsCount;
  std::vector<Point<8> > controlPoints;
  float p[8];
  ifs >> curvesCount;
  for (size_t i = 0; i < curvesCount; i++) {
    ifs >> pointsCount;
    for (size_t j = 0; j < pointsCount; j++) {
      ifs >> p[0] >> p[1];
      ifs >> p[2] >> p[3] >> p[4];
      ifs >> p[5] >> p[6] >> p[7];

      controlPoints.push_back(Point<8>(p));
    }
    canvas.addCurve(new Bezier<8>(controlPoints, controlPoints.size() - 1));
  }
}

void Canvas::save(std::string filename, Canvas &canvas)
{
  std::ofstream ofs(filename.c_str());
  if (!ofs.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
  }

  ofs << canvas._curves.size() << std::endl << std::endl;
  for (size_t i = 0; i < canvas._curves.size(); i++) {
    Curve<8> *curve = canvas._curves[i];
    ofs << curve->_controlPoints.size() << std::endl;
    for (size_t j = 0; i < curve->_controlPoints.size(); i++) {
      Point<8> point = curve->_controlPoints[j];

      ofs << point[0] << ' ' << point[1] << std::endl;
      ofs << point[2] << ' ' << point[3] << ' ' << point[4] << std::endl;
      ofs << point[5] << ' ' << point[6] << ' ' << point[7] << std::endl << std::endl;
    }
  }

  ofs.close();
}
