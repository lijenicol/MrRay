#ifndef MR_RAY_FILM_H
#define MR_RAY_FILM_H

#include <mutex>
#include <string>
#include <vector>

#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

struct Tile
{
    const unsigned int top, left, width, height;
    Colour *colours;

    Tile(
        unsigned int top, unsigned int left, unsigned int width,
        unsigned int height)
        : top(top)
        , left(left)
        , width(width)
        , height(height)
    {
        colours = new Colour[width * height];
    }

    ~Tile() { delete[] colours; }
};

class TilesQueue
{
public:
    TilesQueue()
        : currentIndex(0)
    {
    }

    void addTile(std::shared_ptr<Tile> tile)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tiles.push_back(tile);
    }

    Tile *getTile()
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (currentIndex < tiles.size()) return tiles[currentIndex++].get();
        return nullptr;
    }

    size_t remainingTiles()
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        return tiles.size();
    }

private:
    std::vector<std::shared_ptr<Tile>> tiles;
    int currentIndex;
    std::mutex queueMutex;
};

struct Film
{
public:
    const unsigned int width, height;

    Film(unsigned int width, unsigned int height)
        : width(width)
        , height(height)
    {
        _colours = new Colour[width * height];
    }

    ~Film() { delete[] _colours; }

    // Copy a tile of colours to this film
    void writeTile(const Tile &tile);

    // Write film to file
    void writeToFile(const std::string &path);

    Colour *getData();

private:
    std::mutex filmMutex;
    Colour *_colours;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_FILM_H
