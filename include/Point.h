#ifndef _POINT_H_
#define _POINT_H_

#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>

#include <GL/gl.h>

#define POINT_SIZE 5

template<int n>
class Point
{
  private:
    float _data[n];

  public:
    Point()
    {
      memset(_data, 0, sizeof(float) * n);
    }

    Point(float *d)
    {
      for (int i = 0; i < n; i++) {
        _data[i] = d[i];
      }
    }

    Point operator+(const Point &p) const
    {
      Point<n> ret;
      for (int i = 0; i < n; i++) {
        ret._data[i] = _data[i] + p._data[i];
      }
      return ret;
    }

    Point operator-(const Point &p) const
    {
      Point<n> ret;
      for (int i = 0; i < n; i++) {
        ret._data[i] = _data[i] - p._data[i];
      }
      return ret;
    }

    Point operator*(float s) const
    {
      Point<n> ret;
      for (int i = 0; i < n; i++) {
        ret._data[i] = _data[i] * s;
      }
      return ret;
    }

    Point operator/(float s) const
    {
      Point<n> ret;
      for (int i = 0; i < n; i++) {
        ret._data[i] = _data[i] / s;
      }
      return ret;
    }

    void operator+=(const Point &p)
    {
      for (int i = 0; i < n; i++) {
        _data[i] += p._data[i];
      }
    }

    void operator-=(const Point &p)
    {
      for (int i = 0; i < n; i++) {
        _data[i] -= p._data[i];
      }
    }

    void operator*=(float s)
    {
      for (int i = 0; i < n; i++) {
        _data[i] *= s;
      }
    }

    void operator/=(float s)
    {
      for (int i = 0; i < n; i++) {
        _data[i] /= s;
      }
    }

    float dot(const Point &p) const
    {
      float ret = 0;
      for (int i = 0; i < n; i++) {
        ret += _data[i] * p._data[i];
      }
      return ret;
    }

    void zero()
    {
      for (int i = 0; i < n; i++) {
        _data[i] = 0;
      }
    }

    void normalize()
    {
      float length = 0;
      for (int i = 0; i < n; i++) {
        length += _data[i] * _data[i];
      }
      length = sqrt(length);
      for (int i = 0; i < n; i++) {
        _data[i] /= length;
      }
    }

    float &operator[](int i)
    {
      return _data[i];
    }

    void draw()
    {
      float x = _data[0], y = _data[1];

      glBegin(GL_QUADS);
        glVertex2f(x - POINT_SIZE / 2.0f, y + POINT_SIZE / 2.0f);
        glVertex2f(x - POINT_SIZE / 2.0f, y - POINT_SIZE / 2.0f);
        glVertex2f(x + POINT_SIZE / 2.0f, y - POINT_SIZE / 2.0f);
        glVertex2f(x + POINT_SIZE / 2.0f, y + POINT_SIZE / 2.0f);
      glEnd();
    }

    friend std::ostream &operator<<(std::ostream &os, const Point &p)
    {
      os << "{ ";
      for (size_t i = 0; i < n; i++) {
        os << p._data[i] << ", ";
      }
      return os << " }";
    }
};

#endif
