#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

//global constants
const int A_MOVE_SPEED = 4;
const int A_DO_NOTHING = 0;
const int A_CITIZEN = 1;
const int A_PENELOPE = 2;
const int A_WALL = 3;
const int A_EXIT = 4;
const int A_PIT = 5;
const int A_ZOMBIE = 6;
const int A_GOODIE = 7;
const int A_VACCINEGOODIE = 8;
const int A_GASCAN = 9;
const int A_LANDMINEGOODIE = 10;
const int A_CAN_KILL = -5;
const int A_LANDMINE = 11;
const int A_ZOMBIE_SPEED = 1;
const int A_ZOMBIE_SMART = 12;
const int A_ZOMBIE_DUMB = 13;
const int A_IMPOSSIBLE_VAL = 100000000;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, bool blockState, bool aliveState, Direction dir =0 , int depth=0, double size=1.0);
    virtual ~Actor();
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const;
    bool getBlockState() const;
    bool getAliveState() const;
    void setDead();
    void setActiveState(){activeState = true;}
    int getDepth();
    int getCreatedTick(){return m_tick;}
    bool getActiveState(){return activeState;}
    bool getInfectionStatus(){return infection_status;};
    int getInfectionCount(){return infection_count;};
    void incrementInfectionCount(){infection_count++;};
    void decrementInfectionCount(){infection_count--;}
    void setInfectionStatus(bool infection_stat){infection_status = infection_stat;}
    virtual bool canBeDamaged(){return false;}
    virtual bool canBeVomited(){return false;}
    virtual bool canBlockProjectile(){return true;}
    virtual int getIntersect() = 0;
    virtual void setDetonate(){toDetonate=true;}
    virtual bool getDetonate(){return toDetonate;}
    virtual bool whetherLivingThing();
    virtual bool canBePickedUp(){return false;};
private:
    StudentWorld* m_world;
    bool blockState;
    bool aliveState;
    int m_depth;
    int m_tick;
    bool activeState;
    bool toDetonate;
    bool infection_status;
    int infection_count;
};

class People : public Actor
{
public:
    People(StudentWorld* world, int ID, int startX, int startY);
    ~People();
    virtual void doSomething() = 0;
    virtual bool whetherLivingThing();
    virtual int getIntersect(){return A_CITIZEN;}
    virtual bool canBeDamaged(){return true;}
    virtual bool canBlockProjectile(){return false;}
    virtual bool canBeVomited(){return true;}
};

class Penelope : public People
{
public:
    Penelope(StudentWorld* world, int startX, int startY);
    ~Penelope();
    virtual void doSomething();
    virtual int getIntersect(){return A_PENELOPE;}
    void changeCharge(int goodie, int change, int numChange){
        switch(goodie)
        {
            case A_GASCAN:
                if(change) //if change = 1, then increase charge
                    m_flamecharges+=numChange;
                else
                    m_flamecharges-=numChange;
                break;
            case A_VACCINEGOODIE:
                if(change) //if change = 1, then increase charge
                    m_vaccines+=numChange;
                else
                    m_vaccines-=numChange;
                break;
            case A_LANDMINEGOODIE:
                if(change) //if change = 1, then increase charge
                    m_landmines+=numChange;
                else
                    m_landmines-=numChange;
                break;
        }
    }
    int getLandmineCount() {return m_landmines;}
    int getFlameCharges() {return m_flamecharges;}
    int getVaccineCount() {return m_vaccines;}
private:
    int m_landmines;
    int m_flamecharges;
    int m_vaccines;
};

class Citizen : public People
{
public:
    Citizen(StudentWorld* world, int startX, int startY);
    ~Citizen();
    int moveCitizen();
    virtual void doSomething();
};

class Wall : public Actor
{
public:
    Wall(StudentWorld* world, int startX, int startY);
    ~Wall();
    virtual void doSomething();
    virtual int getIntersect(){return A_WALL;}
};

class Exit : public Actor
{
public:
    Exit(StudentWorld* world, int startX, int startY);
    ~Exit();
    virtual void doSomething();
    virtual int getIntersect(){return A_EXIT;}
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* world, int startX, int startY);
    ~Pit();
    virtual void doSomething();
    virtual int getIntersect(){return A_PIT;}
};

class Landmine : public Actor
{
public:
    Landmine(StudentWorld* world, int startX, int startY);
    ~Landmine();
    virtual void doSomething();
    virtual int getIntersect(){return A_LANDMINE;}
    int getSafetyTicks(){return safetyTicks;}
    virtual bool canBlockProjectile(){return false;}
    virtual bool canBeDamaged(){return true;}
private:
    int safetyTicks;
};

//GOODIE

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* world, int ID, int startX, int startY, int goodie);
    ~Goodie();
    virtual void doSomething();
    virtual int getIntersect(){return A_GOODIE;};
    virtual bool canBeDamaged(){return true;}
    virtual bool canBePickedUp(){return true;}
    virtual bool canBlockProjectile(){return false;}
private:
    int m_goodie;
};

class VaccineGoodie : public Goodie
{
public:
    VaccineGoodie(StudentWorld* world, int startX, int startY);
    ~VaccineGoodie();
};

class GasCan : public Goodie
{
public:
    GasCan(StudentWorld* world, int startX, int startY);
    ~GasCan();
};

class LandmineGoodie : public Goodie
{
public:
    LandmineGoodie(StudentWorld* world, int startX, int startY);
    ~LandmineGoodie();
};

//ZOMBIE

class Zombie : public Actor
{
public:
    Zombie(StudentWorld* world, int startX, int startY);
    ~Zombie();
    virtual void doSomething() = 0;
    virtual int getIntersect(){return A_ZOMBIE;}
    virtual bool canBeDamaged(){return true;}
    virtual bool canBlockProjectile(){return false;}
    virtual bool whetherLivingThing(){return true;}
    int actZombie();
    bool getMovementPlan(){return movementPlan;}
    void setMovementPlan(int newMovementPlan){movementPlan = newMovementPlan;}
    void decreaseMovementPlan(){movementPlan--;}
    void moveZombie();
private:
    int movementPlan;
};

class SmartZombie : public Zombie
{
public:
    SmartZombie(StudentWorld* world, int startX, int startY);
    ~SmartZombie();
    virtual void doSomething();
    virtual int getIntersect(){return A_ZOMBIE_SMART;}
};

class DumbZombie: public Zombie
{
public:
    DumbZombie(StudentWorld* world, int startX, int startY);
    ~DumbZombie();
    virtual void doSomething();
    virtual int getIntersect(){return A_ZOMBIE_DUMB;}
};

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* world, int ID, int startX, int startY);
    ~Projectile();
    virtual void doSomething();
    virtual int getIntersect(){return A_CAN_KILL;};
};

class Flame : public Projectile
{
public:
    Flame(StudentWorld* world, int startX, int startY);
    ~Flame();
    virtual void doSomething();
};

class Vomit : public Projectile
{
public:
    Vomit(StudentWorld* world, int startX, int startY);
    ~Vomit();
    virtual void doSomething();
};
#endif // ACTOR_H_
