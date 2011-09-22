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

#include <queue>

#include "default.h"
#include "../mineserver.h"
#include "../map.h"
#include "../constants.h"
#include "../logger.h"
#include "../protocol.h"

#include "tnt.h"

uint8_t pickint;

bool BlockTNT::rb(int32_t x,int8_t y,int8_t z,int map, User* user)
{
  uint8_t block, meta, count;
  int16_t item;

  Mineserver::get()->map(map)->getBlock(x,y,z,&block,&meta);

  BLOCKDROPS[block]->getDrop(item, count, meta);

  // Undestroyable blocks
  if (block == BLOCK_AIR || block == BLOCK_BEDROCK || block == BLOCK_OBSIDIAN)
  {
    return false;
  }
  else
  {
    Mineserver::get()->map(map)->setBlock(x,y,z,0,0);
    Mineserver::get()->map(map)->sendBlockChange(x,y,z,0,0);
  }
  
  // Pickup Spawn Area
  // The integer "pickint" is used to spawn 1/5 of the blocks, otherwise there would be too much pickups!
  if(pickint == 5)
  {
    if(count) {
      Mineserver::get()->map(map)->createPickupSpawn(x, y, z, item, count, meta, user, false);
      pickint=0;
    }
  } else {
    pickint++;
  }

  return true;
}

void BlockTNT::explode(User* user,int32_t centerX,int8_t centerY,int8_t centerZ,int map)
{
  //TODO: Remove chance of not exploding, once we have redstone etc.
  //TODO: TNT should only explode from lighter or redstone.  If you hit tnt with your hand create a pickup.
  //TODO: ADD a chance of outer blocks of explosion not being removed.
  //TODO: Add ability for custom radius to be reused for mobs maybe.
  pickint = 0;
  if (rand() % 9 == 5)
  {
    // There is a chance of 1/10 that the TNT block doesn't explode;
    // this is more realistic ;)
    Mineserver::get()->map(map)->setBlock(centerX,centerY,centerZ,0,0);
    Mineserver::get()->map(map)->sendBlockChange(centerX,centerY,centerZ,0,0);
    // But we want to be fair; let's create a pickup for the TNT block =)
    Mineserver::get()->map(map)->createPickupSpawn(centerX,centerY,centerZ,46,1,0,user, false);
    LOG2(INFO,"TNT Block was a dud!");
  }
  else
  {
    int radius = 3;
    int count = 0;
    std::queue<int8_t> temp;
    Packet pkt;

    for (int x = -radius; x <= radius; ++x) 
    {
      for (int y = -radius; y <= radius; ++y) 
      {
        for (int z = -radius; z <= radius; ++z)
        {
          int distance = sqrt( pow((double)0-x,2) + pow((double)0-y,2) + pow((double)0-z,2) );
          if( distance <= radius )
          {
            //std::cout << "Added ( " << x << ", " << y << ", " << z << " )";
            if(rb(centerX + x, centerY + y, centerZ + z,map,user))
            {
              temp.push(x);
              temp.push(y);
              temp.push(z);
              ++count;
            }
          }
        }
      }
    }

    //Explosion
    pkt << Protocol::explosion( (double)centerX, (double)centerY, (double)centerZ, (float)radius, (int32_t)count );
    for( int i = 0; i < count * 3; i++)
    {
      pkt << temp.front();
      temp.pop();
    }

    user->sendAll(pkt);

    //LOG2(INFO,"TNT Block exploded!");
  }
}

bool BlockTNT::onPlace(User* user, int16_t newblock, int32_t x, int8_t y, int32_t z, int map, int8_t direction)
{
  uint8_t oldblock;
  uint8_t oldmeta;
  
  if (!Mineserver::get()->map(map)->getBlock(x, y, z, &oldblock, &oldmeta))
  {
    revertBlock(user, x, y, z, map);
    return true;
  }
  /* Check block below allows blocks placed on top */
  if (!this->isBlockStackable(oldblock))
  {
    revertBlock(user, x, y, z, map);
    return true;
  }
  
  /* move the x,y,z coords dependent upon placement direction */
  if (!this->translateDirection(&x, &y, &z, map, direction))
  {
    revertBlock(user, x, y, z, map);
    return true;
  }
  
  if (this->isUserOnBlock(x, y, z, map))
  {
    revertBlock(user, x, y, z, map);
    return true;
  }
  
  if (!this->isBlockEmpty(x, y, z, map))
  {
    revertBlock(user, x, y, z, map);
    return true;
  }
  
  return false;
}

void BlockTNT::onStartedDigging(User* user, int8_t status, int32_t x, int8_t y, int32_t z, int map, int8_t direction)
{
  uint8_t block, metadata;
  explode(user,x,y,z,map);
  
}

bool BlockTNT::onInteract(User* user, int32_t x, int8_t y, int32_t z, int map)
{
  uint8_t block, metadata;
  return true;
}
