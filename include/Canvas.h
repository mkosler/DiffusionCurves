#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <iostream>
#include <vector>

#include "Curve.h"

class Canvas
{
  private:
    unsigned _size;

    std::vector<Curve<8>*> _curves;
    bool _arePointsVisible, _isFinalized;

    bool isBlack(float r, float g, float b);

    std::vector<float> downsample(std::vector<float> pixels);
    void upsample(std::vector<float> pixels, std::vector<float> &upPixels);

  public:
    Canvas(unsigned size);
    ~Canvas();

    void addCurve(Curve<8> *curve);
    void togglePointVisibility();
    void clear();
    void finalize();

    void update(float dt);
    void draw();

    void screenshot(std::string filename, unsigned size);

    friend std::ostream &operator<<(std::ostream &os, const Canvas &c);

    static void load(std::string filename, Canvas &canvas);
    static void save(std::string filename, Canvas &canvas);
};

#endif
