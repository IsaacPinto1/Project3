#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"
#include <set>

class Actor: public GraphObject{
public:
    Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world);
    virtual void doSomething() = 0;
    StudentWorld* getWorld(){return m_world;}
    int getWalkingDirection(){return walkingDir;}
    void setWalkingDirection(int dir){walkingDir = dir;}
    bool isAlive(){return m_isAlive;}
    virtual ~Actor();
    
    virtual bool hasAVortex(){return false;}
    virtual int getRoll() {return 0;}
    virtual int getCoins() {return 0;}
    virtual int getStars() {return 0;}
    virtual bool isMoving(){return false;}
    virtual int changeCoins(int amount){return 0;}
    virtual bool changesDirection() {return false;}
    
private:
    int walkingDir;
    StudentWorld* m_world;
    bool m_isAlive;
};


class Player: public Actor{
public:
    Player(int playerID, int x, int y, StudentWorld* world);
    ~Player();
    virtual void doSomething();
    
    virtual bool hasAVortex(){return hasVortex;}
    virtual int getRoll(){return ceil(1.0*ticks_to_move/8);}
    virtual int getCoins(){return m_coins;}
    virtual int getStars(){return m_stars;}
    virtual bool isMoving(){return waitingToRoll == false;}
    virtual int changeCoins(int amount);
private:
    int ticks_to_move;
    bool waitingToRoll;
    bool hasVortex;
    int m_stars;
    int m_coins;
};

class Enemy: public Actor{
    
};

class Square: public Actor{
public:
    Square(int imageID, int x, int y, StudentWorld* world);
    void trackPlayer(int player);
    void removePlayer(int player);
    bool containsPlayer(int player);
private:
    std::set<int> players; // Peach==1 Yoshi == 2
};

class CoinSquare:public Square{
public:
    CoinSquare(int imageID, int x, int y, StudentWorld* world, int coin);
    virtual void doSomething();
private:
    int coinAmount;
};


class DirectionalSquare: public Square{
public:
    DirectionalSquare(int imageID, int x, int y, StudentWorld* world, int dir);
    virtual void doSomething();
    virtual bool changesDirection() {return true;}
private:
    int direction;
};

class EventSquare: public Square{
public:
    EventSquare(int imageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
};


class BankSquare: public Square{
public:
    BankSquare(int imageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
};

#endif // ACTOR_H_
