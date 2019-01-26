#include <Windows.h>
#include <iostream>
#include "Memory.h"
#include "Hide.h"
#include "Offsets.h"


Memory mem;

struct values
{
	DWORD lPlayer;
	DWORD bAddr;
	DWORD bAddrDwClientState;
}values;


void glowEsp(DWORD glowObj, int glowInd, float r, float g, float b, float a)
{
	mem.write<float>((glowObj + ((glowInd * 0x38) + 0x4)), r / 255);
	mem.write<float>((glowObj + ((glowInd * 0x38) + 0x8)), g / 255);
	mem.write<float>((glowObj + ((glowInd * 0x38) + 0xC)), b / 255);
	mem.write<float>((glowObj + ((glowInd * 0x38) + 0x10)), a / 255);
	mem.write<bool>((glowObj + ((glowInd * 0x38) + 0x24)), true);
	mem.write<bool>((glowObj + ((glowInd * 0x38) + 0x25)), false);
}

/*void RCS()
{
	Vector OldAimPunch;
	bool inAttack = (mem.read<int>(values.bAddr+offsets::dwForceAttack)==5);
	if(!inAttack)
	{
		OldAimPunch.x = OldAimPunch.y = OldAimPunch.z=0;
	}
	Vector AimPuch = mem.read<Vector>(values.lPlayer + offsets::m_Local + offsets::m_aimPunchAngle);
	Vector CurrentAngles = mem.read<Vector>(values.bAddrDwClientState+offsets::m_viewPunchAngle);
	Vector NewViewAngles;

	NewViewAngles.x = CurrentAngles.x+OldAimPunch.x-AimPuch.x;
	NewViewAngles.y = CurrentAngles.y+OldAimPunch.y-AimPuch.y;
	OldAimPunch = AimPuch;

	mem.write<Vector>(values.bAddrDwClientState + offsets::m_viewPunchAngle, NewViewAngles);
}*/


int main()
{
	int iCrosshairId;
	int pID;
	if(pID = mem.getProc(HIDE("csgo.exe")))
	{
		values.bAddr = mem.getModule(HIDE("client_panorama.dll"), pID);
		//values.bAddrDwClientState = mem.getModule(HIDE("engine.dll"), pID);
		while(!GetAsyncKeyState(0x75))
		{
			///////////////////////////////////////////////////////////////////////////////////////////////////
			values.lPlayer = mem.read<DWORD>(values.bAddr+offsets::dwLocalPlayer);
			DWORD glowObject = mem.read<DWORD>(values.bAddr+offsets::dwGlowObjectManager);
			int myTeam = mem.read<int>(values.lPlayer+offsets::m_iTeamNum);

			for(int i = 0; i< 64; i++)
			{
				DWORD entity = mem.read<DWORD>(values.bAddr+offsets::dwEntityList+i*0x10);
				if(entity!=NULL)
				{
					int glowIndx = mem.read<int>(entity+offsets::m_iGlowIndex);
					int entityTeam = mem.read<int>(entity+offsets::m_iTeamNum);
					
					if(myTeam==entityTeam)
					{
						glowEsp(glowObject, glowIndx, 0, 80, 0, 255);
					}
					else
					{
						glowEsp(glowObject, glowIndx, 128, 0, 128, 255);

						if(mem.read<bool>(values.lPlayer + offsets::m_bSpotted)==0)
							mem.write<bool>(entity + offsets::m_bSpotted, 1);
					}


				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////
			iCrosshairId = mem.read<int>(values.lPlayer+offsets::m_iCrosshairId);
			if(GetAsyncKeyState(VK_LSHIFT) && iCrosshairId != 0 && iCrosshairId < 64)
			{
				DWORD base = mem.read<DWORD>(values.bAddr+offsets::dwEntityList+((iCrosshairId-1)*0x10));
				if(mem.read<DWORD>(base+offsets::m_iTeamNum)!=myTeam)
				{
					mem.write<int>(values.bAddr+offsets::dwForceAttack, 5);
					Sleep(25);
					mem.write<int>(values.bAddr+offsets::dwForceAttack, 4);
				}
			}


			aimbot(getPlayer());

			//////////////////////////////////////////////////////////////////////////////////////////////////
			if(mem.read<int>(values.lPlayer+offsets::flashDuration)>0)
				mem.write<int>(values.lPlayer+offsets::flashDuration, 0);
			Sleep(1);
		}
	}

}