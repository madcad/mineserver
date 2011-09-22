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

#include "mineserver.h"
#include "projectileManager.h"
#include "items/projectile.h"
#include "tools.h"
#include "map.h"
#include <iostream>

void ProjectileManager::update()
{
  //TODO: Check for collisions with enviornment, other players, mobs
  //      when not flying place ItemProjectiles in another list to process timeout don't need to check that so often.
  //          Put in the every minute one, if arrow is picked up then remove from timeout list.
  //      Make projectiles inflict damage to target when hit
  //
  for (ProjectileContainer::iterator it = m_aliveProjectiles.begin(); it != m_aliveProjectiles.end(); )
  {
    ItemProjectilePtr currentProjectile = *it;

    currentProjectile.get()->updatePosition();
    uint8_t block;
    uint8_t meta;

    //Check the blocks that the arrow flew through between that 200ms
    if(currentProjectile.get()->getTicksInFlight() > 0 )
    {
      //Change the ticks in flight modifier if you move increment!!
      float temp4 = (int)((9000.f / 540) * cos(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) * ((currentProjectile.get()->getTicksInFlight() - 1) / 5.0F);
      float temp5 = (int)((9000.f / 540) * cos(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) * (currentProjectile.get()->getTicksInFlight() / 5.0F);
      float temp6 = temp5 - temp4;

      const float gravityConstant = -9.81;

      float ytemp4 = (((14000.f / 560) * sin(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) * ((currentProjectile.get()->getTicksInFlight()-1) / 5.0F)) - (0.5 * gravityConstant * pow(((currentProjectile.get()->getTicksInFlight()-1) / 5.0F),2));
      float ytemp5 = (((14000.f / 560) * sin(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) * (currentProjectile.get()->getTicksInFlight() / 5.0F)) - (0.5 * gravityConstant * pow((currentProjectile.get()->getTicksInFlight() / 5.0F),2));
      float ytemp6 = ytemp5 - ytemp4;
      std::cout << "temp6: " << temp6 << " ytemp6: " << ytemp6 << std::endl;

      //horizontal
      for( int i = 0; i < temp6+1; i++)
      {
        float modifierHorizontal = sin(-(currentProjectile.get()->getPosition().yaw / 360.f) * 2.f * M_PI) * (((9000.f / 540) * cos(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) / 5 ) * (((float)currentProjectile.get()->getTicksInFlight() / 5)) + ((1)*i); // 0.2/temp6
        //std::cout << "modifierHorizontal" << modifierHorizontal << std::endl;
        //Shooting straight up and down?
        if( (int)((9000.f / 540) * cos(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) * (currentProjectile.get()->getTicksInFlight() / 5.0F) == 0 )
        {
          modifierHorizontal = 0;
        }
        float positionZ = (currentProjectile.get()->getPosition().z / 32) + modifierHorizontal;
        float positionX = (currentProjectile.get()->getPosition().x / 32) + modifierHorizontal;

        //Vertical
        for( int o = 0; o < ytemp6; o++)
        {
          float modifierVertical = ((((14000.f / 650) * sin(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) / 5 ) * ((float)currentProjectile.get()->getTicksInFlight() / 5.0F)) - ((float)0.5 * (gravityConstant / 5 ) * pow((currentProjectile.get()->getTicksInFlight() / 5.0F) + ((0.2/ytemp6)*o),2));
          float positionY = currentProjectile.get()->getPosition().y - modifierVertical;
          //std::cout << "positionZ: " << (currentProjectile.get()->getPosition().z / 32) << " + ( " << ((9000.f / 540) * cos(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) / 5 << " * " << ((float)currentProjectile.get()->getTicksInFlight() / 5) + ((0.2/temp6)*i) << " ) = " << positionZ << std::endl;
      
          //std::cout << "modifierHorizontal: " << modifierHorizontal << " modifierVertical: " << modifierVertical << std::endl;
          std::cout << " i: " << i << " o: " << o << " X: " << positionX << " Y: " << positionY << " Z: " << positionZ /*<< " modifierVertical: " << modifierVertical << ", " << ((((14000.f / 650) * sin(currentProjectile.get()->getPosition().pitch * (M_PI / 180.f))) / 5 ) * ((float)currentProjectile.get()->getTicksInFlight() / 5.0F)) << " - " << ((float)0.5 * gravityConstant * pow((currentProjectile.get()->getTicksInFlight() / 5.0F) + ((0.2/ytemp6)*o),2))*/ << std::endl;
          Mineserver::get()->map(currentProjectile.get()->getPosition().map)->getBlock(positionX, positionY, positionZ, &block, &meta);
          if(block != BLOCK_AIR)
          {
            //-1 shouldn't be needed in final solution, might need differences depending on player location
            Mineserver::get()->map(currentProjectile.get()->getPosition().map)->setBlock(positionX-1, positionY, positionZ, BLOCK_LEAVES, 0);
            Mineserver::get()->map(currentProjectile.get()->getPosition().map)->sendBlockChange(positionX-1, positionY, positionZ, BLOCK_LEAVES, 0);
            std::cout << "Collision occured! X:" << positionX << " Y:" << positionY << " Z:" << positionZ << std::endl;
            currentProjectile.get()->setInFlight(false);
            i = temp6+1;
            o = ytemp6;
            continue;
          }
        }
      }
    }


    if( !currentProjectile.get()->isInFlight() || currentProjectile.get()->getTicksInFlight() >= 30 ) //remove timeout
    {
      //uint8_t entityData[5];
      //entityData[0] = 0x1d; // Destroy entity;
      //putSint32(&entityData[1], currentProjectile.get()->getEID());
      //currentProjectile.get()->getUser()->sendAll(entityData, 5);

      it = m_aliveProjectiles.erase(it);
      std::cout << "Arrow removed" << std::endl;
    }
    else
    {
      ++it;
    }

    currentProjectile.get()->incrementTicksInFlight();
  }
}

void ProjectileManager::addProjectile(User* user, int8_t projID)
{
  m_aliveProjectiles.push_front(ItemProjectilePtr(new ItemProjectile(user, projID)));
}

void ProjectileManager::addProjectile(ItemProjectilePtr temp)
{
  m_aliveProjectiles.push_front(temp);
}