#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    //initialize member variables
    m_finishedLevel = false;
    m_numActors = 0;
    m_numTicks = 1;
    
    Level lev(assetPath());
    string levelFile = "level0"+to_string(getLevel())+".txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if(result == Level::load_fail_file_not_found || getLevel() == 99)
    {
        return GWSTATUS_PLAYER_WON;
    }
    else if(result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if(result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for(int x = 0; x < 16; x++)
        {
            for(int y = 0; y < 16; y++)
            {
                Level::MazeEntry ge = lev.getContentsOf(x, y);
                switch (ge) {
                    case Level::empty:
                        break;
                    case Level::player:
                        m_penelope = new Penelope(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT);
                        m_numActors++;
                        m_actors.push_back(m_penelope);
                        break;
                    case Level::wall:
                        createActor(new Wall(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::exit:
                        createActor(new Exit(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::pit:
                        createActor(new Pit(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::smart_zombie:
                        createActor(new SmartZombie(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::dumb_zombie:
                        createActor(new DumbZombie(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::landmine_goodie:
                        createActor(new LandmineGoodie(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::vaccine_goodie:
                        createActor(new VaccineGoodie(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::gas_can_goodie:
                        createActor(new GasCan(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                        break;
                    case Level::citizen:
                        createActor(new Citizen(this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                    default:
                        break;
                }
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    displayText();
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_penelope->getAliveState())
        {
            m_actors[i]->doSomething();
            if(m_finishedLevel)
            {
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        else
        {
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    //delete all dead actors
    for(int i = 0; i < m_numActors; i++)
    {
        if(!m_actors[i]->getAliveState())
        {
            delete m_actors[i];
            m_actors.erase(i+m_actors.begin());
            i--;
            m_numActors--;
        }
    }
    m_numTicks++;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for(vector<Actor*>::iterator p = m_actors.begin(); p!=m_actors.end();)
    {
        delete *p;
        p = m_actors.erase(p);
    }
    m_numActors = 0;
}

//check overlapping with X,Y coordinates
//return A_DO_NOTHING, which is 0, if none overlaps
int StudentWorld::getLivingThingOverlap(int x, int y){
    for(int i = 0; i < m_numActors; i++)
    {
        if((distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100))
        {
            if(m_actors[i]->whetherLivingThing())
                return m_actors[i]->getIntersect();
        }
    }
    return A_DO_NOTHING;
}

int StudentWorld::getGoodieOverlap(int x, int y){
    for(int i = 0; i < m_numActors; i++)
    {
        if((distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100))
        {
            if(m_actors[i]->getIntersect() == A_GOODIE)
                return m_actors[i]->getIntersect();
        }
    }
    return A_DO_NOTHING;
}

bool StudentWorld::ifBlock(int x, int y, int selfID, int currentX, int currentY){
    for(int i = 0; i < m_numActors; i++)
    {
        if(selfID == m_actors[i]->getIntersect() && currentX == m_actors[i]->getX() &&
           currentY == m_actors[i]->getY())
            continue;
        if(m_actors[i] != m_penelope)
        {
            if(!m_actors[i]->getBlockState()) //if actor does not block
                continue;
            if(((x <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x>= m_actors[i]->getX()) &&
               (y <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y>= m_actors[i]->getY())) ||
               ((x +SPRITE_WIDTH-1 <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x +SPRITE_WIDTH-1>= m_actors[i]->getX()) &&
                (y +SPRITE_HEIGHT-1 <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y +SPRITE_HEIGHT-1 >= m_actors[i]->getY())) ||
               ((x <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x >= m_actors[i]->getX()) &&
                (y +SPRITE_HEIGHT-1 <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y +SPRITE_HEIGHT-1 >= m_actors[i]->getY())) ||
               ((x +SPRITE_WIDTH-1 <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x +SPRITE_WIDTH-1>= m_actors[i]->getX()) &&
                (y <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y >= m_actors[i]->getY()))
               )
            {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::ifFlameBlocked(int x, int y){
    for(int i = 0; i < m_numActors; i++)
    {
        
        if(m_actors[i]->canBlockProjectile()) //if actor can block
            if(((x <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x>= m_actors[i]->getX()) &&
                (y <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y>= m_actors[i]->getY())) ||
               ((x +SPRITE_WIDTH-1 <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x +SPRITE_WIDTH-1>= m_actors[i]->getX()) &&
                (y +SPRITE_HEIGHT-1 <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y +SPRITE_HEIGHT-1 >= m_actors[i]->getY())) ||
               ((x <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x >= m_actors[i]->getX()) &&
                (y +SPRITE_HEIGHT-1 <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y +SPRITE_HEIGHT-1 >= m_actors[i]->getY())) ||
               ((x +SPRITE_WIDTH-1 <= m_actors[i]->getX()+SPRITE_WIDTH-1 && x +SPRITE_WIDTH-1>= m_actors[i]->getX()) &&
                (y <= m_actors[i]->getY()+SPRITE_HEIGHT-1 && y >= m_actors[i]->getY()))
               )
            {
                return true;
            }
    }
    return false;
}

void StudentWorld::finishLevel(){
    m_finishedLevel = true;
}

int StudentWorld::distance(int x1, int y1, int x2, int y2) const
{
    return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

//check penelope firing flame

void StudentWorld::flameAttack(int x, int y)
{
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_actors[i]->canBeDamaged())
        {
            if(m_actors[i]->getIntersect() == A_LANDMINE &&
                distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100)
            {
                m_actors[i]->setDetonate();
            }
            else if(m_actors[i]->getIntersect() == getLivingThingOverlap(x, y) &&
               distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100)
            {
                if(m_actors[i]->getIntersect() == A_ZOMBIE_DUMB)
                {
                    //drop vaccine goodie 1 in 10 times
                    if(randInt(1, 10) == 1)
                    {
                        int dir = randInt(1, 4);
                        switch (dir) {
                            //left
                            case 1:
                                if(!ifBlock(m_actors[i]->getX()-SPRITE_WIDTH, m_actors[i]->getY(), A_ZOMBIE_DUMB, m_actors[i]->getX(), m_actors[i]->getY()))
                                {
                                    createActor(new VaccineGoodie(this,m_actors[i]->getX()-SPRITE_WIDTH,m_actors[i]->getY()));
                                }
                                break;
                            //right
                            case 2:
                                if(!ifBlock(m_actors[i]->getX()+SPRITE_WIDTH, m_actors[i]->getY(), A_ZOMBIE_DUMB, m_actors[i]->getX(), m_actors[i]->getY()))
                                {
                                    createActor(new VaccineGoodie(this,m_actors[i]->getX()+SPRITE_WIDTH,m_actors[i]->getY()));
                                }
                                break;
                            //up
                            case 3:
                                if(!ifBlock(m_actors[i]->getX(), m_actors[i]->getY()+SPRITE_HEIGHT, A_ZOMBIE_DUMB, m_actors[i]->getX(), m_actors[i]->getY()))
                                {
                                    createActor(new VaccineGoodie(this,m_actors[i]->getX(),m_actors[i]->getY()+SPRITE_HEIGHT));
                                }
                                break;
                            //down
                            case 4:
                                if(!ifBlock(m_actors[i]->getX(), m_actors[i]->getY()+SPRITE_HEIGHT, A_ZOMBIE_DUMB, m_actors[i]->getX(), m_actors[i]->getY()))
                                {
                                    createActor(new VaccineGoodie(this,m_actors[i]->getX(),m_actors[i]->getY()-SPRITE_HEIGHT));
                                }
                                break;
                        }
                    }
                    playSound(SOUND_ZOMBIE_DIE);
                    increaseScore(1000);
                }
                else if(m_actors[i]->getIntersect() == A_ZOMBIE_SMART)
                {
                    playSound(SOUND_ZOMBIE_DIE);
                    increaseScore(2000);
                }
                else if(m_actors[i]->getIntersect() == A_PENELOPE)
                {
                    decLives();
                    playSound(SOUND_PLAYER_DIE);
                }
                else if(m_actors[i]->getIntersect() == A_CITIZEN)
                {
                    increaseScore(-1000);
                    playSound(SOUND_CITIZEN_DIE);
                }
                m_actors[i]->setDead();
            }
            else if(m_actors[i]->getIntersect() == getGoodieOverlap(x, y) &&
                    distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100)
            {
                m_actors[i]->setDead();
            }
        }
    }
}

void StudentWorld::vomitAttack(int x, int y)
{
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_actors[i]->canBeVomited())
        {
            if(m_actors[i]->getIntersect() == getLivingThingOverlap(x, y) &&
                    distance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= 100)
            {
                if(m_actors[i]->getIntersect() == A_PENELOPE)
                {
                    m_penelope->setInfectionStatus(true);
                }
                else if(m_actors[i]->getIntersect() == A_CITIZEN)
                {
                    m_actors[i]->setInfectionStatus(true);
                    if(!m_actors[i]->getInfectionStatus())
                        playSound(SOUND_CITIZEN_INFECTED);
                }
            }
        }
    }
}

//set x and y to closest coordinates
void StudentWorld::getClosestLivingThing(int currX, int currY, int &x, int &y, double& dist, int mode)
{
    int closestX = 0, closestY = 0;
    //set closest distance to max possible distance
    double closestDistance = distance(0, 0, 255, 255);
    for(int i = 0; i < m_numActors; i++)
    {
        if(mode == 0)
        {
            if(m_actors[i]->getIntersect() == A_PENELOPE ||
               m_actors[i]->getIntersect() == A_CITIZEN)
            {
                double currDistance = distance(currX, currY, m_actors[i]->getX(), m_actors[i]->getY());
                if(currDistance < closestDistance)
                {
                    closestDistance = currDistance;
                    closestX = m_actors[i]->getX();
                    closestY = m_actors[i]->getY();
                }
            }
        }
        else if(mode == 1)
        {
            if(m_actors[i]->getIntersect() == A_ZOMBIE_SMART ||
               m_actors[i]->getIntersect() == A_ZOMBIE_DUMB)
            {
                double currDistance = distance(currX, currY, m_actors[i]->getX(), m_actors[i]->getY());
                if(currDistance < closestDistance)
                {
                    closestDistance = currDistance;
                    closestX = m_actors[i]->getX();
                    closestY = m_actors[i]->getY();
                }
            }
        }
    }
    x = closestX;
    y = closestY;
    dist = sqrt(closestDistance);
}

void StudentWorld::deleteSavedCitizens(int x, int y)
{
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_actors[i]->getIntersect() == A_CITIZEN &&
           distance(x, y, m_actors[i]->getX(), m_actors[i]->getY()) <= 100)
            m_actors[i]->setDead();
    }
}

double StudentWorld::getPenelopeDistance(int x, int y)
{
    return distance(x, y, m_penelope->getX(), m_penelope->getY());
}

void StudentWorld::createActor(Actor* actor)
{
    m_actors.push_back(actor);
    m_numActors++;
}

void StudentWorld::displayText(){
    int score = getScore();
    int lives = getLives();
    int level = getLevel();
    int penelope_vaccine = m_penelope->getVaccineCount();
    int penelope_flame = m_penelope->getFlameCharges();
    int penelope_mine = m_penelope->getLandmineCount();
    int penelope_infect = m_penelope->getInfectionCount();
    
    ostringstream oss;
    oss.setf(ios::fixed);
    if(score > 0)
        oss.fill('0');
    oss << "Score: " << setw(5) << score << " ";
    oss << "Level: " << level << " ";
    oss << "Lives: " << lives << " ";
    oss << "Vaccines: " << penelope_vaccine << " ";
    oss << "Flames: " << penelope_flame << " ";
    oss << "Mines: " << penelope_mine << " ";
    oss << "Infected: " << penelope_infect << " ";
    setGameStatText(oss.str());
}

void StudentWorld::explodeLandmine(int x, int y)
{
    playSound(SOUND_LANDMINE_EXPLODE);
    createActor(new Flame(this, x, y));
    //create flame west if possible
    if(!ifFlameBlocked(x-SPRITE_WIDTH, y))
        createActor(new Flame(this, x-SPRITE_WIDTH, y));
    //create flame east if possible
    if(!ifFlameBlocked(x+SPRITE_WIDTH, y))
        createActor(new Flame(this, x+SPRITE_WIDTH, y));
    //create flame north if possible
    if(!ifFlameBlocked(x, y+SPRITE_HEIGHT))
        createActor(new Flame(this, x, y+SPRITE_HEIGHT));
    //create flame south if possible
    if(!ifFlameBlocked(x, y-SPRITE_HEIGHT))
        createActor(new Flame(this, x, y-SPRITE_HEIGHT));
    //create flame northeast if possible
    if(!ifFlameBlocked(x+SPRITE_WIDTH, y+SPRITE_HEIGHT))
        createActor(new Flame(this, x+SPRITE_WIDTH, y+SPRITE_HEIGHT));
    //create flame northwest if possible
    if(!ifFlameBlocked(x-SPRITE_WIDTH, y+SPRITE_HEIGHT))
        createActor(new Flame(this, x-SPRITE_WIDTH, y+SPRITE_HEIGHT));
    //create flame southeast if possible
    if(!ifFlameBlocked(x+SPRITE_WIDTH, y-SPRITE_HEIGHT))
        createActor(new Flame(this, x+SPRITE_WIDTH, y-SPRITE_HEIGHT));
    //create flame southwest if possible
    if(!ifFlameBlocked(x-SPRITE_WIDTH, y-SPRITE_HEIGHT))
        createActor(new Flame(this, x-SPRITE_WIDTH, y-SPRITE_HEIGHT));
    createActor(new Pit(this, x, y));
}

int StudentWorld::getCitizenCount(){
    int citizenNum = 0;
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_actors[i]->getIntersect() == A_CITIZEN)
            citizenNum++;
    }
    return citizenNum;
}

int StudentWorld::getZombieCount(){
    int zombieNum = 0;
    for(int i = 0; i < m_numActors; i++)
    {
        if(m_actors[i]->getIntersect() == A_ZOMBIE_DUMB ||
           m_actors[i]->getIntersect() == A_ZOMBIE_SMART)
            zombieNum++;
    }
    return zombieNum;
}

void StudentWorld::changePenelopeItems(int goodie, int charge, int num){
    m_penelope->changeCharge(goodie, charge, num);
}
