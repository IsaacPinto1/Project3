#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <string>
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
                Board::GridEntry ge = bd.getContentsOf(i, j); // x=5, y=10
                string key = to_string(i) + "," + to_string(j);
                validCoords.insert(key);
                if(ge == Board::blue_coin_square){
                    actors.push_back(new CoinSquare(2, i, j, this, 3));
                    cout << "Blue coin square" << endl;
                } else if (ge == Board::red_coin_square){
                    cout << "Red coin square" << endl;
                } else if (ge == Board::star_square){
                    cout << "Star Square" << endl;
                } else if (ge == Board::event_square){
                    cout << "Event Square" << endl;
                } else if (ge == Board::bank_square){
                    cout << "Bank Square" << endl;
                } else if (ge == Board::player){
                    peach = new Player(0, i, j, this);
                    yoshi = new Player(1, i, j, this);
                    actors.push_back(new CoinSquare(2, i, j, this, 3));
                } else if (ge == Board::bowser){
                    cout << "Bowser/blue coin" << endl;
                } else if (ge == Board::boo){
                    cout << "Boo/blue coin" << endl;
                } else if (ge == Board::right_dir_square){
                    cout << "Directional right" << endl;
                } else if (ge == Board::left_dir_square){
                    cout << "Directional left" << endl;
                } else if (ge == Board::up_dir_square){
                    cout << "Directional up" << endl;
                } else if (ge == Board::down_dir_square){
                    cout << "Directional down" << endl;
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



bool StudentWorld::validSquare(int x, int y){
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

