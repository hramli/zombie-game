#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, bool blockState, bool aliveState, Direction dir, int depth, double size):
GraphObject(imageID, startX, startY, dir, depth, size),m_world(world), blockState(blockState), aliveState(aliveState),m_depth(depth),m_tick(world->getTicks()),activeState(false),infection_status(false),infection_count(0)
{}

Actor::~Actor()
{}

StudentWorld* Actor::getWorld() const{
    return m_world;
}

bool Actor::getBlockState() const{
    return blockState;
}

bool Actor::getAliveState() const{
    return aliveState;
}

void Actor::setDead(){
    aliveState = false;
}

int Actor::getDepth(){
    return m_depth;
}

bool Actor::whetherLivingThing(){
    return false;
}

//People implementation
People::People(StudentWorld* world, int ID, int startX, int startY):
Actor(world,ID,startX, startY, true, true, right, 0)
{
    
}

People::~People(){}

bool People::whetherLivingThing(){
    return true;
}

//Penelope implementation
Penelope::Penelope(StudentWorld* world, int startX, int startY):
People(world, IID_PLAYER,startX, startY),
m_landmines(0),m_flamecharges(0),m_vaccines(0)
{};

Penelope::~Penelope(){}

void Penelope::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    int keyPressed;
    if(!this->getAliveState())
        return;
    if(getInfectionStatus())
        incrementInfectionCount();
    if(getInfectionCount() == 500)
    {
        setDead();
        ptrWorld->playSound(SOUND_PLAYER_DIE);
        return;
    }
    if(ptrWorld->getKey(keyPressed)) //if a key is pressed
    {
        switch (keyPressed) {
            case KEY_PRESS_LEFT:
                if(!ptrWorld->ifBlock(getX()-4, getY(),this->getIntersect(),getX(),getY()))
                    this->moveTo(getX()-A_MOVE_SPEED, getY());
                this->setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if(!ptrWorld->ifBlock(getX()+4, getY(),this->getIntersect(),getX(),getY()))
                    this->moveTo(getX()+A_MOVE_SPEED, getY());
                this->setDirection(right);
                break;
            case KEY_PRESS_UP:
                if(!ptrWorld->ifBlock(getX(), getY()+4,this->getIntersect(),getX(),getY()))
                    this->moveTo(getX(), getY()+A_MOVE_SPEED);
                this->setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if(!ptrWorld->ifBlock(getX(), getY()-4,this->getIntersect(),getX(),getY()))
                    this->moveTo(getX(), getY()-A_MOVE_SPEED);
                this->setDirection(down);
                break;
            case KEY_PRESS_SPACE:
                if(this->m_flamecharges > 0)
                {
                    this->changeCharge(A_GASCAN, 0, 1);
                    ptrWorld->playSound(SOUND_PLAYER_FIRE);
                    switch(getDirection())
                    {
                        case GraphObject::left:
                            for(int i = 1; i <= 3; i++)
                            {
                                if(ptrWorld->ifFlameBlocked(getX()-SPRITE_WIDTH*i, getY()))
                                    break;
                                ptrWorld->createActor(new Flame(ptrWorld,getX()-SPRITE_WIDTH*i, getY()));
                            }
                            break;
                        case GraphObject::right:
                            for(int i = 1; i <= 3; i++)
                            {
                                if(ptrWorld->ifFlameBlocked(getX()+SPRITE_WIDTH*i, getY()))
                                    break;
                                ptrWorld->createActor(new Flame(ptrWorld,getX()+SPRITE_WIDTH*i, getY()));
                            }
                            break;
                        case GraphObject::up:
                            for(int i = 1; i <= 3; i++)
                            {
                                if(ptrWorld->ifFlameBlocked(getX(), getY()+SPRITE_HEIGHT*i))
                                    break;
                                ptrWorld->createActor(new Flame(ptrWorld,getX(), getY()+SPRITE_HEIGHT*i));
                            }
                            break;
                        case GraphObject::down:
                            for(int i = 1; i <= 3; i++)
                            {
                                if(ptrWorld->ifFlameBlocked(getX(), getY()-SPRITE_HEIGHT*i))
                                    break;
                                ptrWorld->createActor(new Flame(ptrWorld,getX(), getY()-SPRITE_HEIGHT*i));
                            }
                            break;
                }
                break;
            case KEY_PRESS_TAB:
                if(this->m_landmines > 0)
                {
                    ptrWorld->createActor(new Landmine(ptrWorld, getX(), getY()));
                    this->changeCharge(A_LANDMINEGOODIE, 0, 1);
                }
                break;
            case KEY_PRESS_ENTER:
                if(this->m_vaccines > 0)
                {
                    this->changeCharge(A_VACCINEGOODIE, 0, 1);
                    People::setInfectionStatus(false);
                    decrementInfectionCount();
                }
                break;
            default:
                break;
        }
    }
}
}

//Citizen implementation
Citizen::Citizen(StudentWorld* world, int startX, int startY)
:People(world, IID_CITIZEN,startX, startY)
{}

Citizen::~Citizen(){}

int Citizen::moveCitizen(){
    StudentWorld* ptrWorld = getWorld();
    switch (getDirection())
    {
        case left:
            if(!ptrWorld->ifBlock(getX()-2, getY(),this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX()-2, getY());
                return 0;
            }
            break;
        case right:
            if(!ptrWorld->ifBlock(getX()+2, getY(),this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX()+2, getY());
                return 0;
            }
            break;
        case up:
            if(!ptrWorld->ifBlock(getX(), getY()+2,this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX(), getY()+2);
                return 0;
            }
            break;
        case down:
            if(!ptrWorld->ifBlock(getX(), getY()-2,this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX(), getY()-2);
                return 0;
            }
            break;
    }
    return 1;
}

void Citizen::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!getAliveState())
        return;
    if(getInfectionStatus())
    {
        incrementInfectionCount();
        if(getInfectionCount() == 500)
        {
            setDead();
            ptrWorld->playSound(SOUND_ZOMBIE_BORN);
            ptrWorld->increaseScore(-1000);
            if(randInt(1, 10) <= 3)
                ptrWorld->createActor(new SmartZombie(ptrWorld, getX(), getY()));
            else
                ptrWorld->createActor(new DumbZombie(ptrWorld, getX(), getY()));
            return;
        }
    }
    //skip 2, 4, 6... ticks
    if((getCreatedTick() % 2 == 0 && ptrWorld->getTicks() % 2 == 1) ||
       (getCreatedTick() % 2 == 1 && ptrWorld->getTicks() % 2 == 0))
        return;
    
    double dist_p = ptrWorld->getPenelopeDistance(getX(), getY());
    int penelopeX = ptrWorld->getPenelopeX();
    int penelopeY = ptrWorld->getPenelopeY();
    int closestZombieX = A_IMPOSSIBLE_VAL, closestZombieY = A_IMPOSSIBLE_VAL;
    double dist_z = A_IMPOSSIBLE_VAL;
    ptrWorld->getClosestLivingThing(getX(), getY(), closestZombieX, closestZombieY, dist_z, 1);
    if((ptrWorld->getZombieCount() == 0 || dist_p > dist_z) && dist_p <= 80*80)
    {
        //if same row
        if(getX() == penelopeX || getY() == penelopeY)
        {
            if(getX() < penelopeX)
                setDirection(right);
            else if(getX() > penelopeX)
                setDirection(left);
            else if(getY() < penelopeY)
                setDirection(up);
            else if(getY() < penelopeY)
                setDirection(down);
            if(!moveCitizen()) //if citizen moved
                return;
        }
        else
        {
            double leftDistance, upDistance, downDistance, rightDistance;
            double leftVsRightDist, upVsDownDist, selectedDist;
            leftDistance = ptrWorld->distance(getX()-1, getY(), penelopeX, penelopeY);
            rightDistance = ptrWorld->distance(getX()+1, getY(), penelopeX, penelopeY);
            upDistance = ptrWorld->distance(getX(), getY()+1, penelopeX, penelopeY);
            downDistance = ptrWorld->distance(getX(), getY()-1, penelopeX, penelopeY);
            leftVsRightDist = fmin(leftDistance, rightDistance);
            upVsDownDist = fmin(upDistance,downDistance);
            selectedDist = fmin(leftVsRightDist, upVsDownDist);
            
            //random select
            selectedDist = leftVsRightDist;
            if(randInt(1, 2) == 1)
                selectedDist = upVsDownDist;
            
            if(selectedDist == leftVsRightDist)
            {
                if(selectedDist == leftDistance)
                {
                    setDirection(left);
                    if(moveCitizen())
                    {
                        setDirection(right);
                        if(!moveCitizen())
                            return;
                    }
                }
                else
                {
                    setDirection(right);
                    if(moveCitizen())
                    {
                        setDirection(left);
                        if(!moveCitizen())
                            return;
                    }
                }
            }
            else if(selectedDist == upVsDownDist)
            {
                if(selectedDist == upDistance)
                {
                    setDirection(up);
                    if(moveCitizen())
                    {
                        setDirection(down);
                        if(!moveCitizen())
                            return;
                    }
                }
                else
                {
                    setDirection(down);
                    if(moveCitizen())
                    {
                        setDirection(up);
                        if(!moveCitizen())
                            return;
                    }
                }
            }
        }
    }
    else if(ptrWorld->getZombieCount() > 0 && dist_z <= 80)
    {
        int dummyX = 0, dummyY = 0;
        double currentDistanceFromZombie = dist_z;
        double leftDistanceFromZombie = -1;
        double rightDistanceFromZombie = -1;
        double upDistanceFromZombie = -1;
        double downDistanceFromZombie = -1;
        if(!ptrWorld->ifBlock(getX()-2, getY(), A_CITIZEN, getX(), getY()))
            ptrWorld->getClosestLivingThing(getX()-2, getY(), dummyX, dummyY, leftDistanceFromZombie, 1);
        if(!ptrWorld->ifBlock(getX()+2, getY(), A_CITIZEN, getX(), getY()))
            ptrWorld->getClosestLivingThing(getX()+2, getY(), dummyX, dummyY, rightDistanceFromZombie, 1);
        if(!ptrWorld->ifBlock(getX(), getY()+2, A_CITIZEN, getX(), getY()))
            ptrWorld->getClosestLivingThing(getX(), getY()+2, dummyX, dummyY, upDistanceFromZombie, 1);
        if(!ptrWorld->ifBlock(getX(), getY()-2, A_CITIZEN, getX(), getY()))
            ptrWorld->getClosestLivingThing(getX(), getY()-2, dummyX, dummyY, downDistanceFromZombie, 1);
        if(currentDistanceFromZombie < leftDistanceFromZombie && currentDistanceFromZombie < rightDistanceFromZombie && currentDistanceFromZombie < upDistanceFromZombie && currentDistanceFromZombie < downDistanceFromZombie)
            return;
        else
        {
            double maxDistanceFromZombie = fmax(leftDistanceFromZombie, rightDistanceFromZombie);
            maxDistanceFromZombie = fmax(maxDistanceFromZombie, upDistanceFromZombie);
            maxDistanceFromZombie = fmax(maxDistanceFromZombie, downDistanceFromZombie);
            if(maxDistanceFromZombie == leftDistanceFromZombie)
            {
                setDirection(left);
            }
            else if(maxDistanceFromZombie == rightDistanceFromZombie)
            {
                setDirection(right);
            }
            else if(maxDistanceFromZombie == upDistanceFromZombie)
            {
                setDirection(up);
            }
            else if(maxDistanceFromZombie == downDistanceFromZombie)
            {
                setDirection(down);
            }
            if(!moveCitizen())
                return;
        }
    }
}

//Wall implementation
Wall::Wall(StudentWorld* world, int startX, int startY):
Actor(world, IID_WALL, startX, startY, true, true)
{}

Wall::~Wall(){}

void Wall::doSomething(){}


//Exit implementation
Exit::Exit(StudentWorld* world, int startX, int startY):
Actor(world, IID_EXIT, startX, startY, false, true, right, 1)
{}

Exit::~Exit(){}

void Exit::doSomething()
{
    StudentWorld* ptrWorld = getWorld();
    //if Exit overlaps with Penelope
    if(ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_PENELOPE)
    {
        if(ptrWorld->getCitizenCount() == 0)
        {
            ptrWorld->finishLevel();
            ptrWorld->playSound(SOUND_LEVEL_FINISHED);
        }
    }
    else if(ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_CITIZEN)
    {
        ptrWorld->increaseScore(500);
        ptrWorld->deleteSavedCitizens(getX(), getY());
        ptrWorld->playSound(SOUND_CITIZEN_SAVED);
    }
}

//Pit implementation
Pit::Pit(StudentWorld* world, int startX, int startY):
Actor(world, IID_PIT, startX, startY, false, true, right, 0)
{}

Pit::~Pit(){}

void Pit::doSomething()
{
    StudentWorld* ptrWorld = getWorld();
    ptrWorld->flameAttack(getX(), getY());
}

//Landmine implementation
Landmine::Landmine(StudentWorld* world, int startX, int startY):
Actor(world, IID_LANDMINE, startX, startY, false, true,right,1),safetyTicks(30)
{}

Landmine::~Landmine(){}

void Landmine::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!getAliveState())
        return;
    if(safetyTicks > 0)
    {
        safetyTicks--;
        if(safetyTicks == 0)
            setActiveState();
        return;
    }
    if(getActiveState() &&
       (ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_PENELOPE ||
       ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_CITIZEN ||
       ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_ZOMBIE_SMART ||
        ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_ZOMBIE_DUMB ||
        getDetonate()))
    {
        setDead();
        ptrWorld->explodeLandmine(getX(), getY());
    }
    
}

//Goodie implementation
Goodie::Goodie(StudentWorld* world, int ID, int startX, int startY, int goodie):
Actor(world, ID, startX, startY, false, true, right, 1),m_goodie(goodie)
{}

Goodie::~Goodie(){}

void Goodie::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!this->getAliveState())
        return;
    if(ptrWorld->getLivingThingOverlap(this->getX(), this->getY()) == A_PENELOPE)
    {
        int numChange = 0;
        switch (m_goodie) {
            case A_LANDMINEGOODIE:
                numChange = 2;
                break;
            case A_VACCINEGOODIE:
                numChange = 1;
                break;
            case A_GASCAN:
                numChange = 5;
                break;
            default:
                break;
        }
        ptrWorld->changePenelopeItems(m_goodie, 1, numChange);
        ptrWorld->increaseScore(50);
        this->setDead();
        ptrWorld->playSound(SOUND_GOT_GOODIE);
    }
}

VaccineGoodie::VaccineGoodie(StudentWorld* world, int startX, int startY):
Goodie(world, IID_VACCINE_GOODIE, startX, startY, A_VACCINEGOODIE)
{}

VaccineGoodie::~VaccineGoodie(){}

GasCan::GasCan(StudentWorld* world, int startX, int startY):
Goodie(world, IID_GAS_CAN_GOODIE, startX, startY, A_GASCAN)
{}

GasCan::~GasCan(){}

LandmineGoodie::LandmineGoodie(StudentWorld* world, int startX, int startY):
Goodie(world, IID_LANDMINE_GOODIE, startX, startY, A_LANDMINEGOODIE)
{}

LandmineGoodie::~LandmineGoodie(){}

//****************
//Zombie implementation
Zombie::Zombie(StudentWorld* world, int startX, int startY):
Actor(world, IID_ZOMBIE, startX, startY, true, true, right, 0)
{}

Zombie::~Zombie(){}

void Zombie::moveZombie(){
    StudentWorld* ptrWorld = getWorld();
    switch (getDirection()) {
        case left:
            if(!ptrWorld->ifBlock(getX()-1, getY(),this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX()-A_ZOMBIE_SPEED, getY());
                decreaseMovementPlan();
            }
            else
                setMovementPlan(0);
            break;
        case right:
            if(!ptrWorld->ifBlock(getX()+1, getY(),this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX()+A_ZOMBIE_SPEED, getY());
                decreaseMovementPlan();
            }
            else
                setMovementPlan(0);
            break;
        case up:
            if(!ptrWorld->ifBlock(getX(), getY()+1,this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX(), getY()+A_ZOMBIE_SPEED);
                decreaseMovementPlan();
            }
            else
                setMovementPlan(0);
            break;
        case down:
            if(!ptrWorld->ifBlock(getX(), getY()-1,this->getIntersect(),getX(),getY()))
            {
                this->moveTo(getX(), getY()-A_ZOMBIE_SPEED);
                decreaseMovementPlan();
            }
            else
                setMovementPlan(0);
            break;
        default:
            break;
    }
}

int Zombie::actZombie(){
    StudentWorld* ptrWorld = getWorld();
    switch (this->getDirection()) {
        case left:
            if(ptrWorld->getLivingThingOverlap(getX()-SPRITE_WIDTH, getY()) == A_PENELOPE ||
               ptrWorld->getLivingThingOverlap(getX()-SPRITE_WIDTH, getY()) == A_CITIZEN)
            {
                if(randInt(1, 3) == 1) //1 in 3 times, zombie will vomit
                {
                    ptrWorld->createActor(new Vomit(ptrWorld,getX()-SPRITE_WIDTH, getY()));
                    ptrWorld->playSound(SOUND_ZOMBIE_VOMIT);
                    return 0;
                }
            }
            break;
        case right:
            if(ptrWorld->getLivingThingOverlap(getX()+SPRITE_WIDTH, getY()) == A_PENELOPE ||
               ptrWorld->getLivingThingOverlap(getX()+SPRITE_WIDTH, getY()) == A_CITIZEN)
            {
                if(randInt(1, 3) == 1) //1 in 3 times, zombie will vomit
                {
                    ptrWorld->createActor(new Vomit(ptrWorld,getX()+SPRITE_WIDTH, getY()));
                    ptrWorld->playSound(SOUND_ZOMBIE_VOMIT);
                    return 0;
                }
            }
            break;
        case up:
            if(ptrWorld->getLivingThingOverlap(getX(), getY()+SPRITE_HEIGHT) == A_PENELOPE ||
               ptrWorld->getLivingThingOverlap(getX(), getY()+SPRITE_HEIGHT) == A_CITIZEN)
            {
                if(randInt(1, 3) == 1) //1 in 3 times, zombie will vomit
                {
                    ptrWorld->createActor(new Vomit(ptrWorld,getX(), getY()+SPRITE_HEIGHT));
                    ptrWorld->playSound(SOUND_ZOMBIE_VOMIT);
                    return 0;
                }
            }
            break;
        case down:
            if(ptrWorld->getLivingThingOverlap(getX(), getY()-SPRITE_HEIGHT) == A_PENELOPE ||
               ptrWorld->getLivingThingOverlap(getX(), getY()-SPRITE_HEIGHT) == A_CITIZEN)
            {
                if(randInt(1, 3) == 1) //1 in 3 times, zombie will vomit
                {
                    ptrWorld->createActor(new Vomit(ptrWorld,getX(), getY()-SPRITE_HEIGHT));
                    ptrWorld->playSound(SOUND_ZOMBIE_VOMIT);
                    return 0;
                }
            }
            break;
    }
    if(getMovementPlan() == 0)
    {
        setMovementPlan(randInt(3, 10));
        int randDir = randInt(1,4);
        switch (randDir) {
            case 1:
                setDirection(left);
                break;
            case 2:
                setDirection(right);
                break;
            case 3:
                setDirection(up);
                break;
            case 4:
                setDirection(down);
                break;
        }
    }
    return 1;
}


//Smart Zombie implementation
SmartZombie::SmartZombie(StudentWorld* world, int startX, int startY):
Zombie(world, startX, startY)
{}

SmartZombie::~SmartZombie(){}

void SmartZombie::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!getAliveState())
        return;
    //every 2nd tick, 4th tick, etc, skip
    if((getCreatedTick() % 2 == 0 && ptrWorld->getTicks() % 2 == 1) ||
       (getCreatedTick() % 2 == 1 && ptrWorld->getTicks() % 2 == 0))
        return;
    if(!actZombie())
        return;
    //set to impossible values
    int closestX = A_IMPOSSIBLE_VAL;
    int closestY = A_IMPOSSIBLE_VAL;
    double closestDistance = A_IMPOSSIBLE_VAL;
    ptrWorld->getClosestLivingThing(getX(), getY(), closestX, closestY, closestDistance,0);
    if(closestDistance > 80)
    {
        int dir = randInt(1, 4);
        switch (dir) {
            case 1:
                setDirection(left);
                break;
            case 2:
                setDirection(right);
                break;
            case 3:
                setDirection(up);
                break;
            case 4:
                setDirection(down);
                break;
            default:
                break;
        }
    }
    else
    {
        double leftDistance, upDistance, downDistance, rightDistance;
        double leftVsRightDist, upVsDownDist, selectedDist;
        leftDistance = ptrWorld->distance(getX()-1, getY(), closestX, closestY);
        rightDistance = ptrWorld->distance(getX()+1, getY(), closestX, closestY);
        upDistance = ptrWorld->distance(getX(), getY()+1, closestX, closestY);
        downDistance = ptrWorld->distance(getX(), getY()-1, closestX, closestY);
        leftVsRightDist = fmin(leftDistance, rightDistance);
        upVsDownDist = fmin(upDistance,downDistance);
        selectedDist = fmin(leftVsRightDist, upVsDownDist);
        
        if(getX() == closestX || getY() == closestY)
        {
            if(selectedDist == leftDistance)
                setDirection(left);

            else if(selectedDist == rightDistance)
                setDirection(right);
            
            else if(selectedDist == upDistance)
                setDirection(up);
            
            else if(selectedDist == downDistance)
                setDirection(down);
        }
        else
        {
            selectedDist = leftVsRightDist;
            if(randInt(1, 2) == 1)
                selectedDist = upVsDownDist;
            if(selectedDist == leftVsRightDist)
            {
                if(selectedDist == leftDistance)
                    setDirection(left);
                else
                    setDirection(right);
            }
            else if(selectedDist == upVsDownDist)
            {
                if(selectedDist == upDistance)
                    setDirection(up);
                else
                    setDirection(down);
            }
        }
    }
    moveZombie();
}

//Dumb Zombie implementation
DumbZombie::DumbZombie(StudentWorld* world, int startX, int startY):
Zombie(world, startX, startY)
{}

DumbZombie::~DumbZombie(){}

void DumbZombie::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!getAliveState())
        return;
    //every 2nd tick, 4th tick, etc, skip
    if((getCreatedTick() % 2 == 0 && ptrWorld->getTicks() % 2 == 1) ||
       (getCreatedTick() % 2 == 1 && ptrWorld->getTicks() % 2 == 0))
        return;
    if(!actZombie())
        return;
    moveZombie();
}

//****************
//Projectile implementation
Projectile::Projectile(StudentWorld* world, int ID, int startX, int startY):
Actor(world, ID, startX, startY, false, true, up, 0)
{}

Projectile::~Projectile(){}

void Projectile::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(ptrWorld->getTicks() - this->getCreatedTick() == 2)
        setDead();
}


Flame::Flame(StudentWorld* world, int startX, int startY):
Projectile(world, IID_FLAME, startX, startY)
{}

Flame::~Flame(){}

void Flame::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!this->getAliveState())
        return;
    Projectile::doSomething();
    ptrWorld->flameAttack(getX(), getY());
}

Vomit::Vomit(StudentWorld* world, int startX, int startY):
Projectile(world, IID_VOMIT, startX, startY)
{}

Vomit::~Vomit(){}

void Vomit::doSomething(){
    StudentWorld* ptrWorld = getWorld();
    if(!this->getAliveState())
        return;
    Projectile::doSomething();
    ptrWorld->vomitAttack(getX(),getY());
    
}

