#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "Curve.h"

template<int n>
class Bezier
  : public Curve<n>
{
  private:
    const float deCasteljau(const float t, const float v[], size_t size) const
    {
      if (size == 1) return v[0];

      float nv[size - 1];
      for (size_t i = 0; i < size - 1; i++) {
        float lhs = (1.0f - t) * v[i],
              rhs = t          * v[i + 1];
        nv[i] = lhs + rhs;
      }

      return deCasteljau(t, nv, size - 1);
    }

    const Point<n> interpolate(const float t)
    {
      return interpolate(t, this->_controlPoints.size());
    }

  protected:
    const Point<n> interpolate(const float t, const size_t finish)
    {
      float components[n][finish];
      for (size_t i = 0; i < finish; i++) {
        for (size_t j = 0; j < n; j++) {
          components[j][i] = this->_controlPoints[i][j];
        }
      }

      float v[n];
      for (size_t i = 0; i < n; i++) {
        v[i] = deCasteljau(t, components[i], finish);
      }

      return Point<n>(v);
    }

  public:
    Bezier(std::vector<Point<n> > controlPoints, unsigned degree)
      : Curve<n>(controlPoints, degree)
    {
    }

    Bezier(std::vector<Point<n> > controlPoints, unsigned degree, float step)
      : Curve<n>(controlPoints, degree, step)
    {
    }

    ~Bezier()
    {
    }

    void initialize()
    {
      for (float i = 0.0f; i < 1.0f; i += this->_step) {
        this->_interpolants.push_back(interpolate(i));
      }
      this->_interpolants.push_back(interpolate(1.0f));
    }
};

#endif
