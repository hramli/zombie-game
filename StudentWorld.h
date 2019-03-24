#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Level.h"
#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    int getLivingThingOverlap(int x, int y);
    int getGoodieOverlap(int x, int y);
    bool ifBlock(int x, int y, int selfID, int currentX, int currentY); //pass in selfID to prevent blocking itself
    bool ifFlameBlocked(int x, int y);
    void finishLevel(); //set ifFinishLevel to true
    int distance(int x1, int y1, int x2, int y2) const;
    void changePenelopeItems(int goodie, int charge, int num);
    double getPenelopeDistance(int x, int y);
    int getPenelopeX(){return m_penelope->getX();}
    int getPenelopeY(){return m_penelope->getY();}
    void createActor(Actor* actor);
    int getTicks(){return m_numTicks;}
    void displayText();
    void flameAttack(int x, int y); //flame attacks overlapped objects
    void vomitAttack(int x, int y);
    //get closest Citizen/Penelope(mode 0) or Zombie(mode 1)
    void getClosestLivingThing(int currX, int currY, int &x, int &y, double& dist, int mode);
    void deleteSavedCitizens(int x, int y);
    int getZombieCount();
    int getCitizenCount();
    void explodeLandmine(int x, int y);
    
    //OPTIMIZE, CHANGE TO CREATE ACTOR

private:
    std::vector<Actor*> m_actors;
    Penelope* m_penelope;
    int m_numActors;
    bool m_finishedLevel;
    int m_numTicks;};


#endif // STUDENTWORLD_H_
