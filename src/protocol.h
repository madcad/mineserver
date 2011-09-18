#ifndef _PROTOCOL_H
#define _PROTOCOL_H
#include "constants.h"
#include "mob.h"
#include "tools.h"

/* This file introduces a basic abstraction over raw protocol packets format.
 * This is needed for varuios protocol updates - we need to change the raw format
 * only in one place. 
 * The implementation is totally in header to be inlined and optimized out.
 *
 * Maybe we need to add a lot of asserts there?
 */

class Protocol
{
  public:
    // Animation (http://mc.kev009.com/Protocol#Animation_.280x12.29)
    static Packet animation(int eid, int aid)
    {
      Packet ret;
      ret << (int8_t)PACKET_ANIMATION << (int32_t)eid << (int8_t)aid;
      return ret;
    }

    // Entity (http://mc.kev009.com/Protocol#Entity_.280x1E.29)
    static Packet entity( int32_t EID )
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY << (int32_t)EID;
      return ret;
    }

    // Entity Status (http://mc.kev009.com/Protocol#Entity_Status_.280x26.29)
    static Packet entityStatus(int eid, int aid)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_STATUS << (int32_t)eid << (int8_t)aid;
      return ret;
    }

    // Entity Metadata (http://mc.kev009.com/Protocol#Entity_Metadata_.280x28.29)
    static Packet entityMetadata(int eid, MetaData& metadata)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_METADATA << (int32_t)eid << metadata;
      return ret;
    }

    // Attach Entity (http://mc.kev009.com/Protocol#Attach_Entity_.280x27.29)
    static Packet attachEntity( int32_t EID, int32_t vehicleID)
    {
      Packet ret;
      ret << (int8_t)PACKET_ATTACH_ENTITY << (int32_t)EID << (int32_t)vehicleID;
      return ret;
    }

    // Mob Spawn (http://mc.kev009.com/Protocol#Mob_Spawn_.280x18.29)
    static Packet mobSpawn(int eid, int8_t type, double x, double y, double z, int yaw, int pitch, MetaData& metadata)
    {
      // Warning! This converts absolute double coordinates to absolute integer coordinates!
      Packet ret;
      ret << (int8_t)PACKET_MOB_SPAWN << (int32_t)eid << (int8_t)type
          << (int32_t)(x * 32) << (int32_t)(y * 32) << (int32_t)(z * 32)
          << (int8_t)yaw << (int8_t)pitch << metadata;
      return ret;
    }

    static Packet mobSpawn(Mob& mob)
    {
      return Protocol::mobSpawn(mob.UID, mob.type, mob.x, mob.y, mob.z, mob.yaw, mob.pitch, mob.metadata);
    }

    // Destory Entity (http://mc.kev009.com/Protocol#Destroy_Entity_.280x1D.29)
    static Packet destroyEntity(int eid)
    {
      Packet ret;
      ret << (int8_t)PACKET_DESTROY_ENTITY << (int32_t)eid;
      return ret;
    }

    // Entity Teleport (http://mc.kev009.com/Protocol#Entity_Teleport_.280x22.29)
    static Packet entityTeleport(int eid, double x, double y, double z, int yaw, int pitch)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_TELEPORT << (int32_t)eid
          << (int32_t)(x * 32) << (int32_t)(y * 32) << (int32_t)(z * 32)
          << (int8_t)yaw << (int8_t)pitch;
      return ret;
    }

    // Entity Look (http://mc.kev009.com/Protocol#Entity_Look_.280x20.29)
    static Packet entityLook(int eid, int yaw, int pitch)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_LOOK << (int32_t)eid << (int8_t)yaw << (int8_t)pitch;
      return ret;
    }
    
    static Packet entityLook(int eid, double yaw, double pitch)
    {
      return entityLook(eid, angleToByte(yaw), angleToByte(pitch));
    }

    // Entity Relative Move (http://mc.kev009.com/Protocol#Entity_Relative_Move_.280x1F.29)
    static Packet entityRelativeMove(int eid, double dx, double dy, double dz)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_RELATIVE_MOVE << (int32_t)eid
          << (int8_t)(dx * 32) << (int8_t)(dy * 32) << (int8_t)(dz * 32);
      return ret;
    }
    
    // Entity Look and Relative Move (http://mc.kev009.com/Protocol#Entity_Look_and_Relative_Move_.280x21.29)
    static Packet entityLookRelativeMove(int eid, double dx, double dy, double dz, int yaw, int pitch)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_LOOK_RELATIVE_MOVE << (int32_t)eid
          << (int8_t)(dx * 32) << (int8_t)(dy * 32) << (int8_t)(dz * 32)
          << (int8_t)yaw << (int8_t)pitch;
      return ret;
    }

    // Entity Velocity (http://mc.kev009.com/Protocol#Entity_Velocity_.280x1C.29)
    static Packet entityVelocity( int32_t EID, int16_t x, int16_t y, int16_t z )
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_VELOCITY << (int16_t)x << (int16_t)y << (int16_t)z;
      return ret;
    }

    // Login Response (http://mc.kev009.com/Protocol#Server_to_Client)
    static Packet loginResponse(int eid)
    {
      Packet ret;
      //TODO: Max Players, Max Height, Dimension, Server Mode
      ret << (int8_t)PACKET_LOGIN_RESPONSE << (int32_t)eid << std::string("") << (int64_t)0 << (int32_t)0 << (int8_t)0 << (int8_t)2 << (int8_t)-128 << (int8_t)50;
      return ret;
    }

    // Handshake (http://mc.kev009.com/Protocol#Handshake_.280x02.29)
    static Packet handshake( std::string hash )
    {
      Packet ret;
      ret << (int8_t)PACKET_HANDSHAKE << hash;
      return ret;
    }

    // Chat Message (http://mc.kev009.com/Protocol#Chat_Message_.280x03.29)
    static Packet chatMessage( std::string message )
    {
      Packet ret;
      ret << (int8_t)PACKET_HANDSHAKE << message;
      return ret;
    }

    // Spawn Position (http://mc.kev009.com/Protocol#Spawn_Position_.280x06.29)
    static Packet spawnPosition(int x, int y, int z)
    {
      Packet ret;
      ret << (int8_t)PACKET_SPAWN_POSITION << (int32_t)x << (int32_t)y << (int32_t)z;
      return ret;
    }

    // Time Update (http://mc.kev009.com/Protocol#Time_Update_.280x04.29)
    static Packet timeUpdate(int64_t time)
    {
      Packet ret;
      ret << (int8_t)PACKET_TIME_UPDATE << (int64_t)time;
      return ret;
    }

    // Kick (http://mc.kev009.com/Protocol#Disconnect.2FKick_.280xFF.29)
    static Packet kick(std::string msg)
    {
      Packet ret;
      ret << (int8_t)PACKET_KICK << msg;
      return ret;
    }

    // Named Entity Spawn (http://mc.kev009.com/Protocol#Named_Entity_Spawn_.280x14.29)
    static Packet namedEntitySpawn(int eid, std::string nick, double x, double y, double z, int yaw, int pitch, int item)
    {
      Packet ret;
      ret << (int8_t)PACKET_NAMED_ENTITY_SPAWN << (int32_t)eid << nick
          << (int32_t)(x * 32) << (int32_t)(y * 32) << (int32_t)(z * 32)
          << (int8_t)yaw << (int8_t)pitch << (int16_t)item;
      return ret;
    }

    // Collect Item (http://mc.kev009.com/Protocol#Collect_Item_.280x16.29)
    static Packet collectItem(int itemEid, int eid)
    {
      Packet ret;
      ret << (int8_t)PACKET_COLLECT_ITEM << (int32_t)itemEid << (int32_t)eid;
      return ret;
    }

    // Pre Chunk (http://mc.kev009.com/Protocol#Pre-Chunk_.280x32.29)
    static Packet preChunk(int x, int z, bool create)
    {
      Packet ret;
      ret << (int8_t)PACKET_PRE_CHUNK << (int32_t)x << (int32_t)z << (int8_t)(create ? 1 : 0);
      return ret;
    }

    // Player Position and Look (http://mc.kev009.com/Protocol#Player_Position_.26_Look_.280x0D.29)
    static Packet playerPositionAndLook(double x, double y, double stance, double z, float yaw, float pitch, bool onGround)
    {
      Packet ret;
      ret << (int8_t)PACKET_PLAYER_POSITION_AND_LOOK << x << y << stance << z << yaw << pitch << (int8_t)(onGround ? 1 : 0);
      return ret;
    }

    // Respawn (http://mc.kev009.com/Protocol#Respawn_.280x09.29)
    static Packet respawn(int world = 0)
    {
      Packet ret;
      ret << (int8_t)PACKET_RESPAWN << (int8_t)world << (int8_t)1 << (int8_t)0 << (int8_t)128 << (int64_t)0;
      return ret;
    }

    // Update Health (http://mc.kev009.com/Protocol#Update_Health_.280x08.29)
    static Packet updateHealth(int health)
    {
      Packet ret;
      ret << (int8_t)PACKET_UPDATE_HEALTH << (int16_t)health << (int16_t)20 << (float)5.0;
      return ret;
    }

    // Entity Equipment (http://mc.kev009.com/Protocol#Entity_Equipment_.280x05.29)
    static Packet entityEquipment(int eid, int slot, int type, int damage)
    {
      Packet ret;
      ret << (int8_t)PACKET_ENTITY_EQUIPMENT << (int32_t)eid << (int16_t)slot << (int16_t)type << (int16_t)damage;
      return ret;
    }

    // Player List Item (http://mc.kev009.com/Protocol#Player_List_Item_.280xC9.29)
    static Packet playerListItem(std::string name, int8_t online, int16_t ping)
    {
      Packet ret;
      ret << (int8_t)PACKET_PLAYER_LIST_ITEM << (std::string)name << (int8_t)online << (int16_t)ping;
      return ret;
    }

    // Set Slot (http://mc.kev009.com/Protocol#Set_slot_.280x67.29)
    static Packet setSlot( int8_t windowID, int16_t slot, int16_t itemID, int8_t itemCount=0, int16_t itemUses=0)
    {
      Packet ret;
      ret << (int8_t)PACKET_SET_SLOT << (int8_t)windowID << (int16_t)slot << (int16_t)itemID;

      if(itemID != (int16_t)-1)
      {
        ret << (int8_t)itemCount << (int16_t)itemUses;
      }
      return ret;
    }

    // Update Sign (http://mc.kev009.com/Protocol#Update_Sign_.280x82.29)
    static Packet updateSign( int32_t x, int16_t y, int32_t z, std::string text1, std::string text2, std::string text3, std::string text4 )
    {
      Packet ret;
      ret << (int8_t)PACKET_UPDATE_SIGN << (int32_t)x << (int16_t)y << (int32_t)z 
          << (std::string)text1 << (std::string)text2 << (std::string)text3 << (std::string)text4;
      return ret;
    }

    // Pickup Spawn (http://mc.kev009.com/Protocol#Pickup_Spawn_.280x15.29)
    static Packet pickupSpawn( int32_t EID, int16_t item, int8_t count, int16_t health, int32_t x, int32_t y, int32_t z, int8_t rotation, int8_t pitch, int8_t roll)
    {
      Packet ret;
      ret << (int8_t)PACKET_PICKUP_SPAWN << (int32_t)EID << (int16_t)item << (int8_t)count << (int16_t)health 
          << (int32_t)x << (int32_t)y << (int32_t)z
          << (int8_t)rotation << (int8_t)pitch << (int8_t)roll;
      return ret;
    }

    // Add Object (http://mc.kev009.com/Protocol#Add_Object.2FVehicle_.280x17.29)
    static Packet addObject( int32_t EID, int8_t type, int32_t x, int32_t y, int32_t z, int32_t flag )
    {
      //TODO: there are three values after the flag, as of 9/18/2011 the wiki is not sure for a fact what it represents.
      Packet ret;
      ret << (int8_t)PACKET_ADD_OBJECT << (int32_t)EID << (int8_t)type << (int32_t)x << (int32_t)y << (int32_t)z << (int32_t)flag;
      return ret;
    }

    // Map Chunk (http://mc.kev009.com/Protocol#Map_Chunk_.280x33.29)
    // Doesn't deal with the map data. To be updated
    static Packet mapChunk( int32_t x, int16_t y, int32_t z, int8_t sizeX, int8_t sizeY, int8_t sizeZ )
    {
      Packet ret;
      ret << (int8_t)PACKET_MAP_CHUNK << (int32_t)x << (int16_t)y << (int32_t)z << (int8_t)sizeX << (int8_t)sizeY << (int8_t)sizeZ;
      return ret;
    }

    // Block Change (http://mc.kev009.com/Protocol#Block_Change_.280x35.29)
    static Packet blockChange( int32_t x, int8_t y, int32_t z, int8_t block, int8_t meta )
    {
      Packet ret;
      ret << (int8_t)PACKET_BLOCK_CHANGE << (int32_t)x << (int8_t)y << (int32_t)z << (int8_t)block << (int8_t)meta;
      return ret;
    }

    // Multi Block (http://mc.kev009.com/Protocol#Multi_Block_Change_.280x34.29)
    // Doesn't deal with map data. To be updated
    static Packet multiBlock( int32_t chunkX, int32_t chunkZ, int16_t size )
    {
      Packet ret;
      ret << (int8_t) PACKET_MULTI_BLOCK_CHANGE << (int32_t)chunkX << (int32_t)chunkZ << (int16_t)size;
      return ret;
    }

    // Block Action (http://mc.kev009.com/Protocol#Block_Action_.280x36.29)
    static Packet blockAction( int32_t x, int16_t y, int32_t z, int8_t instrument, int8_t pitch )
    {
      Packet ret;
      ret << (int8_t)PACKET_BLOCK_ACTION << (int32_t)x << (int16_t)y << (int32_t)z << (int8_t)instrument << (int8_t)pitch;
      return ret;
    }

    // Open Window (http://mc.kev009.com/Protocol#Open_window_.280x64.29)
    // To be updated. Still needs to be done.
    static Packet openWindow( int8_t windowID, int8_t inventoryType, std::string windowType, int8_t slots )
    {
      Packet ret;
      ret << (int8_t)PACKET_OPEN_WINDOW << (int8_t)windowID  << (int8_t)inventoryType << (std::string)windowType << (int8_t)slots;
      return ret;
    }

    // Update Progress Bar (http://mc.kev009.com/Protocol#Update_progress_bar_.280x69.29)
    static Packet updateProgressBar( int8_t windowID, int16_t type, int16_t value )
    {
      Packet ret;
      ret << (int8_t)PACKET_UPDATE_PROGRESS_BAR << (int8_t)windowID << (int16_t)type << (int16_t)value;
      return ret;
    }

    // Transaction (http://mc.kev009.com/Protocol#Transaction_.280x6A.29)
    static Packet transaction( int8_t windowID, int16_t actionNumber, int16_t accepted )
    {
      Packet ret;
      ret << (int8_t)PACKET_TRANSACTION << (int8_t)windowID << (int16_t)actionNumber << (int8_t)accepted;
      return ret;
    }
};
#endif
