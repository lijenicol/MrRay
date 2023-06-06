#ifndef FILM_H
#define FILM_H

#include <mutex>
#include <string>

#include "rtutils.h"

struct Tile
{
	const unsigned int top, left, width, height;
	Colour* colours;

	Tile(unsigned int top, unsigned int left, unsigned int width, 
		 unsigned int height) 
		: top(top), left(left), width(width), height(height) 
	{
		colours = new Colour[width * height];
	}

	~Tile()
	{
		delete[] colours;
	}
};

struct Film
{
public:
	const unsigned int width, height;

	Film(unsigned int width, unsigned int height) 
		: width(width), height(height)
	{
		colours = new Colour[width * height];
	} 

	~Film()
	{
		delete[] colours;
	}

	// Copy a tile of colours to this film
	void writeTile(const Tile& tile);

    // Write film to file
	void writeToFile(const std::string& path);

private:
	std::mutex filmMutex;
	Colour* colours;
};

#endif