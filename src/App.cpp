#include <GL/gl.h>

#include "Bezier.h"
#include "App.h"

void App::handleEvent(sf::Event &event)
{
  switch (event.Type) {
    case sf::Event::Closed:
      _window.Close();
      break;
    case sf::Event::KeyPressed:
      handleKeyPressed(event.Key);
      break;
    case sf::Event::MouseButtonPressed:
      handleMouseButtonPressed(event.MouseButton);
      break;
    case sf::Event::MouseButtonReleased:
      handleMouseButtonReleased(event.MouseButton);
      break;
    case sf::Event::MouseMoved:
      handleMouseMove(event.MouseMove);
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
    case sf::Key::C:
      _canvas.clear();
      break;
    case sf::Key::E:
      _isEditMode = !_isEditMode;
      std::cout << "_isEditMode = " << (_isEditMode ? "true" : "false") << std::endl;
      break;
    case sf::Key::F:
      _canvas.finalize();
      break;
    case sf::Key::L:
      Canvas::load("assets/test.dat", _canvas);
      break;
    case sf::Key::Q:
      if (_isEditMode) {
        _canvas.colorSelectedPoint();
      }
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

void App::handleMouseButtonPressed(sf::Event::MouseButtonEvent &event)
{
  switch (event.Button) {
    case sf::Mouse::Left:
      if (!_canvas.isFinalized()) {
        if (_isEditMode) {
        } else {
          addPoint(event.X, event.Y);
        }
      }
      break;
    case sf::Mouse::Right:
      if (!_canvas.isFinalized()) {
        if (_isEditMode) {
          _selectionRect.Left = event.X; _selectionRect.Top = event.Y;
        } else {
          pushCurve();
        }
      }
      break;
    default:
      break;
  }
}

void App::handleMouseButtonReleased(sf::Event::MouseButtonEvent &event)
{
  switch (event.Button) {
    case sf::Mouse::Right:
      if (_isEditMode) {
        _canvas.selectPoint(_selectionRect);
        _selectionRect.Left = _selectionRect.Top = _selectionRect.Right = _selectionRect.Bottom = 0;
      }
      break;
    default:
      break;
  }
}

void App::handleMouseMove(sf::Event::MouseMoveEvent &event)
{
  if (_isEditMode) {
    if (_window.GetInput().IsMouseButtonDown(sf::Mouse::Left)) {
      _canvas.moveSelectedPoint(event.X, event.Y);
    } else if (_window.GetInput().IsMouseButtonDown(sf::Mouse::Right)) {
      _selectionRect.Right = event.X; _selectionRect.Bottom = event.Y;
    }
  }
}

void App::update(float dt)
{
  _canvas.update(dt);
}

void App::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (size_t i = 0; i < _pointBuffer.size(); i++) {
    _pointBuffer[i].draw();
  }

  glColor3f(0.0f, 1.0f, 0.0f);
  glBegin(GL_LINE_LOOP);
    glVertex2i(_selectionRect.Left, _selectionRect.Top);
    glVertex2i(_selectionRect.Left, _selectionRect.Bottom);
    glVertex2i(_selectionRect.Right, _selectionRect.Bottom);
    glVertex2i(_selectionRect.Right, _selectionRect.Top);
  glEnd();

  _canvas.draw();
}

void App::addPoint(float x, float y)
{
  float lr, lg, lb, rr, rg, rb;
  std::cout << "Please enter left color (3 floats, 0.0f-1.0f): ";
  std::cin >> lr >> lg >> lb;

  std::cout << "Please enter right color (3 floats, 0.0f-1.0f): ";
  std::cin >> rr >> rg >> rb;

  float f[8] = { x, y, lr, lg, lb, rr, rg, rb };
  _pointBuffer.push_back(Point<8>(f));
}

void App::pushCurve()
{
  if (!_pointBuffer.empty()) {
    _canvas.addCurve(new Bezier<8>(_pointBuffer, _pointBuffer.size() - 1));
    _pointBuffer.clear();
  }
}

void App::initialize(sf::VideoMode mode, std::string title)
{
  _window.Create(mode, title);

  std::cout << "AntialiasingLevel: " << _window.GetSettings().AntialiasingLevel << std::endl;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
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
bool App::_isEditMode = false;
sf::Rect<int> App::_selectionRect;
