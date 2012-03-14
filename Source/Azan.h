#ifndef NOTIFY_INCLUDED
#define NOTIFY_INCLUDED

#include <libnotify/notify.h>
//C++ Include
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
//Local Include
#include "mohasebegar_owghat.h"
//Audio LIbrary
#include <stdio.h>
#include <stdlib.h>   // Declaration for exit()
//#define LOCAL_RESOURCES "resource"
#define LOCAL_RESOURCES OSHAREE_RESOURCES
using namespace std;

class azanCore
{
public :
    azanCore(owghat InputOwghat,vector<string> InputString , char * InputPath);
    void startControl();
    void showOwghat();
private:
    void sayGhoroob(string azanName,int azanID);
    int findNazdik();
    int waitAzan();
    int waitVozoo();
    bool WaitTime(tm endTime);
    void sayAzan(string azanName);
    void sayVozoo(string azanName);

    string getAzanName(int a);

    owghat local;
    vector<string> Slocal;

    //Resource Patch
    string minberPatch , tasbihPatch, infoPatch ,sajadePath , masjedPath;
    char* mainPath;
};

void neshandadan_notify(const char *title,const char *matn,const char * file_name);

#endif // NOTIFY_INCLUDED
