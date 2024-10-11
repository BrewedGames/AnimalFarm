// #define SDL_MAIN_HANDLED
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <string>
#include <iostream>
#include <SimpleIni.h>
#include "SceneManager.h"
#include "Bridge.h"



bool isRunning = true;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;



using namespace std;

int main(int argc, char *args[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile("config.ini");
    SCREEN_WIDTH = std::stoi(ini.GetValue("Display", "width"));
    SCREEN_HEIGHT = std::stoi(ini.GetValue("Display", "height"));

    Bridge bridge;
    bridge.SetupBridge();

    SceneManager *gsm = new SceneManager();
    if (gsm->Initialize(ini.GetValue("Project", "name"), SCREEN_WIDTH, SCREEN_HEIGHT) == true)
    {
        gsm->Run();
    }
    delete gsm;
    _CrtDumpMemoryLeaks();
    exit(0);

    return 0;
}