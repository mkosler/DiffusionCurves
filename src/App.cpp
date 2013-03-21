#include <GL/gl.h>

#include "Bezier.h"
#include "App.h"

void App::handleEvent(sf::Event &event)
{
  switch (event.Type) {
    case sf::Event::KeyPressed:
      handleKeyPressed(event.Key);
      break;
    case sf::Event::Closed:
      _window.Close();
      break;
    case sf::Event::Resized:
      glViewport(0, event.Size.Width, event.Size.Height, 0);
      break;
    default:
      break;
  }
}

void App::handleKeyPressed(sf::Event::KeyEvent &event)
{
  switch (event.Code) {
    case sf::Key::Escape:
      _window.Close();
      break;
    case sf::Key::Space:
      std::cout << _canvas << std::endl;
      break;
    case sf::Key::C:
      _canvas.clear();
      break;
    case sf::Key::F:
      _canvas.finalize();
      break;
    case sf::Key::L:
      Canvas::load("assets/test.dat", _canvas);
      break;
    case sf::Key::S:
      std::cout << "Saving canvas..." << std::endl;
      Canvas::save("assets/test.dat", _canvas);
      break;
    case sf::Key::V:
      _canvas.togglePointVisibility();
      break;
    default:
      break;
  }
}

void App::handleKeyReleased(sf::Event::KeyEvent &event)
{
  switch (event.Code) {
    default:
      break;
  }
}

void App::update(float dt)
{
  _canvas.update(dt);
}

void App::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _canvas.draw();
}

void App::addPoint(float x, float y)
{
  addPoint(
    x,
    y,
    _leftColor[0],
    _leftColor[1],
    _leftColor[2],
    _rightColor[0],
    _rightColor[1],
    _rightColor[2]);
}

void App::addPoint(
  float x,
  float y,
  float lr,
  float lg,
  float lb,
  float rr,
  float rg,
  float rb)
{
  float f[8] = { x, y, lr, lg, lb, rr, rg, rb };
  _pointBuffer.push_back(Point<8>(f));
}

void App::initialize(sf::VideoMode mode, std::string title)
{
  _window.Create(mode, title);

  std::cout << "AntialiasingLevel: " << _window.GetSettings().AntialiasingLevel << std::endl;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glDisable(GL_CULL_FACE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, _window.GetWidth() - 1, _window.GetHeight() - 1, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int App::run()
{
  sf::Event event;

  while (_window.IsOpened()) {
    while (_window.GetEvent(event)) {
      handleEvent(event);
    }

    update(_window.GetFrameTime());
    draw();

    _window.Display();
  }

  return 1;
}

sf::Window App::_window;
Canvas App::_canvas(512);
std::vector<Point<8> > App::_pointBuffer;
float App::_leftColor[3] = { 0.0f, 0.0f, 0.0f };
float App::_rightColor[3] = { 0.0f, 0.0f, 0.0f };
