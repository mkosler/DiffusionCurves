#ifndef _CURVE_H_
#define _CURVE_H_

#include <iostream>
#include <vector>

#include <GL/gl.h>

#include "Point.h"

template<int n>
class Curve
{
  private:
    friend class Canvas;

  protected:
    bool _hasBeenUpdated;
    unsigned _degree;
    float _step;
    std::vector<Point<n> > _controlPoints;
    std::vector<Point<n> > _interpolants;

    virtual const Point<n> interpolate(const float t, const size_t finish) = 0;

    Curve(std::vector<Point<n> > controlPoints, unsigned degree)
      : _hasBeenUpdated(false),
        _degree(degree),
        _step(0.01f),
        _controlPoints(controlPoints)
    {
    }

    Curve(std::vector<Point<n> > controlPoints, unsigned degree, float step)
      : _hasBeenUpdated(false),
        _degree(degree),
        _step(step),
        _controlPoints(controlPoints)
    {
    }

  public:
    virtual ~Curve()
    {
    }

    virtual void initialize() = 0;

    virtual void update(float dt)
    {
    }

    void draw()
    {
      glColor3f(1.0f, 0.0f, 0.0f);
      for (size_t i = 0; i < _controlPoints.size(); i++) {
        _controlPoints[i].draw();
      }

      glBegin(GL_QUADS);
        for (size_t i = 0; i < _interpolants.size() - 1; i++) {
          // Get the normal
          Point<n> normal = _interpolants[i + 1] - _interpolants[i];

          // Swap the x- and y-coordinates
          float temp = normal[0];
          normal[0] = -normal[1];
          normal[1] = temp;

          normal.normalize();
          normal *= 3;

          glColor3f(_interpolants[i][2], _interpolants[i][3], _interpolants[i][4]);
          glVertex2f(_interpolants[i][0] + normal[0], _interpolants[i][1] + normal[1]);

          glColor3f(_interpolants[i + 1][2], _interpolants[i + 1][3], _interpolants[i + 1][4]);
          glVertex2f(_interpolants[i + 1][0] + normal[0], _interpolants[i + 1][1] + normal[1]);

          glColor3f(_interpolants[i + 1][5], _interpolants[i + 1][6], _interpolants[i + 1][7]);
          glVertex2f(_interpolants[i + 1][0] - normal[0], _interpolants[i + 1][1] - normal[1]);

          glColor3f(_interpolants[i][5], _interpolants[i][6], _interpolants[i][7]);
          glVertex2f(_interpolants[i    ][0] - normal[0], _interpolants[i    ][1] - normal[1]);
        }
      glEnd();
    }

    friend std::ostream &operator<<(std::ostream &os, const Curve &c)
    {
      os << "=== Control Points [" << c._controlPoints.size() << "] ===" << std::endl;
      for (size_t i = 0; i < c._controlPoints.size(); i++) {
        os << '\t' << c._controlPoints[i] << std::endl;
      }

      os << "=== Interpolants [" << c._interpolants.size() << "] ===" << std::endl;
      for (size_t i = 0; i < c._interpolants.size(); i++) {
        os << '\t' << c._interpolants[i] << std::endl;
      }

      return os;
    }
};

#endif
