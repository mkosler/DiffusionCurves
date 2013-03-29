#ifndef _APP_H_
#define _APP_H_

#include <string>
#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Canvas.h"

class App
{
  private:
    static sf::Window _window;
    static Canvas _canvas;
    static std::vector<Point<8> > _pointBuffer;
    static bool _isEditMode;
    static sf::Rect<int> _selectionRect;

    static void handleEvent(sf::Event &event);

    static void handleKeyPressed(sf::Event::KeyEvent &event);
    static void handleKeyReleased(sf::Event::KeyEvent &event);

    static void handleMouseButtonPressed(sf::Event::MouseButtonEvent &event);
    static void handleMouseButtonReleased(sf::Event::MouseButtonEvent &event);

    static void handleMouseMove(sf::Event::MouseMoveEvent &event);

    static void update(float dt);
    static void draw();

    static void addPoint(float x, float y);
    static void pushCurve();

  public:
    static void initialize(sf::VideoMode mode, std::string title);
    static int run();
    static void load(std::string filename);
};

#endif
