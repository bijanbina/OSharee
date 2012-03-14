#include "Azan.h"

void neshandadan_notify(const char *title,const char *matn,const char * file_name)
{
    NotifyNotification *message_notify;

    message_notify = notify_notification_new (title, matn,file_name);

    notify_notification_set_timeout (message_notify, 10000);
    notify_notification_show(message_notify,NULL);

    g_object_unref(G_OBJECT(message_notify));
}

int azanCore::findNazdik()
{
    time_t ticks, Now;
    time ( &Now );
    ticks=mktime( &local.Maghrib);
    int min = ticks - Now;
    int minM = 5;
    if (min < 0)
        return -1;
    if(mktime(&local.Sunset) - Now < min && 0 < mktime(&local.Sunset) - Now)
    {
        min = mktime(&local.Sunset) - Now ;
        minM = 4;
    }
    if((mktime(&local.Dhuhr) - Now )< min && 0 < (mktime(&local.Dhuhr) - Now ))
    {
        min = mktime(&local.Dhuhr) - Now ;
        minM = 3;
    }

    if((mktime(&local.Sunrise) - Now )< min && 0 < (mktime(&local.Sunrise) - Now ))
    {
        min = mktime(&local.Sunrise) - Now ;
        minM = 2;
    }

    if((mktime(&local.Fajr) - Now )< min && 0 < (mktime(&local.Fajr) - Now ))
    {
        min = mktime(&local.Fajr) - Now ;
        minM = 1;
    }
    return minM;
}

int azanCore::waitAzan()
{
    int a = findNazdik();
    int waitToAzan;
    time_t  Now;
    time ( &Now );
    switch (a)
    {
    case 1:
        waitToAzan = mktime(&local.Fajr) - Now;
        break;
    case 2:
        waitToAzan = mktime(&local.Sunrise) - Now;
        break;
    case 3:
        waitToAzan = mktime(&local.Dhuhr) - Now;
        break;
    case 4:
        waitToAzan = mktime(&local.Sunset) - Now;
        break;
    case 5:
        waitToAzan = mktime(&local.Maghrib) - Now;
        break;
    }
    if (waitToAzan < 0)
        return -1;
    sleep(waitToAzan);
    return a;
}

int azanCore::waitVozoo()
{
    int a = findNazdik();
    time_t  VozooTime;
    time_t  Now;
    tm           nextT;
    time ( &Now );
    switch (a)
    {
    case 1:
        nextT = local.Fajr;
        break;
    case 2:
        nextT = local.Sunrise;
        break;
    case 3:
        nextT = local.Dhuhr;
        break;
    case 4:
        nextT = local.Sunset;
        break;
    case 5:
        nextT = local.Maghrib;
        break;
    }
    VozooTime   = mktime(&nextT);
    VozooTime -= 300;
    nextT = *localtime(&VozooTime);
	while(WaitTime(nextT))
	{
		;
	}
    return a;
}

bool azanCore::WaitTime(tm endTime)
{
	time_t  Now;
    time ( &Now );
    int waitDuration = mktime(&endTime) - Now;
	if (waitDuration < 0)
        return false;
    if (waitDuration > 300)
    {
    	sleep(300);
    	return true;
    }
	sleep(waitDuration);
	return false;
}

void azanCore::startControl()
{
    showOwghat();
    string matn, azanName;
    bool isAzan;
    int a = findNazdik();
    while (a != -1)
    {
        azanName = getAzanName(a);
        isAzan = false;
        matn =  "\n";
        matn += azanName;
        matn += " : ";
        matn += Slocal[a -1];
        if (a ==1 || a== 3 || a == 5)
            isAzan = true;
        neshandadan_notify("وقت شرعی بعدی",matn.c_str(),LOCAL_RESOURCES"/seccade.png");
        if (isAzan)
        {
            waitVozoo();
            sayVozoo(azanName);
            waitAzan();
            sayAzan(azanName);
        }
        else
        {
            waitAzan();
            sayGhoroob(azanName ,a);
        }
        a = findNazdik();
    }
}

azanCore::azanCore(owghat InputOwghat,vector<string> InputString, char * InputPath)
{
    notify_init("OSharee");

    Slocal = InputString;
    local = InputOwghat;

    Slocal[3] = Slocal[4];
    Slocal[4] = Slocal[5];
}

void azanCore::sayAzan(string azanName)
{
    // Play the azan
    popen("mplayer "LOCAL_RESOURCES"/azan.ogg","w");

    azanName += " به افق تهران";
    sleep(1);
    neshandadan_notify(azanName.c_str(),"الله اکبر",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"الله اکبر",LOCAL_RESOURCES"/minber.png");
    sleep(16);
    neshandadan_notify(azanName.c_str(),"الله اکبر الله اکبر",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"اشهد ان لا اله الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"اشهد ان لا اله الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"اشهد ان محمدا رسول الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"اشهد ان محمدا رسول الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"اشهد ان مولانا",LOCAL_RESOURCES"/minber.png");
    sleep(10);
    neshandadan_notify(azanName.c_str(),"امیر المومنین علیا ولی الله",LOCAL_RESOURCES"/minber.png");
    sleep(14);
    neshandadan_notify(azanName.c_str(),"اشهد ان مولانا",LOCAL_RESOURCES"/minber.png");
    sleep(10);
    neshandadan_notify(azanName.c_str()," امیرالمومنین علیا حجت الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str()," حی علی الصلاة حی علی الصلاة",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"حی علی الفلاح حی علی الفلاح",LOCAL_RESOURCES"/minber.png");
    sleep(16);
    neshandadan_notify(azanName.c_str(),"حی علی خیر العمل",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"حی علی خیر العمل",LOCAL_RESOURCES"/minber.png");
    sleep(13);
    neshandadan_notify(azanName.c_str(),"الله اکبر الله اکبر",LOCAL_RESOURCES"/minber.png");
    sleep(15);
    neshandadan_notify(azanName.c_str(),"لا اله الا الله",LOCAL_RESOURCES"/minber.png");
    sleep(13);
    neshandadan_notify(azanName.c_str(),"لا اله الا الله",LOCAL_RESOURCES"/minber.png");
    sleep(15);
}

void azanCore::sayVozoo(string azanName)
{
    string matn = "تا ";
    azanName += " به افق تهران";
    matn += azanName;
    matn += " پنج دقیقه باقی است";
    sleep(1);
    neshandadan_notify("پاشو برو وضو بگیر! تا اذان چیزی باقی نمونده",matn.c_str(),LOCAL_RESOURCES"/kubbetussahra.png");
}

void azanCore::sayGhoroob(string azanName , int azanID)
{
    string Patch;
    if (azanID == 2)
        Patch = LOCAL_RESOURCES"/sunrise.png";
    else
        Patch = LOCAL_RESOURCES"/sunset.png";

    neshandadan_notify(azanName.c_str(),"",Patch.c_str());
}

void azanCore::showOwghat()
{
    string matn;

    matn += "\n";
    matn += "اذان صبح : ";
    matn += Slocal[0];
    matn += "\n";
    matn += "طلوع آفتاب : " ;
    matn+= Slocal[1];
    matn+= "\n";
    matn += "اذان ظهر :";
    matn+= Slocal[2];
    matn+= "\n";
    matn += "غروب آفتاب : ";
    matn+= Slocal[3];
    matn+= "\n";
    matn += "اذان مغرب : ";
    matn+= Slocal[4];

    neshandadan_notify("اوقات شرعی",matn.c_str(),LOCAL_RESOURCES"/Info.png");
}

string azanCore::getAzanName(int a)
{
    string azanName;
    switch (a)
    {
    case 1:
        azanName = "اذان صبح";
        break;
    case 2:
        azanName = "طلوع آفتاب" ;
        break;
    case 3:
        azanName = "اذان ظهر";
        break;
    case 4:
        azanName = "غروب آفتاب";
        break;
    case 5:
        azanName = "اذان مغرب";
        break;
    }
    return azanName;
}






















