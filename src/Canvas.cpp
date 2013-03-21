#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stack>

#include "FreeImage.h"

#include "Bezier.h"
#include "Canvas.h"

Canvas::Canvas(unsigned size)
  : _size(size),
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

  std::cout << "Beginning upsample for loop" << std::endl
            << "pixels.size() / 3 = " << pixels.size() / 3 << std::endl;
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

  std::cout << "Finalizing upsample" << std::endl;
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

    std::stack<std::vector<float> > buffers;

    for (unsigned size = 512; size > 1; size /= 2) {
      // Take a screenshot
      std::ostringstream oss;
      oss << "test_down_" << size << ".bmp";
      screenshot(oss.str(), size);

      // Read the pixels from the current buffer
      std::vector<float> pixels(size * size * 3, 0.0f);
      glReadPixels(0, 0, size, size, GL_RGB, GL_FLOAT, &pixels[0]);

      // Add it to the buffer collection
      std::cout << "Adding buffer of size " << size << " to the stack!" << std::endl;
      buffers.push(pixels);

      // Downsample the image
      std::vector<float> npixels = downsample(pixels);

      std::cout << "Drawing downsample" << std::endl;

      // Draw the new buffer onto the screen
      glDrawPixels(size / 2, size / 2, GL_RGB, GL_FLOAT, &npixels[0]);
      glFlush();
    }

    std::cout << "Beginning to pop buffers" << std::endl;
    while (true) {
      // Pop the top buffer
      std::vector<float> oldBuffer = buffers.top();
      buffers.pop();

      // Check if buffers is empty
      std::cout << "buffers.empty() = " << (buffers.empty() ? "true" : "false") << std::endl;
      if (buffers.empty()) {
        break;
      }

      // Get the next biggest buffer, but don't pop
      std::vector<float> upBuffer = buffers.top();

      // Upsample that buffer
      upsample(oldBuffer, upBuffer);

      unsigned sz = sqrt(upBuffer.size() / 3);

      std::cout << "Drawing onto the screen" << std::endl;

      // Draw it onto the screen
      glDrawPixels(sz, sz, GL_RGB, GL_FLOAT, &upBuffer[0]);
      glFlush();

      // Take a screenshot
      std::ostringstream oss;
      oss << "test_up_" << sz << ".bmp";
      screenshot(oss.str(), sz);
    }
  }
}

void Canvas::screenshot(std::string filename, unsigned size)
{
  //filename = "assets/" + filename;

  //std::vector<unsigned char> pixels(size * size * 3, 0);
  //glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);

  //FIBITMAP *image = FreeImage_ConvertFromRawBits(&pixels[0], size, size, 3 * size, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
  //FreeImage_Save(FIF_BMP, image, filename.c_str(), 0);

  //FreeImage_Unload(image);
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
