#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    peach = nullptr;
    yoshi = nullptr;
    bank = 0;
}

StudentWorld::~StudentWorld(){
    delete yoshi;
    delete peach;
    cleanUp();
}

int StudentWorld::init()
{
    Board bd;
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    
    Board::LoadResult result = bd.loadBoard(board_file);
    
    if (result == Board::load_fail_file_not_found){
        cerr << "Could not find board01.txt data file\n";
        return GWSTATUS_BOARD_ERROR;
    } else if (result == Board::load_fail_bad_format){
        cerr << "Your board was improperly formatted\n";
        return GWSTATUS_BOARD_ERROR;
    } else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        for(int i = 0; i < 16; i++){
            for(int j = 0; j < 16; j++){
                Board::GridEntry ge = bd.getContentsOf(i, j);
                string key = to_string(i) + "," + to_string(j);
                validCoords.insert(key);
                if(ge == Board::blue_coin_square){
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3));
                } else if (ge == Board::red_coin_square){
                    actors.push_back(new CoinSquare(IID_RED_COIN_SQUARE, i*16, j*16, this, -3));
                } else if (ge == Board::star_square){
                    actors.push_back(new StarSquare(IID_STAR_SQUARE, i*16, j*16, this));
                } else if (ge == Board::event_square){
                    actors.push_back(new EventSquare(IID_EVENT_SQUARE, i*16, j*16, this));
                } else if (ge == Board::bank_square){
                    actors.push_back(new BankSquare(IID_BANK_SQUARE, i*16, j*16, this));
                } else if (ge == Board::player){
                    peach = new Player(IID_PEACH, i*16, j*16, this); // Peach
                    yoshi = new Player(IID_YOSHI, i*16, j*16, this); // Yoshi
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3));
                } else if (ge == Board::bowser){
                    actors.push_back(new Bowser(IID_BOWSER, i*16, j*16, this));
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3));
                } else if (ge == Board::boo){
                    actors.push_back(new Boo(IID_BOO, i*16, j*16, this));
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3));
                } else if (ge == Board::right_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i*16, j*16, this, 0));
                } else if (ge == Board::left_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i*16, j*16, this, 180));
                } else if (ge == Board::up_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i*16, j*16, this, 90));
                } else if (ge == Board::down_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i*16, j*16, this, 270));
                } else{
                    validCoords.erase(key);
                }
                
            }
        }
    }
	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // The term "actors" refers to all actors, e.g., Peach, Yoshi,
    // baddies, squares, vortexes, etc.
    // Give each actor a chance to do something, incl. Peach and Yoshi
    peach->doSomething();
    yoshi->doSomething();
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->isAlive()){
            actors[i]->doSomething();
        }
    }
    // Remove newly-inactive actors after each tick
    int i = 0;
    while(i < actors.size()){
        if(actors[i]->isAlive()){
            i++;
        } else{
            delete actors[i];
            actors.erase(actors.begin()+i);
        }
    }
    // Update the Game Status Line
    string text = "P1 Roll: ";
    text.append(to_string(peach->getRoll()));
    text.append(" Stars: ");
    text.append(to_string(peach->getStars()));
    text.append(" $$: ");
    text.append(to_string(peach->getCoins()));
    if(peach->hasAVortex()){
        text.append(" VOR");
    }
    text.append(" | Time: ");
    text.append(to_string(timeRemaining()));
    text.append(" | Bank: ");
    text.append(to_string(getBank()));
    text.append(" | P2 Roll: ");
    text.append(to_string(yoshi->getRoll()));
    text.append(" Stars: ");
    text.append(to_string(yoshi->getStars()));
    text.append(" $$: ");
    text.append(to_string(yoshi->getCoins()));
    if(yoshi->hasAVortex()){
        text.append(" VOR ");
    }
    
    setGameStatText(text); // update the coins/stars stats text at screen top
    if (timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
        
        if (yoshi->getStars() > peach->getStars())
        {
            setFinalScore(yoshi->getStars(), yoshi->getCoins()); // (yoshiStars, yoshiCoins)
            return GWSTATUS_YOSHI_WON;
        }else if(peach->getStars() > yoshi->getStars()){ // if(peach won)
            setFinalScore(peach->getStars(), peach->getCoins()); // (peachStars, peachCoins)
            return GWSTATUS_PEACH_WON;
        } else {
            if(yoshi->getCoins() > peach->getCoins()){
                setFinalScore(yoshi->getStars(), yoshi->getCoins()); // (yoshiStars, yoshiCoins)
                return GWSTATUS_YOSHI_WON;
            } else if(peach->getCoins() > yoshi -> getCoins()){
                setFinalScore(peach->getStars(), peach->getCoins()); // (peachStars, peachCoins)
                return GWSTATUS_PEACH_WON;
            } else {
                int winner = randInt(0,1);
                if(winner == 0){
                    setFinalScore(peach->getStars(), peach->getCoins()); // (peachStars, peachCoins)
                    return GWSTATUS_PEACH_WON;
                } else {
                    setFinalScore(yoshi->getStars(), yoshi->getCoins()); // (yoshiStars, yoshiCoins)
                    return GWSTATUS_YOSHI_WON;
                }
            }
        }
     }
    // the game isn't over yet so continue playing
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    while(actors.size() > 0){
        delete actors[0];
        actors.erase(actors.begin());
    }
}


int StudentWorld::playerAction(Actor *player){
    if(player == peach){
        return getAction(1);
    } else{
        return getAction(2);
    }
}



bool StudentWorld::validSquare(int x, int y){ // Takes in 0-15 coords
    string key = to_string(x) + "," + to_string(y);
    if(validCoords.count(key) == 1){
        return true;
    }
    return false;
}


bool StudentWorld::doesIntersect(Actor *a1, Actor *a2){
    if(a1->getX()==a2->getX() && a1->getY() == a2->getY()){
        return true;
    }
    return false;
}

bool StudentWorld::doesIntersect(Actor *a1, int player){
    if(player == 1){
        return doesIntersect(a1, peach);
    } else{
        return doesIntersect(a1, yoshi);
    }
}

bool StudentWorld::doesOverlap(Actor *a1, Actor *a2){
    int x1, y1, x2, y2;
    x1 = a1->getX();
    y1 = a1->getY();
    x2 = a2->getX();
    y2 = a2->getY();
    if (x1 <= x2 + 15 && x1 + 15 >= x2 && y1 <= y2 + 15 && y1 + 15 >= y2) {
        return true;
    }
    return false;
}

void StudentWorld::changeCoins(int amount, int player){
    if(player == 1){
        peach->changeCoins(amount);
    } else{
        yoshi->changeCoins(amount);
    }
}

void StudentWorld::changeStars(int amount, int player){
    if(player == 1){
        peach->changeStars(amount);
    } else{
        yoshi->changeStars(amount);
    }
}

bool StudentWorld::playerMoving(int player){
    if(player == 1){
        return peach->isMoving();
    } else{
        return yoshi->isMoving();
    }
}

void StudentWorld::changePlayerDirection(int player, int direction){
    if(player == 1){
        if(direction == 180){
            peach->setDirection(180);
        } else{
            peach->setDirection(0);
        }
        peach->setWalkingDirection(direction);
    } else{
        if(direction == 180){
            yoshi->setDirection(180);
        } else{
            yoshi->setDirection(0);
        }
        yoshi->setWalkingDirection(direction);
    }
}

bool StudentWorld::atFork(Actor* player){
    for(int i = 0; i < actors.size(); i++){
        if(doesIntersect(actors[i], player) && !actors[i]->changesDirection() && !actors[i]->doesMove() && countValidAdjacent(actors[i]) > 2){
            return true;
        }
    }
    return false;
}


int StudentWorld::countValidAdjacent(Actor *square){
    int count = 0;
    int x = square->getX()/16;
    int y = square->getY()/16;
    if(validSquare(x+1, y)){
        count++;
    }
    if(validSquare(x-1, y)){
        count++;
    }
    if(validSquare(x, y+1)){
        count++;
    }
    if(validSquare(x, y-1)){
        count++;
    }
    return count;
}


void StudentWorld::depositInBank(int player){
    if(player == 1){
        bank += -peach->changeCoins(-5);
        playSound(SOUND_DEPOSIT_BANK);
    } else{
        bank += -yoshi->changeCoins(-5);
        playSound(SOUND_DEPOSIT_BANK);
    }
}

void StudentWorld::withDrawFromBank(int player){
    if(player == 1){
        peach->changeCoins(bank);
        bank = 0;
        playSound(SOUND_WITHDRAW_BANK);
    } else{
        yoshi->changeCoins(bank);
        bank = 0;
        playSound(SOUND_WITHDRAW_BANK);
    }
}

void StudentWorld::addStar(int player){
    if(player == 1){
        if(peach->getCoins() < 20){
            return;
        }
        peach->changeCoins(-20);
        peach->changeStars(1);
        playSound(SOUND_GIVE_STAR);
    }
    
    if(player == 2){
        if(yoshi->getCoins() < 20){
            return;
        }
        yoshi->changeCoins(-20);
        yoshi->changeStars(1);
        playSound(SOUND_GIVE_STAR);
    }
}

void StudentWorld::giveVortex(int player){
    if(player == 1){
        peach->giveVortex();
    } else{
        yoshi->giveVortex();
    }
}


void StudentWorld::swapPlayers(){
    Player* newPeach = new Player(*yoshi, *peach, 1);
    Player* newYoshi = new Player(*peach, *yoshi, 2);
    delete peach;
    peach = newPeach;
    delete yoshi;
    yoshi = newYoshi;
}


void StudentWorld::getIntsFromCoord(std::string str, int& x, int&y){
    stringstream ss(str);
    string token;

    getline(ss, token, ',');
    x = stoi(token);

    getline(ss, token);
    y = stoi(token);
}

void StudentWorld::teleportMover(int player){
    if(player == 1){
        teleportMover(peach);
    } else{
        teleportMover(yoshi);
    }
}

void StudentWorld::teleportMover(Actor* p){
    int i = randInt(0, int(validCoords.size()-1));
    int x; int y;
    set<string>::iterator p1 = validCoords.begin();
    advance(p1, i);
    getIntsFromCoord(*p1, x, y);
    p->moveTo(x*16,y*16);
    p->moveTo(x*16,y*16);
}

void StudentWorld::swapCoins(){
    int peachCoins = peach->getCoins();
    int yoshiCoins = yoshi->getCoins();
    yoshi->setCoins(peachCoins);
    peach->setCoins(yoshiCoins);
}

void StudentWorld::swapStars(){
    int peachStars = peach->getStars();
    int yoshiStars = yoshi->getStars();
    yoshi->setStars(peachStars);
    peach->setStars(yoshiStars);
}

void StudentWorld::robPlayer(int player){
    if(player == 1){
        peach->setCoins(0);
        peach->setStars(0);
    } else if (player == 2){
        yoshi->setCoins(0);
        yoshi->setStars(0);
    }
}


void StudentWorld::dropSquare(int x, int y){
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y && !actors[i]->doesMove()){
            actors[i]->kill();
            actors.push_back(new DroppingSquare(IID_DROPPING_SQUARE, x, y, this));
            return;
        }
    }
}


void StudentWorld::createVortex(int x, int y, int walkDir){
    actors.push_back(new Vortex(IID_VORTEX, x, y, this, walkDir));
}

bool StudentWorld::didHit(Actor *v){
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->impactable() && doesOverlap(actors[i], v)){
            actors[i]->getVortexed();
            return true;
        }
    }
    return false;
}

void StudentWorld::invalidateMovement(int player){
    if(player == 1){
        peach->setWalkingDirection(-1);
    } else{
        yoshi->setWalkingDirection(-1);
    }
}
