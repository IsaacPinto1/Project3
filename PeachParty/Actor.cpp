#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cmath>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


// Actor Begin
Actor::Actor(int imageID, int startX, int startY, int dir, int depth, StudentWorld* world)
: GraphObject(imageID, startX, startY, dir, depth, 1.0)
{
    m_world = world;
    m_isAlive = true;
}


// Actor End


// Mover Begin

Mover::Mover(int imageID, int x, int y, int dir, int depth, StudentWorld* world, int walkDir, bool waiting, int ticks)
:Actor(imageID, x, y, dir, depth, world)
{
    waitingToMove = waiting;
    ticks_to_move = ticks;
    walkingDir = walkDir;
}


void Mover::startMoving(){
    ticks_to_move = randInt(1, 10)*8;
    waitingToMove = false;
}

bool Mover::checkDirection(){ // Makes sure there is valid square ahead of player
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
    return getWorld()->validSquare(xpos, ypos);
}

void Mover::fixDirection(){ // Changes direction if at turning point (not a fork)
    int xpos, ypos, direction;
    direction = getWalkingDirection();
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

void Mover::walk(){
    int direction = getWalkingDirection(); // Move in direction
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
    changeTicks(-1);
    if(ticks_to_move == 0){
        stopWalking();
    }
}

void Mover::newRandomDirection(){
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

// Mover End


// Player Begin
Player::Player(int playerID, int x, int y, StudentWorld* world)
:Mover(playerID, x, y, 0, 0, world, 0, true, 0)
{
    hasVortex = false;
    m_stars = 0;
    m_coins = 0;
}

Player::Player(Player& position, Player& stats, int player)
:Mover(player-1, position.getX(), position.getY(), position.getDirection(), 0, position.getWorld(), position.getWalkingDirection(), !position.isMoving(), position.getTicksToMove())
{
    hasVortex = stats.hasAVortex();
    m_stars = stats.getStars();
    m_coins = stats.getCoins();
}


void Player::doSomething(){
    if(!isMoving()){
        if(getWalkingDirection() == -1){
            newRandomDirection();
        }
        int action = getWorld()->playerAction(this);
        if(action == ACTION_ROLL){
            startMoving();
        } else if(action == ACTION_FIRE && hasVortex){
            getWorld()->createVortex(getX(), getY(), getWalkingDirection());
            getWorld()->playSound(SOUND_PLAYER_FIRE);
            hasVortex = false;
            return;
        } else{
            return;
        }
    }
    
    if(!checkFork()){
        return;
    }
    
    if(!checkDirection()){
        fixDirection();
    }
    walk();
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


bool Player::checkFork(){
    if(!getWorld()->atFork(this)){
        return true;
    }
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
        return false;
    }
    return true;
}


void Player::setCoins(int amount){
    if(amount < 0){
        return;
    }
    m_coins = amount;
}

void Player::setStars(int amount){
    if(amount < 0){
        return;
    }
    m_stars = amount;
}


// Player End

// Enemy Begin

Enemy::Enemy(int ImageID, int x, int y, StudentWorld* world)
:Mover(ImageID, x, y, 0, 0, world, 0, true, 0)
{
    pauseCounter = 180;
}

bool Enemy::shouldInteract(int player){
    if(getWorld()->doesIntersect(this, player) && !containsPlayer(player) && !getWorld()->playerMoving(player)){
        trackPlayer(player);
        return true;
    }
    if(!getWorld()->doesIntersect(this, player) && containsPlayer(player)){
        removePlayer(player);
    }
    return false;
}


void Enemy::getVortexed(){
    getWorld()->teleportMover(this);
    setDirection(0);
    setWalkingDirection(0);
    stopWalking();
}




bool Enemy::checkFork(){
    if(!getWorld()->atFork(this)){
        return true;
    }
    newRandomDirection();
    return true;
}

void Enemy::stopWalking(){
    setWaitingStatus(true);
    setPauseCounter(180);
}

// Enemy End

// Boo Begin

Boo::Boo(int ImageID, int x, int y, StudentWorld* world)
: Enemy(ImageID, x, y, world)
{
}

void Boo::doSomething(){
    if(!isMoving()){
        for(int i = 1; i <= 2; i++){
            if(shouldInteract(i)){
                Interact();
            }
        }
        changePauseCounter(-1);
        if(getPauseCounter() == 0){
            startMoving();
        } else{
            return;
        }
    }
    clearPlayers();
    checkFork();
    
    if(!checkDirection()){
        fixDirection();
    }
    walk();
}

void Boo::startMoving(){
    setTicks(randInt(1, 3)*8);
    newRandomDirection();
    setWaitingStatus(false);
}

void Boo::Interact(){
    int action = randInt(1, 2); // 1: Swap coins, 2: Swap Stars
    if(action == 1){
        getWorld()->swapCoins();
    } else{
        getWorld()->swapStars();
    }
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

// Boo end


// Bowser Begin

Bowser::Bowser(int ImageID, int x, int y, StudentWorld* world)
: Enemy(ImageID, x, y, world)
{
}

void Bowser::doSomething(){
    if(!isMoving()){
        for(int i = 1; i <= 2; i++){
            if(shouldInteract(i)){
                Interact(i);
            }
        }
        changePauseCounter(-1);
        if(getPauseCounter() == 0){
            startMoving();
            newRandomDirection();
        } else{
            return;
        }
    }
    clearPlayers();
    checkFork();
    if(!checkDirection()){
        fixDirection();
    }
    walk();
}

void Bowser::stopWalking(){
    setWaitingStatus(true);
    setPauseCounter(180);
    int action = randInt(1, 4); // 1: dropping square, else:nothing
    if(action == 1){
        getWorld()->dropSquare(getX(), getY());
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}


void Bowser::Interact(int player){
    int action = randInt(1, 2); // 1: activate 2: do nothing
    if(action == 1){
        getWorld()->robPlayer(player);
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

// Bowser End




// Vortex Begin


Vortex::Vortex(int ImageID, int x, int y, StudentWorld* world, int walkDir)
: Mover(ImageID, x, y, 0, 0, world, walkDir, false, 0)
{
}


void Vortex::doSomething(){
    if(!isAlive()){
        return;
    }
    walk();
    if(getX() >= VIEW_WIDTH || getY() >= VIEW_WIDTH){
        kill();
    }
    if(getWorld()->didHit(this)){
        kill();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}

void Vortex::walk(){
    int direction = getWalkingDirection(); // Move in direction
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
}

// Vortex End



// Square Begin

Square::Square(int imageID, int x, int y, StudentWorld* world, int dir)
: Actor(imageID, x, y, dir, 1, world) // ID, x ,y, sprite direction, depth, worldpointer
{
}


bool Square::newPlayerLanded(int player){
    return getWorld()->doesIntersect(this, player) && !containsPlayer(player) && !(getWorld()->playerMoving(player));
}

void Square::removeTrackIfGone(int player){
    if(!(getWorld()->doesIntersect(this, player)) && containsPlayer(player)){
        removePlayer(player);
    }
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
    for(int i = 1; i <=2 ; i++){
        if(newPlayerLanded(i)){
            getWorld()->changeCoins(coinAmount, i);
            trackPlayer(i);
            if(coinAmount == 3){
                getWorld()->playSound(SOUND_GIVE_COIN);
            } else{
                getWorld()->playSound(SOUND_TAKE_COIN);
            }
        }
        removeTrackIfGone(i);
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
    for(int i = 1; i <= 2; i++){
        if(getWorld()->doesIntersect(this, i)){
            getWorld()->changePlayerDirection(i, direction);
        }
    }
}

// Directional Square End



// Event Square Begin
EventSquare::EventSquare(int imageID, int x, int y, StudentWorld* world)
: Square(imageID, x, y, world, 0)
{
}


void EventSquare::doSomething(){
    for(int i = 1; i <= 2; i++){
        if(newPlayerLanded(i)){
            int event = randInt(1, 3);
            if(event == 1){
                getWorld()->teleportMover(i);
                getWorld()->invalidateMovement(i);
                trackPlayer(i);
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            } else if (event == 2){
                getWorld()->swapPlayers();
                if(i == 1){
                    trackPlayer(2);
                } else{
                    trackPlayer(1);
                }
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            } else if (event == 3){
                trackPlayer(i);
                getWorld()->giveVortex(i);
                getWorld()->playSound(SOUND_GIVE_VORTEX);
            }
        }
        removeTrackIfGone(i);
    }
}
// Event Square End



// Bank Square Start


BankSquare::BankSquare(int imageID, int x, int y, StudentWorld* world)
: Square(imageID, x, y, world, 0)
{
}

void BankSquare::doSomething(){
    for(int i = 1; i <= 2; i++){
        if(getWorld()->doesIntersect(this, i) && !containsPlayer(i)){
            if(getWorld()->playerMoving(i)){ // If moving, deduct 5 coins
                getWorld()->depositInBank(i);
            } else{
                getWorld()->withDrawFromBank(i); // If not moving, empty bank, then track
                trackPlayer(i);
            }
        }
        removeTrackIfGone(i);
    }
}

// Bank Square End



// Star Square Start

StarSquare::StarSquare(int imageID, int x, int y, StudentWorld* world)
:Square(imageID, x, y, world, 0)
{
}


void StarSquare::doSomething(){
    for(int i = 1; i <= 2; i++){
        if(getWorld()->doesIntersect(this, i) && !containsPlayer(i)){
            trackPlayer(i);
            getWorld()->addStar(i);
        }
        removeTrackIfGone(i);
    }
}
// Star Square End



// DroppingSquare Begin

DroppingSquare::DroppingSquare(int imageID, int x, int y, StudentWorld* world)
:Square(imageID, x, y, world, 0)
{
}

void DroppingSquare::doSomething(){
    for(int i = 1; i <= 2; i++){
        if(newPlayerLanded(i)){
            int action = randInt(1, 2); // 1: Take 10 coins, 2: Take 1 star
            if(action == 1){
                getWorld()->changeCoins(-10, i);
            }
            if(action == 2){
                getWorld()->changeStars(-1, i);
            }
            trackPlayer(i);
            getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        }
        removeTrackIfGone(i);
    }
}

// DroppingSquare End
