#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cmath>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


// Actor Begin
Actor::Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world, int walkDir)
: GraphObject(imageID, startX, startY, dir, depth, 1.0)
{
    walkingDir = walkDir;
    m_world = world;
    m_isAlive = true;
}

Actor::~Actor(){}

// Actor End


// Player Begin
Player::Player(int playerID, int x, int y, StudentWorld* world)
:Actor(playerID, x, y, 0, 0, world, 0)
{
    waitingToRoll = true;
    ticks_to_move = 0;
    hasVortex = false;
    m_stars = 0;
    m_coins = 0;
    justTeleported = false;
}

Player::Player(Actor& position, Actor& stats, int player)
:Actor(player-1, position.getX(), position.getY(), position.getDirection(), 0, position.getWorld(), position.getWalkingDirection())
{
    waitingToRoll = !position.isMoving();
    ticks_to_move = position.getTicksToMove();
    hasVortex = stats.hasAVortex();
    m_stars = stats.getStars();
    m_coins = stats.getCoins();
    justTeleported = false;
}

Player::Player(Actor& old, int x, int y, int player)
: Actor(player-1, x, y, old.getDirection(), 0, old.getWorld(), old.getWalkingDirection())
{
    m_stars = old.getStars();
    m_coins = old.getCoins();
    hasVortex = old.hasAVortex();
    ticks_to_move = 0;
    waitingToRoll = true;
    justTeleported = true;
}

Player::~Player(){}

void Player::doSomething(){
    if(waitingToRoll){
        if(justTeleported){
            for(;;){
                int direction = randInt(0, 3); // 0 right, 1 up, 2 left, 3 down
                if(direction == 0){
                    if(getWorld()->validSquare(getX()/16+1, getY()/16)){
                        setDirection(0);
                        setWalkingDirection(0);
                        break;
                    }
                } else if(direction == 1){
                    if(getWorld()->validSquare(getX()/16, getY()/16+1)){
                        setDirection(0);
                        setWalkingDirection(90);
                        break;
                    }
                } else if(direction == 2){
                    if(getWorld()->validSquare(getX()/16-1, getY()/16)){
                        setDirection(180);
                        setWalkingDirection(180);
                        break;
                    }
                } else if(direction == 3){
                    if(getWorld()->validSquare(getX()/16, getY()/16-1)){
                        setDirection(0);
                        setWalkingDirection(270);
                        break;
                    }
                }
            }
        }
        justTeleported = false;
        int action = getWorld()->playerAction(this);
        if(action == ACTION_ROLL){
            ticks_to_move = 8;//randInt(1, 10)*8;
            waitingToRoll = false;
        } else{
            return;
        }
    }
    
    if(getWorld()->atFork(this)){
        int action = getWorld()->playerAction(this);
        if(action == ACTION_LEFT && getWalkingDirection() != 0 && getWorld()->validSquare(getX()/16-1, getY()/16)){
            setWalkingDirection(180);
            setDirection(180);
        } else if(action == ACTION_RIGHT && getWalkingDirection() != 180 && getWorld()->validSquare(getX()/16+1, getY()/16)){
            setWalkingDirection(0);
            setDirection(0);
        } else if(action == ACTION_UP && getWalkingDirection() != 270 && getWorld()->validSquare(getX()/16, getY()/16+1)){
            setWalkingDirection(90);
            setDirection(0);
        } else if(action == ACTION_DOWN && getWalkingDirection() != 90 && getWorld()->validSquare(getX()/16, getY()/16-1)){
            setWalkingDirection(270);
            setDirection(0);
        } else{
            return;
        }
        
    }
    
    int xpos = 0, ypos = 0, direction; // Get coords of square player is walking towards
    direction = getWalkingDirection();
    if(direction == 0){
        xpos = getX()/16+1;
        ypos = getY()/16;
    } else if (direction == 90){
        xpos = getX()/16;
        ypos = getY()/16+1;
    } else if(direction == 180){
        xpos = ceil(1.0*getX()/16)-1;
        ypos = ceil(1.0*getY()/16);
    } else if(direction == 270){
        xpos = ceil(1.0*getX()/16);
        ypos = ceil(1.0*getY()/16)-1;
    }
    
    if(!(getWorld()->validSquare(xpos, ypos))){ // Change direction if square walking towards isn't valid
        xpos = getX()/16;
        ypos = getY()/16;
        if(direction == 0 || direction == 180){
            if(getWorld()->validSquare(xpos, ypos+1)){ // Move up
                setWalkingDirection(90);
                setDirection(0);
            } else{ // Move down
                setWalkingDirection(270);
                setDirection(0);
            }
        } else{
            if(getWorld()->validSquare(xpos+1, ypos)){ // Move right
                setWalkingDirection(0);
                setDirection(0);
            } else{
                setWalkingDirection(180); // Move left
                setDirection(180);
            }
        }
    }
    
    direction = getWalkingDirection(); // Move in direction
    if(direction == 0){ // right
        //moveAtAngle(right, 2);
        moveTo(getX()+2, getY());
    } else if(direction == 90){ // up
        //moveAtAngle(up, 2);
        moveTo(getX(), getY()+2);
    } else if (direction == 180){ // left
        //moveAtAngle(left, 2);
        moveTo(getX()-2, getY());
    } else if(direction == 270){ // down
        //moveAtAngle(down, 2);
        moveTo(getX(), getY()-2);
    }
    ticks_to_move--;
    if(ticks_to_move == 0){
        waitingToRoll = true;
    }
}

int Player::changeCoins(int amount){
    if(m_coins + amount < 0){
        m_coins = 0;
        return -m_coins;
    }
    m_coins += amount;
    return amount;
}

int Player::changeStars(int amount){
    if(m_stars + amount < 0){
        m_stars = 0;
        return -m_stars;
    }
    m_stars += amount;
    return amount;
}


// Player End


// Square Begin

Square::Square(int imageID, int x, int y, StudentWorld* world, int dir)
: Actor(imageID, x, y, dir, 1, world, 0) // ID, x ,y, sprite direction, depth, worldpointer
{
}

void Square::trackPlayer(int player){
    players.insert(player);
}


void Square::removePlayer(int player){
    players.erase(player);
}


bool Square::containsPlayer(int player){
    return players.count(player) == 1;
}

// Square End



// Coin Square Begin

CoinSquare::CoinSquare(int imageID, int x, int y, StudentWorld* world, int coin)
:Square(imageID, x, y, world, 0)
{
    coinAmount = coin;
}


void CoinSquare::doSomething(){ // Peach== 1 Yoshi == 2
    if(!isAlive()){
        return;
    }
    if(getWorld()->doesIntersect(this, 1) && !containsPlayer(1) && !(getWorld()->playerMoving(1))){
        getWorld()->changeCoins(coinAmount, 1);
        trackPlayer(1);
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    if(!(getWorld()->doesIntersect(this, 1)) && containsPlayer(1)){
        removePlayer(1);
    }
    
    if(getWorld()->doesIntersect(this, 2) && !containsPlayer(2) && !(getWorld()->playerMoving(2))){
        getWorld()->changeCoins(coinAmount,2);
        trackPlayer(2);
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    if(!(getWorld()->doesIntersect(this, 2)) && containsPlayer(2)){
        removePlayer(2);
    }
}
// Coin Square End


// Directional Square Start

DirectionalSquare::DirectionalSquare(int imageID, int x, int y, StudentWorld* world, int dir)
:Square(imageID, x, y, world, dir)
{
    direction = dir;
}

void DirectionalSquare::doSomething(){
    if(getWorld()->doesIntersect(this, 1)){
        getWorld()->changePlayerDirection(1, direction);
    }
    if(getWorld()->doesIntersect(this, 2)){
        getWorld()->changePlayerDirection(2, direction);
    }
}

// Directional Square End



// Event Square Begin
EventSquare::EventSquare(int imageID, int x, int y, StudentWorld* world)
: Square(imageID, x, y, world, 0)
{
}


void EventSquare::doSomething(){
    if(getWorld()->doesIntersect(this, 1) && !containsPlayer(1) && !getWorld()->playerMoving(1)){
        int event = 1;//randInt(1, 3);
        if(event == 1){
            getWorld()->teleportPlayer(1);
            trackPlayer(1);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (event == 2){
            getWorld()->swapPlayers();
            trackPlayer(2);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (event == 3){
            trackPlayer(1);
            getWorld()->giveVortex(1);
            getWorld()->playSound(SOUND_GIVE_VORTEX);
        }
    }
    if(!getWorld()->doesIntersect(this, 1) && containsPlayer(1)){
        removePlayer(1);
    }
    
    if(getWorld()->doesIntersect(this, 2) && !containsPlayer(2) && !getWorld()->playerMoving(2)){
        int event = 1;//randInt(1, 3);
        if(event == 1){
            getWorld()->teleportPlayer(2);
            trackPlayer(2);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (event == 2){
            getWorld()->swapPlayers();
            trackPlayer(1);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        } else if (event == 3){
            trackPlayer(2);
            getWorld()->giveVortex(2);
            getWorld()->playSound(SOUND_GIVE_VORTEX);
        }
    }
    if(!getWorld()->doesIntersect(this, 2) && containsPlayer(2)){
        removePlayer(2);
    }
}
// Event Square End



// Bank Square Start


BankSquare::BankSquare(int imageID, int x, int y, StudentWorld* world)
: Square(imageID, x, y, world, 0)
{
}

void BankSquare::doSomething(){
    if(getWorld()->doesIntersect(this, 1) && !containsPlayer(1)){ // Intersects with peach
        if(getWorld()->playerMoving(1)){ // If moving, deduct 5 coins
            getWorld()->depositInBank(1);
        } else{
            getWorld()->withDrawFromBank(1); // If not moving, empty bank, then track
            trackPlayer(1);
        }
    } else if(!getWorld()->doesIntersect(this, 1) && containsPlayer(1)){
        removePlayer(1); // If doesn't intersect and tracking, stop tracking
    }
    
    if(getWorld()->doesIntersect(this, 2) && !containsPlayer(2)){ // Intersects with peach
        if(getWorld()->playerMoving(2)){ // If moving, deduct 5 coins
            getWorld()->depositInBank(2);
        } else{
            getWorld()->withDrawFromBank(2); // If not moving, empty bank, then track
            trackPlayer(2);
        }
    } else if(!getWorld()->doesIntersect(this, 2) && containsPlayer(2)){
        removePlayer(2); // If doesn't intersect and tracking, stop tracking
    }
}

// Bank Square End



// Star Square Start

StarSquare::StarSquare(int imageID, int x, int y, StudentWorld* world)
:Square(imageID, x, y, world, 0)
{
}


void StarSquare::doSomething(){
    if(getWorld()->doesIntersect(this, 1) && !containsPlayer(1) && !getWorld()->playerMoving(1)){
        trackPlayer(1);
        getWorld()->addStar(1);
    }
    if(!getWorld()->doesIntersect(this, 1) && containsPlayer(1)){
        removePlayer(1);
    }
    
    if(getWorld()->doesIntersect(this, 2) && !containsPlayer(2) && !getWorld()->playerMoving(2)){
        trackPlayer(2);
        getWorld()->addStar(2);
    }
    if(!getWorld()->doesIntersect(this, 2) && containsPlayer(2)){
        removePlayer(2);
    }
}
// Star Square End
