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
                    actors.push_back(new CoinSquare(2, i, j, this));
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
                    actors.push_back(new CoinSquare(2, i, j, this));
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
    setGameStatText("P1 Roll: 3 Stars: 2 $$: 15 | Time: 75 | Bank: 9"); // update the coins/stars stats text at screen top
    if (timeRemaining() <= 0)
    {
        playSound(14);
        //if(yoshi won)
        if (true)
        {
            setFinalScore(1, 2); // (yoshiStars, yoshiCoins)
            return GWSTATUS_YOSHI_WON;
        }else{ // if(peach won)
            setFinalScore(3, 4); // (peachStars, peachCoins)
            return GWSTATUS_PEACH_WON;
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


StudentWorld::~StudentWorld(){
    delete yoshi;
    delete peach;
    cleanUp();
}
