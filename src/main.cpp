#include <SFML/Window.hpp>

#include "App.h"

#define WINDOW_SIZE 512

int main(int argc, char *argv[])
{
  App::initialize(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE, 32), "CPSC 645 HW2 - Michael Kosler");

  return App::run();
}
