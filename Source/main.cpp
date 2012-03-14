#include "Azan.h"
#include <string>
#include <cstring>

using namespace std;
#define START_Sleep 25
#define Reset_sleep 1000

int main(int argc , char* argv[])
{
    while(true)
    {
        sleep(START_Sleep);
        time_t buffer;
        tm *current_Time;
        vector<string> Slocal_owghat;


        time ( &buffer );
        current_Time = localtime ( &buffer );


        mohasebegar_owghat *local_mohasebegar = new mohasebegar_owghat(current_Time ,35.683334, 51.349998) ;
        owghat local_owghat = local_mohasebegar->getPrayer();
        Slocal_owghat = local_mohasebegar->getPrayerTimes();
        cout << local_owghat.Maghrib.tm_min << endl;
        delete local_mohasebegar;

        azanCore localCore(local_owghat ,Slocal_owghat ,argv[0] );
        localCore.startControl();
		//Program END new for new day start again
		sleep(Reset_sleep);

		neshandadan_notify("شب خوش","یک شب دیگر هم از عمرمون گذشت امیدوارم پیشرفت کرده باشیم!","/resource/sunset.png");


		//Calculate End of the day
		tm endDay = *current_Time;
		endDay.tm_hour = 23;
		endDay.tm_min   = 59;
		endDay.tm_sec   = 59;
		time_t WaitForTomorrow = mktime(&endDay) - mktime(current_Time);
		sleep(WaitForTomorrow);
    }
    return 0;
}



