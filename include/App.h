#ifndef _APP_H_
#define _APP_H_

#include <string>
#include <vector>

#include <SFML/Window.hpp>

#include "Canvas.h"

class App
{
  private:
    static sf::Window _window;
    static Canvas _canvas;
    static std::vector<Point<8> > _pointBuffer;
    static float _leftColor[3], _rightColor[3];

    static void handleEvent(sf::Event &event);

    static void handleKeyPressed(sf::Event::KeyEvent &event);
    static void handleKeyReleased(sf::Event::KeyEvent &event);

    static void update(float dt);
    static void draw();

    static void addPoint(float x, float y);
    static void addPoint(float x, float y, float lr, float lg, float lb, float rr, float rg, float rb);

  public:
    static void initialize(sf::VideoMode mode, std::string title);
    static int run();
};

#endif
