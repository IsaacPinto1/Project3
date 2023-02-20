#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"

class Actor: public GraphObject{
public:
    Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world);
    virtual void doSomething() = 0;
    StudentWorld* getWorld(){return m_world;}
    int getWalkingDirection(){return walkingDir;}
    void setWalkingDirection(int dir){walkingDir = dir;}
    bool isAlive(){return m_isAlive;}
    virtual ~Actor();
private:
    int walkingDir;
    StudentWorld* m_world;
    bool m_isAlive;
};


class Player: public Actor{
public:
    Player(int playerNumb, int x, int y, StudentWorld* world);
    ~Player();
    virtual void doSomething();
private:
    int playerNumb;
    int ticks_to_move;
    bool waitingToRoll;
};

class Enemy: public Actor{
    
};

class Square: public Actor{
public:
    Square(int imageID, int x, int y, StudentWorld* world);
};

class CoinSquare:public Square{
public:
    CoinSquare(int imageID, int x, int y, StudentWorld* world);
    void virtual doSomething();
};

#endif // ACTOR_H_
