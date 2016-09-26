// robots.cpp

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <cassert>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;             // max number of rows in the arena
const int MAXCOLS = 20;             // max number of columns in the arena
const int MAXROBOTS = 100;          // max number of robots allowed
const int MAXCHANNELS = 3;          // max number of channels
const double WALL_DENSITY = 0.12;   // density of walls

const int NORTH = 0;
const int EAST  = 1;
const int SOUTH = 2;
const int WEST  = 3;
const int NUMDIRS = 4;

const int EMPTY = 0;
const int WALL  = 1;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
// type name, since it's mentioned in the Robot declaration.

class Robot
{
public:
    // Constructor
    Robot(Arena* ap, int r, int c, int channel);
    
    // Accessors
    int  row() const;
    int  col() const;
    int  channel() const;
    bool isDead() const;
    
    // Mutators
    void forceMove(int dir);
    void move();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_channel;
    int    m_damage;
};

class Player
{
public:
    // Constructor
    Player(Arena *ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    bool isDead() const;
    
    // Mutators
    string stand();
    string move(int dir);
    void   setDead();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    bool   m_dead;
};

class Arena
{
public:
    // Constructor/destructor
    Arena(int nRows, int nCols);
    ~Arena();
    
    // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     robotCount() const;
    int     getCellStatus(int r, int c) const;
    int     numberOfRobotsAt(int r, int c) const;
    void    display(string msg) const;
    
    // Mutators
    void   setCellStatus(int r, int c, int status);
    bool   addRobot(int r, int c, int channel);
    bool   addPlayer(int r, int c);
    string moveRobots(int channel, int dir);
    
private:
    int     m_grid[MAXROWS][MAXCOLS];
    int     m_rows;
    int     m_cols;
    Player* m_player;
    Robot*  m_robots[MAXROBOTS];
    int     m_nRobots;
    
    // Helper functions
    void checkPos(int r, int c) const;
};

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nRobots);
    ~Game();
    
    // Mutators
    void play();
    
private:
    Arena* m_arena;
    
    // Helper functions
    string takePlayerTurn();
    string takeRobotsTurn();
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int lowest, int highest);
bool charToDir(char ch, int& dir);
bool attemptMove(const Arena& a, int dir, int& r, int& c);
bool recommendMove(const Arena& a, int r, int c, int& bestDir);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Robot implementation
///////////////////////////////////////////////////////////////////////////

Robot::Robot(Arena* ap, int r, int c, int channel)
{
    if (ap == nullptr)
    {
        cout << "***** A robot must be created in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "***** Robot created with invalid coordinates (" << r << ","
        << c << ")!" << endl;
        exit(1);
    }
    if (channel < 1  ||  channel > MAXCHANNELS)
    {
        cout << "***** Robot created with invalid channel " << channel << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_channel = channel;
    m_damage = 0;
}

int Robot::row() const
{
    return m_row;
}

int Robot::col() const
{
    return m_col;
}

int Robot::channel() const
{
    return m_channel;
}

bool Robot::isDead() const
{
    return (m_damage >= 3);
}

void Robot::forceMove(int dir)
{
    if (! attemptMove(*m_arena, dir, m_row, m_col))
        m_damage++;
}

void Robot::move()
{
    // Attempt to move in a random direction; if we can't move, don't move
    if (!isDead())
        attemptMove(*m_arena, randInt(0, NUMDIRS-1), m_row, m_col);
}

///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** The player must be created in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
        << "," << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_dead = false;
}

int Player::row() const
{
    return m_row;
}

int Player::col() const
{
    return m_col;
}

string Player::stand()
{
    return "Player stands.";
}

string Player::move(int dir)
{
    if (! attemptMove(*m_arena, dir, m_row, m_col))
        return "Player couldn't move; player stands.";
    
    if (m_arena->numberOfRobotsAt(m_row, m_col) > 0)
    {
        setDead();
        return "Player walked into a robot and died.";
    }
        
    if (dir == NORTH)
    {
        return "Player moved north.";
    }
    if (dir == EAST)
    {
        return "Player moved east.";
    }
    if (dir == SOUTH)
    {
        return "Player moved south.";
    }
    else // direction is west
        return "Player moved west.";
}

bool Player::isDead() const
{
    return m_dead;
}

void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementation
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
    if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
    {
        cout << "***** Arena created with invalid size " << nRows << " by "
        << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nRobots = 0;
    for (int r = 1; r <= m_rows; r++)
        for (int c = 1; c <= m_cols; c++)
            setCellStatus(r, c, EMPTY);
}

Arena::~Arena()
{
    delete m_player;
    for (int i = 0; i < m_nRobots; i++)
        delete m_robots[i];
}

int Arena::rows() const
{
    return m_rows;
}

int Arena::cols() const
{
    return m_cols;
}

Player* Arena::player() const
{
    return m_player;
}

int Arena::robotCount() const
{
    return m_nRobots;
}

int Arena::getCellStatus(int r, int c) const
{
    checkPos(r, c);
    return m_grid[r-1][c-1];
}

int Arena::numberOfRobotsAt(int r, int c) const
{
    int nRobotsAt = 0;
    for (int i = 0; i < m_nRobots; i++)
        if (m_robots[i]->row() == r && m_robots[i]->col() == c)
            nRobotsAt++;
    return nRobotsAt;
}

void Arena::display(string msg) const
{
    char displayGrid[MAXROWS][MAXCOLS];
    int r, c;
    
    // Fill displayGrid with dots (empty) and stars (wall)
    for (r = 1; r <= rows(); r++)
        for (c = 1; c <= cols(); c++)
            displayGrid[r-1][c-1] = (getCellStatus(r,c) == EMPTY ? '.' : '*'); // ASK: what does colon operator do?
    
    // Indicate robot positions by their channels.  If more than one robot
    // occupies a cell, show just one (any one will do).

    int n = 0;
    for (r = 1; r <= rows(); r++)
    {
        for (c = 1; c <= cols(); c++)
        {
           if (numberOfRobotsAt(r, c) > 0)
            {
                char chan = m_robots[n]->channel() + 48;
                displayGrid[r-1][c-1] = chan;
                n += numberOfRobotsAt(r, c);
            }
        }
    }
    
    // Indicate player's position
    if (m_player != nullptr)
        displayGrid[m_player->row()-1][m_player->col()-1] = (m_player->isDead() ? 'X' : '@');
    
    // Draw the grid
    clearScreen();
    for (r = 1; r <= rows(); r++)
    {
        for (c = 1; c <= cols(); c++)
            cout << (displayGrid[r-1][c-1]);
        cout << endl;
    }
    cout << endl;
    
    // Write message, robot, and player info
    if (msg != "")
        cout << msg << endl;
    cout << "There are " << robotCount() << " robots remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player!" << endl;
    else if (m_player->isDead())
        cout << "The player is dead." << endl;
}

void Arena::setCellStatus(int r, int c, int status)
{
    checkPos(r, c);
    m_grid[r-1][c-1] = status;
}

bool Arena::addRobot(int r, int c, int channel)
{
    if (m_nRobots == MAXROBOTS)
        return false;
    m_robots[m_nRobots] = new Robot(this, r, c, channel);
    m_nRobots++;
    return true;
}

bool Arena::addPlayer(int r, int c)
{
    if (m_player != nullptr)
        return false;
    m_player = new Player(this, r, c);
    return true;
}

string Arena::moveRobots(int channel, int dir)
{
    // Robots on the channel will respond with probability 1/2
    bool willRespond = (randInt(0, 1) == 0);
    
    // Move all robots
    int nRobotsOriginally = m_nRobots;
    
    for (int i = 0; i < nRobotsOriginally; i++)
    {
        // Force robots listening on the channel to move in the indicated direction if willRespond is true.
        if (willRespond==true && m_robots[i]->channel() == channel)
        {
            m_robots[i]->forceMove(dir);
        }
        
        // If willRespond is false, or if the robot listens on a different channel, it just moves.
        else
            m_robots[i]->move();
        
        // Mark player as dead if robot hits it
        if (m_robots[i]->row() == m_player->row() && m_robots[i]->col() == m_player->col())
            m_player->setDead();
        
        // Release dead dynamically allocated robots
        if (m_robots[i]->isDead() == true)
        {
            delete m_robots[i];
            m_robots[i] = m_robots[m_nRobots-1];
            m_nRobots--;
        }
    }
    
    if (m_nRobots < nRobotsOriginally)
        return "Some robots have been destroyed.";
    else
        return "No robots were destroyed.";
}

void Arena::checkPos(int r, int c) const
{
    if (r < 1  ||  r > m_rows  ||  c < 1  ||  c > m_cols)
    {
        cout << "***** " << "Invalid arena position (" << r << ","
        << c << ")" << endl;
        exit(1);
    }
}

///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRobots)
{
    if (nRobots < 0  ||  nRobots > MAXROBOTS)
    {
        cout << "***** Game created with invalid number of robots:  "
        << nRobots << endl;
        exit(1);
    }
    int nEmpty = rows * cols - nRobots - 1;  // 1 for Player
    if (nEmpty < 0)
    {
        cout << "***** Game created with a " << rows << " by "
        << cols << " arena, which is too small too hold a player and "
        << nRobots << " robots!" << endl;
        exit(1);
    }
    
    // Create arena
    m_arena = new Arena(rows, cols);
    
    // Add some walls in WALL_DENSITY of the empty spots
    assert(WALL_DENSITY >= 0  &&  WALL_DENSITY <= 1);
    int nWalls = static_cast<int>(WALL_DENSITY * nEmpty);
    while (nWalls > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        if (m_arena->getCellStatus(r, c) == WALL)
            continue;
        m_arena->setCellStatus(r, c, WALL);
        nWalls--;
    }
    
    // Add player
    int rPlayer;
    int cPlayer;
    do
    {
        rPlayer = randInt(1, rows);
        cPlayer = randInt(1, cols);
    } while (m_arena->getCellStatus(rPlayer, cPlayer) != EMPTY);
    m_arena->addPlayer(rPlayer, cPlayer);
    
    // Populate with robots
    while (nRobots > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        if (m_arena->getCellStatus(r,c) != EMPTY  ||  (r == rPlayer && c == cPlayer))
            continue;
        m_arena->addRobot(r, c, randInt(1, MAXCHANNELS));
        nRobots--;
    }
}

Game::~Game()
{
    delete m_arena;
}

string Game::takePlayerTurn()
{
    for (;;)
    {
        cout << "Your move (n/e/s/w/x or nothing): ";
        string playerMove;
        getline(cin, playerMove);
        
        Player* player = m_arena->player();
        int dir;
        
        if (playerMove.size() == 0)
        {
            if (recommendMove(*m_arena, player->row(), player->col(), dir))
                return player->move(dir);
            else
                return player->stand();
        }
        else if (playerMove.size() == 1)
        {
            if (tolower(playerMove[0]) == 'x')
                return player->stand();
            else if (charToDir(playerMove[0], dir))
                return player->move(dir);
        }
        cout << "Player move must be nothing, or 1 character n/e/s/w/x." << endl;
    }
}

string Game::takeRobotsTurn()
{
    for (;;)
    {
        cout << "Broadcast (e.g., 2n): ";
        string broadcast;
        getline(cin, broadcast);
        if (broadcast.size() != 2)
            cout << "Broadcast must be channel followed by direction." << endl;
        else if (broadcast[0] < '1'  ||  broadcast[0] > '0'+MAXCHANNELS)
            cout << "Channel must be a valid digit." << endl;
        else
        {
            int dir;
            if (charToDir(broadcast[1], dir))
                return m_arena->moveRobots(broadcast[0]-'0', dir);
            else
                cout << "Direction must be n, e, s, or w." << endl;
        }
    }
}

void Game::play()
{
    m_arena->display("");
    while ( ! m_arena->player()->isDead()  &&  m_arena->robotCount() > 0)
    {
        string msg = takePlayerTurn();
        m_arena->display(msg);
        Player* player = m_arena->player();
        if (player->isDead())
            break;
        msg = takeRobotsTurn();
        m_arena->display(msg);
    }
    if (m_arena->player()->isDead())
        cout << "You lose." << endl;
    else
        cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

// Return a uniformly distributed random int from lowest to highest, inclusive
int randInt(int lowest, int highest)
{
    if (highest < lowest)
        swap(highest, lowest);
    return lowest + (rand() % (highest - lowest + 1));
}

bool charToDir(char ch, int& dir)
{
    switch (tolower(ch))
    {
        default:  return false;
        case 'n': dir = NORTH; break;
        case 'e': dir = EAST;  break;
        case 's': dir = SOUTH; break;
        case 'w': dir = WEST;  break;
    }
    return true;
}

// Return false without changing anything if moving one step from (r,c)
// in the indicated direction would hit a wall or run off the edge of the
// arena.  Otherwise, update r and c to the position resulting from the
// move and return true.

bool attemptMove(const Arena& a, int dir, int& r, int& c)
{
    switch (dir){
        case NORTH:
            if (r-1 < 1 || a.getCellStatus(r-1, c) == WALL)
                return false;
            r--;
            break;
        case EAST:
            if (c+1 > a.cols() || a.getCellStatus(r, c+1) == WALL)
                return false;
            c++;
            break;
        case SOUTH:
            if (r+1 > a.rows() || a.getCellStatus(r+1, c) == WALL)
                return false;
            r++;
            break;
        case WEST:
            if (c-1 < 1 || a.getCellStatus(r, c-1) == WALL)
                return false;
            c--;
            break;
        default:
            cerr << "invalid direction";
    }
    return true;
}

bool recommendMove(const Arena& a, int r, int c, int& bestDir)
{
    // where danger level is the number of robots surrounding player after given move
    // X means to stand
    int dangerLevelN = 0, dangerLevelE = 0, dangerLevelS = 0, dangerLevelW = 0, dangerLevelX = 0;
    
    /*
     ABC
     D@E
     FGH
     where @ is the player's current locations
     count how many robots will be directly adjacent to player after moving in each direction
     */
    
    int A = a.numberOfRobotsAt(r-1, c-1);
    int B = a.numberOfRobotsAt(r-1, c);
    int C = a.numberOfRobotsAt(r-1, c+1);
    int D = a.numberOfRobotsAt(r, c-1);
    int E = a.numberOfRobotsAt(r, c+1);
    int F = a.numberOfRobotsAt(r+1, c-1);
    int G = a.numberOfRobotsAt(r+1, c);
    int H = a.numberOfRobotsAt(r+1, c+1);
    
    
    dangerLevelN += (A + C + a.numberOfRobotsAt(r-2, c));
    // dangerLevelN += (a.numberOfRobotsAt(r-2, c-1) + a.numberOfRobotsAt(r-2, c) + a.numberOfRobotsAt(r-2, c+1));
    dangerLevelE += (C + H + a.numberOfRobotsAt(r, c+2));
    // dangerLevelE += (a.numberOfRobotsAt(r-1, c+2) + a.numberOfRobotsAt(r, c+2) + a.numberOfRobotsAt(r+1, c+2));
    dangerLevelS += (F + H + a.numberOfRobotsAt(r+2, c));
    // dangerLevelS += (a.numberOfRobotsAt(r+2, c-1) + a.numberOfRobotsAt(r+2, c) + a.numberOfRobotsAt(r+2, c+1));
    dangerLevelW += (A + F + a.numberOfRobotsAt(r, c-2));
    // dangerLevelW += (a.numberOfRobotsAt(r-1, c-2) + a.numberOfRobotsAt(r, c-2) + a.numberOfRobotsAt(r+1, c-2));
    dangerLevelX += (B + D + E + G);
    
    // danger level is highest for moves where player hits robot directly
    if (B > 0)
        dangerLevelN = 100;
    if (D > 0)
        dangerLevelW = 100;
    if (E > 0)
        dangerLevelE = 100;
    if (G > 0)
        dangerLevelS = 100;
    
    // can't recommend a move that would take player out of arena or into a wall
    if (r-1 == 0)
        dangerLevelN = 200;
    if (r == a.rows())
        dangerLevelS = 200;
    if (c-1 == 0)
        dangerLevelW = 200;
    if (c == a.cols())
        dangerLevelE = 200;
    
    // find direction with lowest danger level
    int tempDir = 4;
    int lowestDanger = 101;
    
    if (dangerLevelN < lowestDanger)
    {
        lowestDanger = dangerLevelN;
        tempDir = NORTH;
    }
    if (dangerLevelE < lowestDanger)
    {
        lowestDanger = dangerLevelE;
        tempDir = EAST;
    }
    if (dangerLevelS < lowestDanger)
    {
        lowestDanger = dangerLevelS;
        tempDir = SOUTH;
    }
    if (dangerLevelW < lowestDanger)
    {
        lowestDanger = dangerLevelW;
        tempDir = WEST;
    }
    if (dangerLevelX < lowestDanger)
    {
        return false;
    }

    bestDir = tempDir;
    return true;
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////


int main()
{
    // Initialize the random number generator
    srand(static_cast<unsigned int>(time(0)));
    
    // Create a game
    // Use this instead to create a mini-game:   Game g(3, 5, 2);
    Game g(10, 12, 50);
    
    // Play the game
    g.play();

}


///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                               &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
    static const char* term = getenv("TERM");
    if (term == nullptr  ||  strcmp(term, "dumb") == 0)
        cout << endl;
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
    }
}

#endif