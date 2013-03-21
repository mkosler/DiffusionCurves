#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stack>

#include <SOIL.h>

#include "Bezier.h"
#include "Canvas.h"

#define SMOOTH_ITERATIONS 100

Canvas::Canvas(unsigned size)
  : _size(size),
    _arePointsVisible(true),
    _isFinalized(false),
    _hasDiffusionCurve(false),
    _curveIndex(-1),
    _selectionIndex(-1)
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

std::vector<bool> Canvas::getConstraintMask(std::vector<float> pixels)
{
  std::vector<bool> constraintMask(pixels.size() / 3, false);
  for (size_t i = 0; i < constraintMask.size(); i++) {
    constraintMask[i] = !isBlack(pixels[(3 * i)], pixels[(3 * i) + 1], pixels[(3 * i) + 2]);
  }
  return constraintMask;
}

std::vector<float> Canvas::downsample(std::vector<float> pixels)
{
  unsigned sz = sqrt(pixels.size() / 3);
  unsigned dsz = sz / 2;

  std::vector<float> downPixels(dsz * dsz * 3, 0);
  for (size_t i = 0; i < (downPixels.size() / 3); i++) {
    unsigned row = i / dsz;

    unsigned x = (i * 2) + (row * sz);
    unsigned y = x + 1;
    unsigned z = x + sz;
    unsigned w = z + 1;

    unsigned count = 0;
    if (!isBlack(pixels[(3 * x)], pixels[(3 * x) + 1], pixels[(3 * x) + 2])) {
      count++;

      downPixels[(3 * i)]     += pixels[(3 * x)];
      downPixels[(3 * i) + 1] += pixels[(3 * x) + 1];
      downPixels[(3 * i) + 2] += pixels[(3 * x) + 2];
    }

    if (!isBlack(pixels[(3 * y)], pixels[(3 * y) + 1], pixels[(3 * y) + 2])) {
      count++;

      downPixels[(3 * i)]     += pixels[(3 * y)];
      downPixels[(3 * i) + 1] += pixels[(3 * y) + 1];
      downPixels[(3 * i) + 2] += pixels[(3 * y) + 2];
    }

    if (!isBlack(pixels[(3 * z)], pixels[(3 * z) + 1], pixels[(3 * z) + 2])) {
      count++;

      downPixels[(3 * i)]     += pixels[(3 * z)];
      downPixels[(3 * i) + 1] += pixels[(3 * z) + 1];
      downPixels[(3 * i) + 2] += pixels[(3 * z) + 2];
    }

    if (!isBlack(pixels[(3 * w)], pixels[(3 * w) + 1], pixels[(3 * w) + 2])) {
      count++;

      downPixels[(3 * i)]     += pixels[(3 * w)];
      downPixels[(3 * i) + 1] += pixels[(3 * w) + 1];
      downPixels[(3 * i) + 2] += pixels[(3 * w) + 2];
    }

    if (count > 0) {
      downPixels[(3 * i)]     /= count;
      downPixels[(3 * i) + 1] /= count;
      downPixels[(3 * i) + 2] /= count;
    }
  }

  return downPixels;
}

void Canvas::upsample(std::vector<float> pixels, std::vector<float> &upPixels)
{
  unsigned sz = sqrt(pixels.size() / 3);
  unsigned usz = sz * 2;

  for (size_t i = 0; i < (pixels.size() / 3); i++) {
    unsigned row = i / sz;

    unsigned x = (i * 2) + (row * usz);
    unsigned y = x + 1;
    unsigned z = x + usz;
    unsigned w = z + 1;

    if (isBlack(upPixels[(3 * x)], upPixels[(3 * x) + 1], upPixels[(3 * x) + 2])) {
      upPixels[(3 * x)]     = pixels[(3 * i)];
      upPixels[(3 * x) + 1] = pixels[(3 * i) + 1];
      upPixels[(3 * x) + 2] = pixels[(3 * i) + 2];
    }

    if (isBlack(upPixels[(3 * y)], upPixels[(3 * y) + 1], upPixels[(3 * y) + 2])) {
      upPixels[(3 * y)]     = pixels[(3 * i)];
      upPixels[(3 * y) + 1] = pixels[(3 * i) + 1];
      upPixels[(3 * y) + 2] = pixels[(3 * i) + 2];
    }

    if (isBlack(upPixels[(3 * z)], upPixels[(3 * z) + 1], upPixels[(3 * z) + 2])) {
      upPixels[(3 * z)]     = pixels[(3 * i)];
      upPixels[(3 * z) + 1] = pixels[(3 * i) + 1];
      upPixels[(3 * z) + 2] = pixels[(3 * i) + 2];
    }

    if (isBlack(upPixels[(3 * w)], upPixels[(3 * w) + 1], upPixels[(3 * w) + 2])) {
      upPixels[(3 * w)]     = pixels[(3 * i)];
      upPixels[(3 * w) + 1] = pixels[(3 * i) + 1];
      upPixels[(3 * w) + 2] = pixels[(3 * i) + 2];
    }
  }
}

void Canvas::smooth(std::vector<float> &pixels, std::vector<bool> mask)
{
  size_t sz = sqrt(mask.size());

  for (unsigned j = 0; j < SMOOTH_ITERATIONS; j++) {
    for (size_t i = 0; i < mask.size(); i++) {
      if (!mask[i]) {
        size_t r = i / sz, c = i % sz;

        float R = 0.0f, G = 0.0f, B = 0.0f;
        unsigned count = 0;
        if (c > 0) {
          count++;

          R += pixels[(3 * (i - 1))];
          G += pixels[(3 * (i - 1)) + 1];
          B += pixels[(3 * (i - 1)) + 2];
        }

        if (c < sz - 1) {
          count++;

          R += pixels[(3 * (i + 1))];
          G += pixels[(3 * (i + 1)) + 1];
          B += pixels[(3 * (i + 1)) + 2];
        }

        if (r > 0) {
          count++;

          R += pixels[(3 * (i - sz))];
          G += pixels[(3 * (i - sz)) + 1];
          B += pixels[(3 * (i - sz)) + 2];
        }

        if (r < sz - 1) {
          count++;

          R += pixels[(3 * (i + sz))];
          G += pixels[(3 * (i + sz)) + 1];
          B += pixels[(3 * (i + sz)) + 2];
        }

        R /= count; G /= count; B /= count;

        pixels[(3 * i)]     = R;
        pixels[(3 * i) + 1] = G;
        pixels[(3 * i) + 2] = B;
      }
    }
  }
}

void Canvas::addCurve(Curve<8> *curve)
{
  _curves.push_back(curve);
}

void Canvas::togglePointVisibility()
{
  _arePointsVisible = !_arePointsVisible;
}

void Canvas::clear()
{
  _isFinalized = _hasDiffusionCurve = false;
  _diffusionCurve.clear();
  _arePointsVisible = true;
  while (!_curves.empty()) {
    delete _curves.back();
    _curves.pop_back();
  }
}

void Canvas::finalize()
{
  _isFinalized = true;
  _arePointsVisible = false;
}

bool Canvas::isFinalized() const
{
  return _isFinalized;
}

void Canvas::update(float dt)
{
}

void Canvas::draw()
{
  if (_hasDiffusionCurve) {
    glDrawPixels(_size, _size, GL_RGB, GL_FLOAT, &_diffusionCurve[0]);
    return;
  }

  for (size_t i = 0; i < _curves.size(); i++) {
    if (_arePointsVisible) {
      _curves[i]->drawControlPoints();
    }
    _curves[i]->drawCurve();
  }
  glFlush();

  if (_isFinalized) {
    std::stack<std::vector<float> > buffers;

    for (unsigned size = 512; size > 1; size /= 2) {
      // Read the pixels from the current buffer
      std::vector<float> pixels(size * size * 3, 0.0f);
      glReadPixels(0, 0, size, size, GL_RGB, GL_FLOAT, &pixels[0]);
      if (size == _size) {
        std::ostringstream oss;
        oss << "test_down_" << size << ".tga";
        screenshot(oss.str(), size);
      }

      // Add it to the buffer collection
      buffers.push(pixels);

      // Downsample the image
      std::vector<float> npixels = downsample(pixels);

      // Draw the new buffer onto the screen
      glDrawPixels(size / 2, size / 2, GL_RGB, GL_FLOAT, &npixels[0]);
      glFlush();
    }

    while (true) {
      // Pop the top buffer
      std::vector<float> oldBuffer = buffers.top(); buffers.pop();

      // Check if buffers is empty
      if (buffers.empty()) {
        break;
      }

      // Get the next buffer on the stack to upsample and smooth
      std::vector<float> upBuffer = buffers.top(); buffers.pop();
      std::vector<bool> mask = getConstraintMask(upBuffer);

      // Upsample that buffer
      upsample(oldBuffer, upBuffer);
      smooth(upBuffer, mask);

      unsigned sz = sqrt(upBuffer.size() / 3);

      // Draw it onto the screen
      glDrawPixels(sz, sz, GL_RGB, GL_FLOAT, &upBuffer[0]);
      glFlush();

      // Add the smoothed buffer back to the stack
      buffers.push(upBuffer);
    }

    _diffusionCurve = std::vector<float>(_size * _size * 3, 0.0f);
    glReadPixels(0, 0, _size, _size, GL_RGB, GL_FLOAT, &_diffusionCurve[0]);

    std::ostringstream oss;
    oss << "test_up_" << _size << ".tga";
    screenshot(oss.str(), _size);

    _hasDiffusionCurve = true;
  }
}

void Canvas::selectPoint(sf::Rect<int> rect)
{
  for (size_t i = 0; i < _curves.size(); i++) {
    for (size_t j = 0; j < _curves[i]->_controlPoints.size(); j++) {
      Point<8> p = _curves[i]->_controlPoints[j];
      if (rect.Contains(p[0], p[1])) {
        _curveIndex = i; _selectionIndex = j;
        return;
      }
    }
  }

  _curveIndex = _selectionIndex = -1;
}

void Canvas::colorSelectedPoint()
{
  if (_curveIndex < 0 || _selectionIndex < 0) return;

  float lr, lg, lb, rr, rg, rb;
  std::cout << "Please enter left color (3 floats, 0.0f-1.0f): ";
  std::cin >> lr >> lg >> lb;

  _curves[_curveIndex]->updateLeftColor(_selectionIndex, lr, lg, lb);

  std::cout << "Please enter right color (3 floats, 0.0f-1.0f): ";
  std::cin >> rr >> rg >> rb;

  _curves[_curveIndex]->updateRightColor(_selectionIndex, rr, rg, rb);
}

void Canvas::moveSelectedPoint(float x, float y)
{
  if (_curveIndex < 0 || _selectionIndex < 0) return;

  _curves[_curveIndex]->updateLocation(_selectionIndex, x, y);
}

void Canvas::screenshot(std::string filename, unsigned size)
{
  filename = "assets/" + filename;

  if (!SOIL_save_screenshot(filename.c_str(), SOIL_SAVE_TYPE_TGA, 0, 0, size, size)) {
    std::cerr << "Error saving " << filename << std::endl;
  } else {
    std::cout << "Successfully saved " << filename << std::endl;
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
