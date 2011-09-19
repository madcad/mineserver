#include <math.h>

#include "projectile.h"
#include "../packets.h"
#include "../mineserver.h"
#include "../map.h"
#include "../projectileManager.h"

uint32_t generateEID();

bool ItemProjectile::affectedItem(int item) const
{
  switch (item)
  {
  case ITEM_SNOWBALL:
  case ITEM_EGG:
  case ITEM_BOW:
    return true;
  }
  return false;
}

void ItemProjectile::onRightClick(User* user, Item* item)
{
  int8_t projID = 0;
  switch (item->getType())
  {
  case ITEM_SNOWBALL:
    projID = 61;
    break;
  case ITEM_EGG:
    projID = 62;
    break;
  case ITEM_BOW:
    projID = 60;
    break;
  }

  if(projID != 0)
  {
    if(projID == 60) // Bow and Arrow
    {
      //TODO: Make a function of inventory that checks for an item in this manner.
      //      Make a function to spend/remove one of a certian item in this manner.
      bool foundArrow = false;
      // Hotbar
      for( int i = 36; i < 45; i++ )
      {
        if( user->inv[i].getType() == ITEM_ARROW )
        {
          foundArrow = true;
          user->inv[i].decCount();
          i = 45;
        }
      }
      //Inventory
      if(foundArrow == false)
      {
        for(int i = 9; i < 36; i++)
        {
          if( user->inv[i].getType() == ITEM_ARROW )
          {
            foundArrow = true;
            user->inv[i].decCount();
            i = 36;
          }
        }
      }
      if(!foundArrow)
        return;
      Mineserver::get()->map(user->pos.map)->sendProjectileSpawn(user, projID);
    }
    else
    {
      item->decCount();
      Mineserver::get()->map(user->pos.map)->sendProjectileSpawn(user, projID);
    }
  }
}

ItemProjectile::ItemProjectile()
{
  setEID();
}

ItemProjectile::ItemProjectile(User* user, int8_t ID)
{
  setEID();
  setProjectileID(ID);

  //Intials used for calculations
  intialHeight = user->pos.y;
  intialAngle = user->pos.pitch;
  totalHorizontalDistance = 0;
  intialVelocity = 9000;

  setUser(user);
  inFlight = true;
  ticksInFlight = 0;

  //
  // Below is for position and velocity
  //
  double tempx = ( user->pos.x * 32 );
  double tempy = ( user->pos.y + 1.5f ) * 32;
  double tempz = ( user->pos.z * 32 );
  int distFromUser = 50; // 32 is one whole block away, 50 seems to work while walking

  //userYaw makes the player's yaw between 0-359
  float userYaw = ((int)user->pos.yaw % 360) + (user->pos.yaw - (int)user->pos.yaw);
  if( user->pos.yaw < 0 )
  {
    userYaw += 360;
  }

  if( user->pos.pitch < 0 )
  {
    tempy = ( distFromUser * abs(sin(user->pos.pitch / 90.f))) + (user->pos.y + 1.5f) * 32;
  }
  else if ( user->pos.pitch > 0 )
  {
    tempy = -( distFromUser * abs(sin(user->pos.pitch / 90.f))) + (user->pos.y + 1.5f) * 32;
  }

  if( userYaw != 0 && userYaw != 180 )
  {
    tempx = ( ( -distFromUser * sin(user->pos.yaw * (M_PI / 180.0f) ) ) * cos( user->pos.pitch * (M_PI / 180.0f) ) ) + (user->pos.x * 32);
  }
  if( userYaw != 90 && userYaw != 270 )
  {
    tempz = ( ( distFromUser * cos(user->pos.yaw * (M_PI / 180.0f) ) ) * cos( user->pos.pitch * (M_PI / 180.0f) ) ) + (user->pos.z * 32);
  }

  //vec pos = vec((int)tempx, (int)tempy, (int)tempz);
  setPosition((int)tempx, (int)tempy, (int)tempz, user->pos.map, user->pos.yaw, user->pos.pitch);
  //std::cout << "tempz: " << tempz << ", " << tempz / 32 << std::endl;

  //The 9000 and 14000 are based off of trial and error.  Not calculated to exactly mimic notchian projectile.
  // 9000 might = 1.125 m/s
  // 14000 might = 1.75 m/s
  const int horizontalVelocity = 1.125 * 8000;
  const int verticalVelocity = 1.75 * 8000;

  // Add ability to set velocity based on projectile type
  setVelocity((int)(sin(-(user->pos.yaw / 360.f) * 2.f * M_PI) * cos(user->pos.pitch * (M_PI / 180.0f)) * horizontalVelocity),
              (int)(sinf(-(user->pos.pitch / 90.f)) * verticalVelocity),
              (int)(cos(-(user->pos.yaw / 360.f) * 2.f * M_PI) * cos(user->pos.pitch * (M_PI / 180.0f)) * horizontalVelocity));

}

// http://en.wikipedia.org/wiki/Trajectory_of_a_projectile
void ItemProjectile::updatePosition()
{
  const float gravityConstant = -9.81;

  std::cout << /*"Distance: " << (9000.f / 2700) * ticksInFlight <<*/ "ticks: " << ticksInFlight;
  //std::cout << /*" cos(pitch): " << (int)cos(pos.pitch * (M_PI / 180) ) <<*/ " Displacement in x: " << (int)((9000.f / 540) * cos(pos.pitch * (M_PI / 180.f))) * ((float)getTicksInFlight() / 5.0F) << " Displacement in y: " << (((14000.f / 650) * sin(pos.pitch * (M_PI / 180.f))) * ((float)getTicksInFlight() / 5.0F)) - (0.5 * gravityConstant * pow(((float)getTicksInFlight() / 5.0F),2)) << std::endl;

  if(ticksInFlight == 1)
  {
    //Will be constant as the projectile flys
    displacementX = (int)((9000.f / 540) * cos(pos.pitch * (M_PI / 180.f))) * (ticksInFlight / 5.0F);
  }

  displacementY = (((14000.f / 650) * sin(pos.pitch * (M_PI / 180.f))) * (ticksInFlight / 5.0F)) - (0.5 * gravityConstant * pow((ticksInFlight / 5.0F),2));

  //Just do z+ for now

  pos.z = pos.z + ( displacementX * 32 );
  pos.y = intialHeight - displacementY;

  std::cout << " posX: " << (pos.x / 32 ) << " posY: " << pos.y << " posZ: " << (pos.z / 32) << std::endl;
}