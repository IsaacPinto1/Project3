#ifndef ACTOR_H_
#define ACTOR_H_

#include "StudentWorld.h"
#include "GraphObject.h"
#include <set>

class Actor: public GraphObject{
public:
    Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world);
    
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const{return m_world;}
    virtual int getWalkingDirection(){return 0;}
    virtual void setWalkingDirection(int dir){return;}
    bool isAlive(){return m_isAlive;}
    virtual ~Actor();
    
    void trackPlayer(int player){players.insert(player);}
    void removePlayer(int player){players.erase(player);}
    bool containsPlayer(int player){return players.count(player)==1;}
    void clearPlayers(){players.clear();}
    
    virtual bool hasAVortex(){return false;}
    virtual void giveVortex(){return;}
    virtual int getRoll() {return 0;}
    virtual int getCoins() {return 0;}
    virtual int getStars() {return 0;}
    virtual bool isMoving(){return false;}
    virtual int changeCoins(int amount){return 0;}
    virtual int changeStars(int amount){return 0;}
    virtual bool changesDirection() {return false;}
    virtual int getTicksToMove(){return 0;}
    virtual void setCoins(int amount){return;}
    virtual void setStars(int amount){return;}
    virtual bool doesMove(){return false;}
    void kill(){m_isAlive = false;}
    virtual bool impactable(){return false;}
    virtual void getVortexed(){return;}
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
    std::set<int> players;
};

class Mover: public Actor{
public:
    Mover(int imageID, int x, int y, int dir, int depth, StudentWorld* world, int walkDir, bool waiting, int ticks);
    ~Mover();
    virtual int getWalkingDirection(){return walkingDir;}
    virtual void setWalkingDirection(int dir){walkingDir = dir;}
    virtual int getTicksToMove(){return ticks_to_move;}
    virtual bool isMoving(){return waitingToMove == false;}
    virtual int getRoll(){return ceil(1.0*ticks_to_move/8);}
    void setTicks(int amount){ticks_to_move = amount;}
    void changeTicks(int amount){ticks_to_move += amount;}
    void setWaitingStatus(bool status){waitingToMove = status;}
    virtual void startMoving(); // Set ticks to random roll and change waiting to move status
    bool checkDirection(); // Check valid square in front
    void fixDirection(); // Change direction if not valid
    virtual void walk(); // Move in direction of motion, then decrement ticks. Call stopWalking() if ticks run out
    void newRandomDirection();
    virtual void stopWalking() = 0; // Behavior when ticks run out
    virtual bool checkFork() = 0; // Returns true if not at fork or was at fork but moved, false if stuck at fork
    virtual bool doesMove(){return true;}
    
private:
    bool waitingToMove;
    int ticks_to_move;
    int walkingDir;
};


class Player: public Mover{
public:
    Player(int playerID, int x, int y, StudentWorld* world);
    Player(Actor& position, Actor& stats, int player);
    Player(Actor& old, int x, int y, int player);
    ~Player();
    virtual void doSomething();
    
    virtual bool hasAVortex(){return hasVortex;}
    virtual void giveVortex(){hasVortex = true;}
    virtual int getCoins(){return m_coins;}
    virtual int getStars(){return m_stars;}
    virtual int changeCoins(int amount);
    virtual int changeStars(int amount);
    virtual void stopWalking(){setWaitingStatus(true);}
    virtual bool checkFork();
    virtual void setStars(int amount);
    virtual void setCoins(int amount);
    
private:
    bool hasVortex;
    int m_stars;
    int m_coins;
};

class Enemy: public Mover{
public:
    Enemy(int ImageID, int x, int y, StudentWorld* world);
    void setPauseCounter(int amount){pauseCounter = amount;}
    void changePauseCounter(int amount){pauseCounter += amount;}
    int getPauseCounter(){return pauseCounter;}
    bool shouldInteract(int player);
    virtual bool impactable(){return true;}
    virtual void getVortexed();
    
    virtual bool checkFork();
    virtual void stopWalking();
private:
    int pauseCounter;
};



class Boo: public Enemy{
public:
    Boo(int ImageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void startMoving();
    void Interact();
};

class Bowser: public Enemy{
public:
    Bowser(int ImageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
    virtual void stopWalking();
    void Interact(int player);
};

class Vortex: public Mover{
public:
    Vortex(int ImageID, int x, int y, StudentWorld* world, int walkDir);
    virtual void doSomething();
    virtual void walk();
    virtual void stopWalking(){return;} // Behavior when ticks run out
    virtual bool checkFork(){return false;}
};



class Square: public Actor{
public:
    Square(int imageID, int x, int y, StudentWorld* world, int dir);
    bool newPlayerLanded(int player);
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


class StarSquare: public Square{
public:
    StarSquare(int imageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
};

class DroppingSquare: public Square{
public:
    DroppingSquare(int imageID, int x, int y, StudentWorld* world);
    virtual void doSomething();
};

#endif // ACTOR_H_
