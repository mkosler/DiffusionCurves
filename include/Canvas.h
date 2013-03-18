#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <iostream>
#include <vector>

#include "Curve.h"

class Canvas
{
  private:
    std::vector<Curve<8>*> _curves;
    bool _arePointsVisible;

  public:
    Canvas();
    ~Canvas();

    void addCurve(Curve<8> *curve);
    void togglePointVisibility();

    void update(float dt);
    void draw();

    friend std::ostream &operator<<(std::ostream &os, const Canvas &c);

    static void load(std::string filename, Canvas &canvas);
    static void save(std::string filename, Canvas &canvas);
};

#endif
