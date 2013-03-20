#include <cstdio>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "FreeImage.h"

#include "Bezier.h"
#include "Canvas.h"

Canvas::Canvas(unsigned width, unsigned height)
  : _width(width),
    _height(height),
    _arePointsVisible(true),
    _isFinalized(false)
{
}

Canvas::~Canvas()
{
  clear();
}

bool Canvas::isBlack(float r, float g, float b)
{
  return (r + g + b) == 0;
}

void Canvas::downsample(float *pixels, unsigned side)
{
  unsigned nside = side / 2;
  for (unsigned i = 0; i < nside * nside; i++) {
    unsigned row = i / nside;

    unsigned x = (i * nside) + (row * side);
    unsigned y = x + 1;
    unsigned z = x + ((row + 1) * side);
    unsigned w = z + 1;

    unsigned count = 0;
    if (!isBlack(pixels[(3 * x)], pixels[(3 * x) + 1], pixels[(3 * x) + 2])) {
      count++;

      if (x != 0) {
        pixels[(3 * i)]     += pixels[(3 * x)];
        pixels[(3 * i) + 1] += pixels[(3 * x) + 1];
        pixels[(3 * i) + 2] += pixels[(3 * x) + 2];
      }
    }

    if (!isBlack(pixels[(3 * y)], pixels[(3 * y) + 1], pixels[(3 * y) + 2])) {
      count++;

      pixels[(3 * i)]     += pixels[(3 * y)];
      pixels[(3 * i) + 1] += pixels[(3 * y) + 1];
      pixels[(3 * i) + 2] += pixels[(3 * y) + 2];
    }

    if (!isBlack(pixels[(3 * z)], pixels[(3 * z) + 1], pixels[(3 * z) + 2])) {
      count++;

      pixels[(3 * i)]     += pixels[(3 * z)];
      pixels[(3 * i) + 1] += pixels[(3 * z) + 1];
      pixels[(3 * i) + 2] += pixels[(3 * z) + 2];
    }

    if (!isBlack(pixels[(3 * w)], pixels[(3 * w) + 1], pixels[(3 * w) + 2])) {
      count++;

      pixels[(3 * i)]     += pixels[(3 * w)];
      pixels[(3 * i) + 1] += pixels[(3 * w) + 1];
      pixels[(3 * i) + 2] += pixels[(3 * w) + 2];
    }

    pixels[(3 * i)]     /= count;
    pixels[(3 * i) + 1] /= count;
    pixels[(3 * i) + 2] /= count;
  }
}

void Canvas::first()
{
  for (unsigned size = _width; size > 2; size /= 2) {
    std::ostringstream oss;
    oss << "test_" << size << ".bmp";

    screenshot(oss.str(), size, size);

    float *pixels = new float[size * size * 3];
    glReadPixels(0, 0, size, size, GL_RGB, GL_FLOAT, pixels);

    downsample(pixels, size);

    glDrawPixels(size / 2, size / 2, GL_RGB, GL_FLOAT, pixels);
    glFlush();

    delete[] pixels;
  }
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

void Canvas::finalize()
{
  _isFinalized = true;
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
  glFlush();

  if (_isFinalized) {
    _isFinalized = false;
    first();
  }
}

void Canvas::screenshot(std::string filename, unsigned width, unsigned height)
{
  filename = "assets/" + filename;

  std::cout << "Filename: " << filename << std::endl;

  unsigned char *pixels = new unsigned char[width * height * 3];
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
  FreeImage_Save(FIF_BMP, image, filename.c_str(), 0);

  delete image;
  delete[] pixels;
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
