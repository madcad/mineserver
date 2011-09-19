/*
  Copyright (c) 2011, The Mineserver Project
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the The Mineserver Project nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ITEMS_PROJECTILE_H
#define _ITEMS_PROJECTILE_H

#include <stdint.h>
#include <string>
#include <list>

#include "../mineserver.h"
#include "../constants.h"
#include "../inventory.h"
#include "itembasic.h"

class ItemProjectile: public ItemBasic
{
  private:
  int32_t EID;
  int8_t projID;
  float intialHeight;
  float intialVelocity;
  float intialAngle;
  float totalHorizontalDistance;
  struct position 
  {
    double x;
    double y;
    double z;
    size_t map;
    float yaw;
    float pitch;
  };
  struct velocity
  {
    double x;
    double y;
    double z;
  };
  position pos;
  velocity vel;
  int displacementX;
  int displacementY;
  int ticksInFlight; //Ticks projectile was in flight
  int ticksStationary; // Ticks projectile wasn't moving
  bool inFlight;
  //const int projectileTimeout = 10000; // Milliseconds Stay alive exactly one minute
  User* user;

  //Need time alive, to make arrow disappear after certian time.
  //Need timeout of arrow, starts after it is no longer flying
  //On collision with a mob or another player arrow is disappers and deals damage.
  //Track owner(user) of arrow
  //inFlight() to see if it is moving, check for colisions.
  //If arrow is shot by player they can retrieve their arrows under certian conditions

  // There are 4 values unknown that may deal with projectiles.
  // http://mc.kev009.com/Protocol#Add_Object.2FVehicle_.280x17.29
  // At the time of writing the first one should be set to zero for us to not crash the client

public:
  bool affectedItem(int item) const;
  void onRightClick(User* user, Item* item);

  ItemProjectile();
  ItemProjectile(User* user, int8_t ID);

  void updatePosition();
  //bool hasTimedout() { return timeStationary >= projectileTimeout;}
  bool isInFlight() { return inFlight; }
  bool canBeRetrieved() {} //pickedup

  //Setters
  void setPosition( double x, double y, double z, size_t map, float yaw, float pitch){ pos.x = x; pos.y = y; pos.z = z; pos.map = map; pos.yaw = yaw, pos.pitch = pitch;}
  void setVelocity( double x, double y, double z){vel.x = x; vel.y = y; vel.z = z;}
  void setEID(){EID = Mineserver::generateEID();}
  void setProjectileID(int8_t ID){projID = ID;}
  void setUser(User* owner){user = owner;}
  void setInFlight( bool temp ){inFlight = temp;}

  //Getters
  struct position getPosition(){return pos;}
  struct velocity getVelocity(){return vel;}
  int32_t getEID() { return EID; }
  int8_t getProjectileID(){return projID;}
  User* getUser(){return user;}
  int getTicksInFlight(){return ticksInFlight;}
  int getTicksStationary(){return ticksStationary;}

  //Increment ticks
  void incrementTicksInFlight(){ ++ticksInFlight;}
  void incrementTicksStationary(){ ++ticksStationary;}

 };
#endif
