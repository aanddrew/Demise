g++ -std=c++11 sfmlTest.cpp geom/Wall.cpp utils/Point2d.cpp utils/Vector2d.cpp
g++ sfmlTest.o Wall.o Point2d.o Vector2d.o -o sfml-test -lsfml-graphics -lsfml-window -lsfml-system