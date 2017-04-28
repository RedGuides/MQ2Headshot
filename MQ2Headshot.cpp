// MQ2Headshot.cpp : Defines the entry point for the DLL application.
// Author: Naes
#include "../MQ2Plugin.h"

PreSetup("MQ2Headshot");
PLUGIN_VERSION(1.3);

#define HEADSHOT_HUMANOID 1
#define HEADSHOT_UNDEAD 3
#define HEADSHOT_SUMMONED 28
#define HEADSHOT_ANYTHING -1

// can leave as-is in case other classes get similar in future
PCHAR szHeadshotLabel[] = { 
    "", // 0x0
    "", // Warrior
    "UNDEAD", // Cleric
    "UNDEAD", // Paladin
    "HEADSHOT", // Ranger
    "UNDEAD", // Shadow Knight
    "SUMMONED", // Druid
    "", // Monk
    "", // Bard
    "ASSASSINATE", // Rogue
    "", // Shaman
    "UNDEAD", // Necromancer
    "", // Wizard
    "SUMMONED", // Magician
    "", // Enchanter
    "", // Beastlord
    "DECAPITATE" // Berserker
};

PSPAWNINFO pCharFix;
int showHeadCount = -1;
char INISection[MAX_STRING];



bool CanHeadshot(PSPAWNINFO pNewSpawn)
{
    /*static const/**/ DWORD headshotAAIndex = GetAAIndexByName("Headshot"); //  = 13573;
    /*static const/**/ DWORD anatomyAAIndex = GetAAIndexByName("Anatomy");
    /*static const/**/ DWORD slayUndeadAAIndex = GetAAIndexByName("Slay Undead");
    /*static const/**/ DWORD decapitationAAIndex = GetAAIndexByName("Decapitation");
    /*static const/**/ DWORD turnUndeadAAIndex = GetAAIndexByName("Turn Undead");
    /*static const/**/ DWORD turnSummonedAAIndex = GetAAIndexByName("Turn Summoned");
    /*static const/**/ DWORD originAAIndex = GetAAIndexByName("Origin");
	
    if (GetSpawnType(pNewSpawn) == NPC && 
         gGameState==GAMESTATE_INGAME) // not at char select
    {
        _ALTABILITY* aa = NULL;
        int bodyType = HEADSHOT_HUMANOID;
        
        int maxKillLevel = 0; // set to highest headshotable, label all under it

		if (PCHARINFO pCharInf = GetCharInfo()) {
			if (pCharFix = pCharInf->pSpawn) {
				switch (pCharFix->mActorClient.Class)
				{
				case Ranger:
					aa = pAltAdvManager->GetAAById(headshotAAIndex);
					maxKillLevel = aa->MinLevel - 1;
					break;

				case Rogue:
					aa = pAltAdvManager->GetAAById(anatomyAAIndex);
					if (pCharFix->Level >= 60)
						maxKillLevel = aa->MinLevel - 1;
					break;

				case Paladin:
					bodyType = HEADSHOT_UNDEAD;
					aa = pAltAdvManager->GetAAById(slayUndeadAAIndex);
					if (aa)
						maxKillLevel = 255; // rk1=??
					break;

				case Berserker:
					bodyType = HEADSHOT_ANYTHING;
					aa = pAltAdvManager->GetAAById(decapitationAAIndex);
					maxKillLevel = aa->MinLevel - 1;
					break;

				case Cleric:
					bodyType = HEADSHOT_UNDEAD;
					aa = pAltAdvManager->GetAAById(turnUndeadAAIndex);
					maxKillLevel = 255;
					break;
				case Mage:
					bodyType = HEADSHOT_SUMMONED;
					aa = pAltAdvManager->GetAAById(turnSummonedAAIndex);
					maxKillLevel = 255;
					break;
				case Druid:
					bodyType = HEADSHOT_SUMMONED;
					aa = pAltAdvManager->GetAAById(turnSummonedAAIndex);
					maxKillLevel = 255;
					break;
				case Shadowknight:
					bodyType = HEADSHOT_UNDEAD;
					aa = pAltAdvManager->GetAAById(originAAIndex);
					if (aa)
						maxKillLevel = 255;
					break;
				case Necromancer:
					bodyType = HEADSHOT_UNDEAD;
					aa = pAltAdvManager->GetAAById(originAAIndex);
					if (aa)
						maxKillLevel = 255;
					break;
				}
			}
		}
        if ((bodyType == HEADSHOT_ANYTHING || GetBodyType(pNewSpawn) == bodyType) && 
            pNewSpawn->Level <= maxKillLevel)
                return true;
    }

    return false;
}

PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
    if (CanHeadshot(pNewSpawn))
    {
        char new_name[MAX_STRING];
        sprintf_s(new_name, "%s: %d", szHeadshotLabel[pCharFix->mActorClient.Class], pNewSpawn->Level);
        strcpy_s(pNewSpawn->Lastname, new_name);    
    }
}

void WriteHeadcountSetting(int activate)
{
    CHAR szMsg[MAX_STRING]={0};
    CHAR szTemp[MAX_STRING]={0};
    
    sprintf_s(szTemp, "%d", activate);

    showHeadCount = activate;
    WritePrivateProfileString(INISection, "ShowHeadcount", szTemp, INIFileName);

    sprintf_s(szMsg, "[MQ2Headshot] /headcount setting: %d", activate);
    WriteChatColor(szMsg, USERCOLOR_DEFAULT);
}

void HeadcountCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    if (strlen(szLine) != 0)
    {
        CHAR Arg1[MAX_STRING] = {0}; 
        GetArg(Arg1, szLine, 1); 

        if (!_strnicmp(Arg1, "off", 3))
        {
            WriteHeadcountSetting(0);
            return;
        } else if (!_strnicmp(Arg1, "on", 2))
        {
            WriteHeadcountSetting(1);
        }
    }

    PSPAWNINFO pSpawns = NULL;
    if (ppSpawnManager && pSpawnList)
    pSpawns = (PSPAWNINFO)pSpawnList;

    unsigned int count = 0;
    while (pSpawns)
    {
        if (CanHeadshot(pSpawns))
            ++count;

        pSpawns = pSpawns->pNext;
    }
    
    char buffer[MAX_STRING];
    sprintf_s(buffer, "[MQ2Headshot] # of %s mobs in %s: %d", szHeadshotLabel[pCharFix->mActorClient.Class],((PZONEINFO)pZoneInfo)->ShortName, count);
    WriteChatColor(buffer, USERCOLOR_DEFAULT);
}

PLUGIN_API VOID SetGameState(DWORD newGameState)
{
    DebugSpewAlways("MQ2Headshot::SetGameState(%d)", newGameState);
    // fix for first load
    if (newGameState == GAMESTATE_INGAME)
    {
        if (showHeadCount == -1)
        {
            if (!pCharFix && GetCharInfo() && GetCharInfo()->pSpawn)
                pCharFix = GetCharInfo()->pSpawn;
            sprintf_s(INISection,"%s_%s", pCharFix->Name, EQADDR_SERVERNAME);
            showHeadCount = GetPrivateProfileInt(INISection, "ShowHeadcount", 1, INIFileName);

            PSPAWNINFO pNewSpawns = NULL;
            if (ppSpawnManager && pSpawnList)
                pNewSpawns = (PSPAWNINFO)pSpawnList;

            while (pNewSpawns)  // clear the lastnames
            {
                OnAddSpawn(pNewSpawns);

                pNewSpawns = pNewSpawns->pNext;
            }
        }
    
        if (showHeadCount == 1)
            HeadcountCommand(pCharFix, "");
    
    } else if (newGameState == GAMESTATE_CHARSELECT)
    {
        showHeadCount = -1;
    }
}

PLUGIN_API VOID InitializePlugin(VOID)
{
    if (GetCharInfo() && GetCharInfo()->pSpawn)
        pCharFix = GetCharInfo()->pSpawn;
    
    AddCommand("/headcount", HeadcountCommand);

    WriteChatColor("[MQ2Headshot] \agLoaded", USERCOLOR_DEFAULT);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Headshot");

    PSPAWNINFO pClearSpawns = NULL;
    if (ppSpawnManager && pSpawnList)
        pClearSpawns = (PSPAWNINFO)pSpawnList;

    while (pClearSpawns)  // clear the lastnames
    {
        if (CanHeadshot(pClearSpawns))
            strcpy_s(pClearSpawns->Lastname, "");

        pClearSpawns = pClearSpawns->pNext;
    }

    
    RemoveCommand("/headcount"); 
}