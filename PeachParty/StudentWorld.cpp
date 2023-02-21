#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <algorithm>
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
    } else if (result == Board::load_fail_bad_format){
        cerr << "Your board was improperly formatted\n";
    } else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        for(int i = 0; i < 16; i++){
            for(int j = 0; j < 16; j++){
                Board::GridEntry ge = bd.getContentsOf(i, j);
                string key = to_string(i) + "," + to_string(j);
                validCoords.insert(key);
                if(ge == Board::blue_coin_square){
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i, j, this, 3));
                    cout << "Blue coin square" << endl;
                } else if (ge == Board::red_coin_square){
                    cout << "Red coin square" << endl;
                } else if (ge == Board::star_square){
                    cout << "Star Square" << endl;
                } else if (ge == Board::event_square){
                    actors.push_back(new EventSquare(IID_EVENT_SQUARE, i, j, this));
                } else if (ge == Board::bank_square){
                    actors.push_back(new BankSquare(IID_BANK_SQUARE, i, j, this));
                } else if (ge == Board::player){
                    peach = new Player(IID_PEACH, i, j, this); // Peach
                    yoshi = new Player(IID_YOSHI, i, j, this); // Yoshi
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i, j, this, 3));
                } else if (ge == Board::bowser){
                    cout << "Bowser/blue coin" << endl;
                } else if (ge == Board::boo){
                    cout << "Boo" << endl;
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, i, j, this, 3));
                } else if (ge == Board::right_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i, j, this, 0));
                    actors[actors.size()-1]->setDirection(0);
                } else if (ge == Board::left_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i, j, this, 180));
                    actors[actors.size()-1]->setDirection(180);
                } else if (ge == Board::up_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i, j, this, 90));
                    actors[actors.size()-1]->setDirection(90);
                } else if (ge == Board::down_dir_square){
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, i, j, this, 270));
                    actors[actors.size()-1]->setDirection(270);
                } else{
                    validCoords.erase(key);
                }
                
            }
        }
        for(set<string>::iterator p = validCoords.begin(); p != validCoords.end(); p++){
            cout << *p << endl;
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
        playSound(14);
        
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


bool StudentWorld::doesIntersect(Actor *a1, int player){
    Actor* a2;
    if(player == 1){
        a2 = peach;
    } else{
        a2 = yoshi;
    }
    if(a1->getX()==a2->getX() && a1->getY() == a2->getY()){
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
    if(player == peach){
        for(int i = 0; i < actors.size(); i++){
            if(doesIntersect(actors[i], 1) && !actors[i]->changesDirection() && countValidAdjacent(actors[i]) > 2){
                return true;
            }
        }
        return false;
    } else { // Yoshi
        for(int i = 0; i < actors.size(); i++){
            if(doesIntersect(actors[i], 2) && !(actors[i]->changesDirection()) && countValidAdjacent(actors[i]) > 2){
                return true;
            }
        }
        return false;
    }
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
