#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cmath>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


// Actor Begin
Actor::Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world)
: GraphObject(imageID, startX*16, startY*16, 0, depth, 1.0)
{
    walkingDir = 0;
    m_world = world;
    m_isAlive = true;
}

Actor::~Actor(){}

// Actor End


// Player Begin
Player::Player(int player, int x, int y, StudentWorld* world)
:Actor(player, x, y, 0, 0, world)
{
    playerNumb = player+1;
    waitingToRoll = true;
    ticks_to_move = 0;
    hasVortex = false;
    m_stars = 0;
    m_coins = 0;
}

Player::~Player(){}

void Player::doSomething(){
    if(waitingToRoll){
        int action = getWorld()->getAction(playerNumb);
        if(action == ACTION_ROLL){
            ticks_to_move = randInt(1, 10)*8;
            waitingToRoll = false;
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

void Player::changeCoins(int amount){
    m_coins += amount;
    if(m_coins < 0){
        m_coins = 0;
    }
}


// Player End


// Square Begin

Square::Square(int imageID, int x, int y, StudentWorld* world)
: Actor(imageID, x, y, 0, 1, world)
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
:Square(imageID, x, y, world)
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
    if(getWorld()->doesIntersect(this, 2) && !containsPlayer(2) && !(getWorld()->playerMoving(2))){
        getWorld()->changeCoins(coinAmount,2);
        trackPlayer(2);
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    if(!(getWorld()->doesIntersect(this, 1)) && containsPlayer(1)){
        removePlayer(1);
    }
    if(!(getWorld()->doesIntersect(this, 2)) && containsPlayer(2)){
        removePlayer(2);
    }
}
// Coin Square End
