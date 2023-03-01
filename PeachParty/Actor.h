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
    bool isAlive(){return m_isAlive;}
    virtual ~Actor(){}
    
    void trackPlayer(int player){players.insert(player);}
    void removePlayer(int player){players.erase(player);}
    bool containsPlayer(int player){return players.count(player)==1;}
    void clearPlayers(){players.clear();}
    
    virtual bool changesDirection() {return false;}
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
    virtual ~Mover(){}
    int getWalkingDirection(){return walkingDir;}
    void setWalkingDirection(int dir){walkingDir = dir;}
    int getTicksToMove(){return ticks_to_move;}
    bool isMoving(){return waitingToMove == false;}
    int getRoll(){return ceil(1.0*ticks_to_move/8);}
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
    Player(Player& position, Player& stats, int player);
    virtual ~Player(){}
    virtual void doSomething();
    
    bool hasAVortex(){return hasVortex;}
    void giveVortex(){hasVortex = true;}
    int getCoins(){return m_coins;}
    int getStars(){return m_stars;}
    int changeCoins(int amount);
    int changeStars(int amount);
    virtual void stopWalking(){setWaitingStatus(true);}
    virtual bool checkFork();
    void setStars(int amount);
    void setCoins(int amount);
    
private:
    bool hasVortex;
    int m_stars;
    int m_coins;
};

class Enemy: public Mover{
public:
    Enemy(int ImageID, int x, int y, StudentWorld* world);
    virtual ~Enemy(){}
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
    virtual bool checkFork(){return true;}
};



class Square: public Actor{
public:
    Square(int imageID, int x, int y, StudentWorld* world, int dir);
    virtual ~Square(){}
    bool newPlayerLanded(int player);
    void removeTrackIfGone(int player);
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
