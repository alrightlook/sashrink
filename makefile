PocketSaTools:main.cc unpack.cpp surface.cc
	g++ -g -o PocketSaTools `sdl2-config --cflags --libs` main.cc unpack.cpp surface.cc