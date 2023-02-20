#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>
#include <set>

class Actor;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool validSquare(int x, int y);
    
private:
    std::set<std::string> validCoords;
    std::vector<Actor*> actors;
    Actor* peach;
    Actor* yoshi;
};

#endif // STUDENTWORLD_H_
