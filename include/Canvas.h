#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Curve.h"

class Canvas
{
  private:
    unsigned _size;

    std::vector<Curve<8>*> _curves;
    bool _arePointsVisible, _isFinalized, _hasDiffusionCurve;
    int _curveIndex, _selectionIndex;

    std::vector<float> _diffusionCurve;

    bool isBlack(float r, float g, float b);

    std::vector<bool> getConstraintMask(std::vector<float> pixels);

    std::vector<float> downsample(std::vector<float> pixels);
    void upsample(std::vector<float> pixels, std::vector<float> &upPixels);
    void smooth(std::vector<float> &pixels, std::vector<bool> mask);

  public:
    Canvas(unsigned size);
    ~Canvas();

    void addCurve(Curve<8> *curve);
    void togglePointVisibility();
    void clear();
    void finalize();

    bool isFinalized() const;

    void update(float dt);
    void draw();

    void selectPoint(sf::Rect<int> rect);
    void moveSelectedPoint(float x, float y);

    void screenshot(std::string filename, unsigned size);

    friend std::ostream &operator<<(std::ostream &os, const Canvas &c);

    static void load(std::string filename, Canvas &canvas);
    static void save(std::string filename, Canvas &canvas);
};

#endif
