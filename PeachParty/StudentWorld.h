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
    int playerAction(Actor* player);
    bool validSquare(int x, int y);
    int getBank(){return bank;}
    bool doesIntersect(Actor* a1, int player);
    bool doesOverlap(Actor* a1, Actor* a2); // Vortex graphical overlap
    void changeCoins(int amount, int player);
    bool playerMoving(int player);
    void changePlayerDirection(int player, int direction);
    bool atFork(Actor* player);
    int countValidAdjacent(Actor* square);
    void depositInBank(int player);
    void withDrawFromBank(int player);
    
private:
    std::set<std::string> validCoords; // 0-15 coords
    std::vector<Actor*> actors;
    Actor* peach;
    Actor* yoshi;
    int bank;
};

#endif // STUDENTWORLD_H_
