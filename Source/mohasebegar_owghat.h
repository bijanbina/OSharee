#ifndef MOHASEBEGAR_OWGHAT_H
#define MOHASEBEGAR_OWGHAT_H
#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

struct owghat_sharee
{
    long double Fajr;
    long double Sunrise;
    long double Dhuhr;
    long double Asr;
    long double Sunset;
    long double Maghrib;
    long double Isha;
};

struct owghat
{
    tm Fajr;
    tm Sunrise;
    tm Dhuhr;
    tm Sunset;
    tm Maghrib;
};


class mohasebegar_owghat
{
public:
    mohasebegar_owghat(int year, int month, int day,long double latitude,long double longitude,long double timeZone);
    mohasebegar_owghat(tm *date,long double latitude,long double longitude);
    vector<string> getPrayerTimes();
    owghat getPrayer();
    void setCalcMethod (int methodID);
    void setAsrMethod (int methodID);
    void setFajrAngle (long double angle);
    void setMaghribAngle (long double angle);
    void setIshaAngle (long double angle);
    void setDhuhrMinutes (int minutes);
    void setMaghribMinutes (long double minutes);
    void setIshaMinutes (long double minutes);
    void setCustomParams (long double params[5]);
    void setHighLatsMethod (int methodID);
    void setTimeFormat (int timeFormat);
protected:
private:
    void create(int year, int month, int day,long double latitude,long double longitude,long double Atimezone);
    string floatToTime24 (long double time_n);
    string floatToTime12 (long double time_n, bool noSuffix);
    string floatToTime12NS (long double time_n);
    tm floatToTm (long double time_n);
    //Compute
    vector<long double> sunPosition(long double jd);
    long double equationOfTime(long double jd);
    long double sunDeclination(long double jd);
    long double computeMidDay(long double t);
    long double computeTime(long double G, long double t);;
    long double computeAsr(int step,long double t);
    //prayer time
    owghat_sharee computeTimes (owghat_sharee times);
    vector<string> computeDayTimes();
    owghat_sharee adjustTimes(owghat_sharee times);
    vector<string> adjustTimesFormat(owghat_sharee times);
    owghat adjustTm(owghat_sharee times);
    owghat_sharee adjustHighLatTimes (owghat_sharee times);
    owghat_sharee dayPortion(owghat_sharee times);
    long double nightPortion(long double angle);
    //Misc Functions
    long double timeDiff(long double time1,long double time2);
    string twoDigitsFormat (int num);
    //Julian Date Functions
    long double julianDate(int year, int month,int day);
    //Time-Zone Functions
    long double getTimeZone (tm *date);
    long double effectiveTimeZone(int year, int month,int day,long double timeZone);
    //Trigonometric Functions
    long double dsin(long double d);
    long double dcos(long double d);
    long double dtan(long double d);
    long double darcsin(long double x);
    long double darccos(long double x);
    long double darctan(long double x);
    long double darctan2(long double y,long double x);
    long double darccot(long double x);
    long double dtr (long double d);
    long double rtd (long double r);
    long double fixangle (long double a);
    long double fixhour(long double a);
    //MY Function
    string num2str(int num);
    //------------------------ Constants --------------------------
    int Jafari;
    int Karachi;
    int ISNA;
    int MWL;
    int Makkah;
    int Egypt;
    int Custom;
    int Tehran;

    // Juristic Methods
    int Shafii;
    int Hanafi;

    // Adjusting Methods for Higher Latitudes
    int None;
    int MidNight;
    int OneSeventh;
    int AngleBased;


    // Time Formats
    int Time24;
    int Time12;
    int Time12NS;
    int Float;

    string timeNames[7];
    string InvalidTime;
    // Variable
    int calcMethod;
	int asrJuristic;
	int dhuhrMinutes;
	int adjustHighLats;

	int timeFormat;

	int numIterations;

	long double methodParams[8][5];

	//Import Data
	long double lat;        // latitude
	long double lng;        // longitude
	long double timeZone;   // time-zone
	//Unknow type
	long double JDate;      // Julian date

	tm InputDay;
};

#endif // MOHASEBEGAR_OWGHAT_H
