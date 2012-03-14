//--------------------- Copyright Block ----------------------
/*

PrayTime.js: Prayer Times Calculator (ver 1.2.1)
Copyright (C) 2007-2010 PrayTimes.org

Developer: Bijan Binaee, Hamid Zarrabi-Zadeh
License: GNU General Public License, ver 3

TERMS OF USE:
	Permission is granted to use this code, with or
	without modification, in any website or application
	provided that credit is given to the original work
	with a link back to PrayTimes.org.

This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY.

PLEASE DO NOT REMOVE THIS COPYRIGHT BLOCK.

*/


//--------------------- Help and Manual ----------------------
/*

User's Manual:
http://praytimes.org/manual

Calculating Formulas:
http://praytimes.org/calculation

30 March 2011:
                                Java Code Convert to C++ and add it to notify in ubuntu 10.10
*/
#include "mohasebegar_owghat.h"
#define PI M_PI

mohasebegar_owghat::mohasebegar_owghat(int year, int month, int day,long double latitude,long double longitude,long double Atimezone)
{
    create( year,  month,  day, latitude, longitude, Atimezone);
}

void mohasebegar_owghat::create(int year, int month, int day,long double latitude,long double longitude,long double Atimezone)
{
        //------------------------ Constants --------------------------
    Jafari     = 0;    // Ithna Ashari
    Karachi    = 1;    // University of Islamic Sciences, Karachi
    ISNA       = 2;    // Islamic Society of North America (ISNA)
    MWL        = 3;    // Muslim World League (MWL)
    Makkah     = 4;    // Umm al-Qura, Makkah
    Egypt      = 5;    // Egyptian General Authority of Survey
    Custom     = 6;    // Custom Setting
    Tehran     = 7;    // Institute of Geophysics, University of Tehran

    // Juristic Methods
    Shafii     = 0;    // Shafii (standard)
    Hanafi     = 1;    // Hanafi

    // Adjusting Methods for Higher Latitudes
    None       = 0;    // No adjustment
    MidNight   = 1;    // middle of night
    OneSeventh = 2;    // 1/7th of night
    AngleBased = 3;    // angle/60th of night


    // Time Formats
    Time24     = 0;    // 24-hour format
    Time12     = 1;    // 12-hour format
    Time12NS   = 2;    // 12-hour format with no suffix
    Float      = 3;    // floating point number

    timeNames[0] = "Fajr";
    timeNames[1] = "Sunrise";
    timeNames[2] = "Dhuhr";
    timeNames[3] = "Asr";
    timeNames[4] = "Sunset";
    timeNames[5] = "Maghrib";
    timeNames[6] = "Isha";


    InvalidTime = "-----";

    //---------------------- Global Variables --------------------

	calcMethod   = 7;		// caculation method
	asrJuristic  = 0;		// Juristic method for Asr
	dhuhrMinutes = 0;		// minutes after mid-day for Dhuhr
	adjustHighLats = 1;	// adjusting method for higher latitudes

	timeFormat   = 0;		// time format
	//--------------------- Technical Settings --------------------

    numIterations = 1; 	// number of iterations needed to compute times

    //------------------- Calc Method Parameters --------------------
	/*  methodParams[methodNum] = {fa, ms, mv, is, iv};

			fa : fajr angle
			ms : maghrib selector (0 = angle; 1 = minutes after sunset)
			mv : maghrib parameter value (in angle or minutes)
			is : isha selector (0 = angle; 1 = minutes after maghrib)
			iv : isha parameter value (in angle or minutes)
	*/
	methodParams[Tehran][0]	= 17.7;
	methodParams[Tehran][1]	= 0;
	methodParams[Tehran][2]	= 4.5;
	methodParams[Tehran][3]	= 0;
	methodParams[Tehran][4]	= 18;

    //constructor:
    lat = latitude;
	lng = longitude;
	timeZone = effectiveTimeZone(year, month, day, Atimezone);
	JDate = julianDate(year, month, day) - longitude/ (15* 24);
}

mohasebegar_owghat::mohasebegar_owghat(tm *date,long double latitude,long double longitude)
{
    InputDay = *date;
    create(1900 + date->tm_year,date->tm_mon+1,date->tm_mday,latitude, longitude,date->tm_gmtoff/3600.0);
}

//! return prayer times for a given date
vector<string> mohasebegar_owghat::getPrayerTimes()
{
    return computeDayTimes();
}

owghat mohasebegar_owghat::getPrayer()
{
    owghat_sharee times;
    times.Fajr = 5;
    times.Sunrise = 6;
    times.Dhuhr = 12;
    times.Asr = 13;
    times.Sunset = 18;
    times.Maghrib = 18;
    times.Isha = 18;

	for (int i=1; i<=numIterations; i++)
		times = computeTimes(times);

	times = adjustTimes(times);


    return adjustTm(times);
}

//! set the calculation method
void mohasebegar_owghat::setCalcMethod (int methodID)
{
	calcMethod = methodID;
}

void mohasebegar_owghat::setAsrMethod (int methodID)
{
	if (methodID < 0 || methodID > 1)
		return;
	asrJuristic = methodID;
}

void mohasebegar_owghat::setFajrAngle (long double angle)
{
    long double buffer[5] = {angle, NULL, NULL, NULL, NULL};
	setCustomParams(buffer);
}

void mohasebegar_owghat::setMaghribAngle (long double angle)
{
    long double buffer[5] = {NULL, 0, angle, NULL, NULL};
	setCustomParams(buffer);
}

void mohasebegar_owghat::setIshaAngle (long double angle)
{
    long double buffer[5] = {NULL, NULL, NULL, 0, angle};
    setCustomParams(buffer);
}

void mohasebegar_owghat::setDhuhrMinutes (int minutes)
{
	dhuhrMinutes = minutes;
}

// set the minutes after Sunset for calculating Maghrib
void mohasebegar_owghat::setMaghribMinutes (long double minutes)
{
    long double buffer[5] = {NULL, 1, minutes, NULL, NULL};
	setCustomParams(buffer);
}

// set the minutes after Maghrib for calculating Isha
void mohasebegar_owghat::setIshaMinutes (long double minutes)
{
    long double buffer[5] = {NULL, NULL, NULL, 1, minutes};
	setCustomParams(buffer);
}

// set custom values for calculation parameters
void mohasebegar_owghat::setCustomParams (long double params[5])
{
	for (int i=0; i<5; i++)
	{
		if (params[i] == NULL)
			methodParams[Custom][i] = methodParams[calcMethod][i];
		else
			methodParams[Custom][i] = params[i];
	}
	calcMethod = Custom;
}

// set adjusting method for higher latitudes
void mohasebegar_owghat::setHighLatsMethod (int methodID)
{
	adjustHighLats = methodID;
}

// set the time format
void mohasebegar_owghat::setTimeFormat (int timeFormat)
{
	timeFormat = timeFormat;
}

// convert float hours to 24h format
string mohasebegar_owghat::floatToTime24 (long double time_n)
{
	time_n = fixhour(time_n+ 0.5/ 60);  // add 0.5 minutes to round
	int hours = (int)time_n;
	int minutes = (int)((time_n - hours)* 60);
	string buffer = twoDigitsFormat(hours);
    buffer += ":";
	buffer += twoDigitsFormat(minutes);
	return buffer;
}

tm mohasebegar_owghat::floatToTm (long double time_n)
{
    tm buffer;
    buffer = InputDay;
	time_n = fixhour(time_n+ 0.5/ 60);  // add 0.5 minutes to round
	buffer.tm_hour = (int)(time_n);
    buffer.tm_min = (int)((time_n - buffer.tm_hour)* 60);
	return buffer;
}

// convert float hours to 12h format
string mohasebegar_owghat::floatToTime12 (long double time_n, bool noSuffix)
{
	time_n = fixhour(time_n + 0.5/ 60);  // add 0.5 minutes to round
	int hours = floor(time_n);
	int minutes = floor((time_n - hours)* 60);
	string suffix = hours >= 12 ? " pm" : " am";
	hours = (hours+ 12- 1)% 12+ 1;
	string buffer = num2str(hours);
	buffer += ":";
	buffer += twoDigitsFormat(minutes);
	if (!noSuffix)
        buffer += suffix;
	return buffer;
}

string mohasebegar_owghat::floatToTime12NS (long double time_n)
{
	return floatToTime12(time_n, true);
}

//---------------------- Calculation Functions -----------------------

// References:
// http://www.ummah.net/astronomy/saltime
// http://aa.usno.navy.mil/faq/docs/SunApprox.html


// compute declination angle of sun and equation of time
vector<long double> mohasebegar_owghat::sunPosition(long double jd)
{
	long double D = jd - 2451545.0;
	long double g = fixangle(357.529 + 0.98560028* D);
	long double q = fixangle(280.459 + 0.98564736* D);
	long double L = fixangle(q + 1.915* dsin(g) + 0.020* dsin(2*g));

	long double R = 1.00014 - 0.01671* dcos(g) - 0.00014* dcos(2*g);
	long double e = 23.439 - 0.00000036* D;

	long double d = darcsin(dsin(e)* dsin(L));
	long double RA = darctan2(dcos(e)* dsin(L), dcos(L))/ 15;
	RA = fixhour(RA);
	long double EqT = q/15 - RA;

    vector<long double> buffer(2);
     buffer[0] = d;
     buffer[1] = EqT;
	return buffer;
}

// compute equation of time
long double mohasebegar_owghat::equationOfTime(long double jd)
{
	return sunPosition(jd)[1];
}

// compute declination angle of sun
long double mohasebegar_owghat::sunDeclination(long double jd)
{
	return sunPosition(jd)[0];
}

// compute mid-day (Dhuhr, Zawal) time
long double mohasebegar_owghat::computeMidDay(long double t)
{
	long double T = equationOfTime(JDate+ t);
	long double Z = fixhour(12- T);
	return Z;
}

// compute time for a given angle G
long double mohasebegar_owghat::computeTime(long double G, long double t)
{
	long double D = sunDeclination(JDate+ t);
	long double Z = computeMidDay(t);
	long double V = 1/15.0 * darccos((-dsin(G)- dsin(D)* dsin(lat))/ (dcos(D)* dcos(lat)));
	long double buffer = Z+ (G>90 ? -V : V);
	return buffer;
}

// compute the time of Asr
long double mohasebegar_owghat::computeAsr(int step,long double t)  // Shafii: step=1, Hanafi: step=2
{
	long double D = sunDeclination(JDate+ t);
	long double G = 0 - darccot(step+ dtan(abs(lat- D)));
	return computeTime(G, t);
}

//---------------------- Compute Prayer Times -----------------------


// compute prayer times at given julian date
owghat_sharee mohasebegar_owghat::computeTimes(owghat_sharee times)
{
	owghat_sharee t = dayPortion(times);
	long double Fajr    = computeTime(180 - methodParams[calcMethod][0], t.Fajr);
	long double Sunrise = computeTime(180- 0.833, t.Sunrise);
	long double Dhuhr   = computeMidDay(t.Dhuhr);
	long double Asr     = computeAsr(1+ asrJuristic, t.Asr);
	long double Sunset  = computeTime(0.833, t.Sunset);;
	long double Maghrib = computeTime(methodParams[calcMethod][2], t.Maghrib);
	long double Isha    = computeTime(methodParams[calcMethod][4], t.Isha);

    owghat_sharee buffer;
    buffer.Fajr= Fajr;
    buffer.Sunrise = Sunrise;
    buffer.Dhuhr = Dhuhr;
    buffer.Asr = Asr;
    buffer.Sunset = Sunset;
    buffer.Maghrib = Maghrib;
    buffer.Isha = Isha;
	return buffer;
}

// compute prayer times at given julian date
vector<string> mohasebegar_owghat::computeDayTimes()
{
    owghat_sharee times;
    times.Fajr = 5;
    times.Sunrise = 6;
    times.Dhuhr = 12;
    times.Asr = 13;
    times.Sunset = 18;
    times.Maghrib = 18;
    times.Isha = 18;

	for (int i=1; i<=numIterations; i++)
		times = computeTimes(times);

	times = adjustTimes(times);
	return adjustTimesFormat(times);
}

// adjust times in a prayer time array
owghat_sharee mohasebegar_owghat::adjustTimes(owghat_sharee times)
{
    times.Fajr += timeZone- lng/ 15;
    times.Sunrise += timeZone- lng/ 15;
    times.Dhuhr += timeZone- lng/ 15;
    times.Asr += timeZone- lng/ 15;
    times.Sunset += timeZone- lng/ 15;
    times.Maghrib += timeZone- lng/ 15;
    times.Isha += timeZone- lng/ 15;

	times.Dhuhr += dhuhrMinutes/ 60.0; //Dhuhr
	if (methodParams[calcMethod][1] == 1) // Maghrib
		times.Maghrib = times.Sunset + methodParams[calcMethod][2]/ 60;
	if (methodParams[calcMethod][3] == 1) // Isha
		times.Isha = times.Maghrib + methodParams[calcMethod][4]/ 60;
///HERE CAN HAVE BUG!! if (adjustHighLats != None)
	if (adjustHighLats != NULL)
		times = adjustHighLatTimes(times);
	return times;
}


// convert times array to given time format
vector<string> mohasebegar_owghat::adjustTimesFormat(owghat_sharee times)
{
    //Copy owghat_sharee to long double array
    long double temp[7];
    temp[0] = times.Fajr;
    temp[1] = times.Sunrise;
    temp[2] = times.Dhuhr;
    temp[3] = times.Asr;
    temp[4] = times.Sunset;
    temp[5] = times.Maghrib;
    temp[6] = times.Isha;
    //Finsish Copy!

    vector<string> buffer(7);
	if (timeFormat == Float)
    {
        for (int i=0; i<7; i++)
        {
            buffer[i] = num2str((int)temp[i]);
        }
        return buffer;
    }
	for (int i=0; i<7; i++)
		if (timeFormat == Time12)
			buffer[i] = floatToTime12(temp[i],false);
		else if (timeFormat == Time12NS)
			buffer[i] = floatToTime12(temp[i], true);
		else
			buffer[i] = floatToTime24(temp[i]);
	return buffer;
}
// ADD : get time in tm format
owghat mohasebegar_owghat::adjustTm(owghat_sharee times)
{
    //Copy owghat_sharee to long double array
    owghat buffer;

    buffer.Fajr = floatToTm(times.Fajr);
    buffer.Sunrise = floatToTm(times.Sunrise);
    buffer.Dhuhr = floatToTm(times.Dhuhr);
    buffer.Sunset = floatToTm(times.Sunset);
    buffer.Maghrib = floatToTm(times.Maghrib);

	return buffer;
}

// adjust Fajr, Isha and Maghrib for locations in higher latitudes
owghat_sharee mohasebegar_owghat::adjustHighLatTimes (owghat_sharee times)
{
	long double nightTime = timeDiff(times.Sunset, times.Sunrise); // sunset to sunrise

	// Adjust Fajr
	//! May have bug!
	long double FajrDiff = nightPortion(methodParams[calcMethod][0]) * nightTime;
//if (isnan(times.Fajr) || timeDiff(times.Fajr, times.Sunrise) > FajrDiff)
//	if (timeDiff(times.Fajr, times.Sunrise) > FajrDiff)
//		times.Fajr = times.Sunrise - FajrDiff;

	// Adjust Isha
	long double IshaAngle = (methodParams[calcMethod][3] == 0) ? methodParams[calcMethod][4] : 18;
	long double IshaDiff = nightPortion(IshaAngle)* nightTime;
//	if (isnan(times.Isha) || timeDiff(times.Sunset, times.Isha) > IshaDiff)
//	if (timeDiff(times.Sunset, times.Isha) > IshaDiff)
//		times.Isha = times.Sunset + IshaDiff;

	// Adjust Maghrib
	long double MaghribAngle = (methodParams[calcMethod][1] == 0) ? methodParams[calcMethod][2] : 4;
	long double MaghribDiff = nightPortion(MaghribAngle)* nightTime;
//	if (isnan(times.Maghrib) || timeDiff(times.Sunset, times.Maghrib) > MaghribDiff)
//	if (timeDiff(times.Sunset, times.Maghrib) > MaghribDiff)
//		times.Maghrib = times.Sunset + MaghribDiff;

	return times;
}

// the night portion used for adjusting times in higher latitudes
long double mohasebegar_owghat::nightPortion(long double angle)
{
	if (adjustHighLats == AngleBased)
		return 1/60* angle;
	if (adjustHighLats == MidNight)
		return 1/2;
	if (adjustHighLats == OneSeventh)
		return 1/7;
}

// convert hours to day portions
owghat_sharee mohasebegar_owghat::dayPortion(owghat_sharee times)
{
    times.Fajr /= 24;
    times.Sunrise /= 24;
    times.Dhuhr /= 24;
    times.Asr /= 24;
    times.Sunset /= 24;
    times.Maghrib /= 24;
    times.Isha /= 24;
	return times;
}

//---------------------- Misc Functions -----------------------


// compute the difference between two times
long double mohasebegar_owghat::timeDiff(long double time1,long double time2)
{
	return fixhour(time2- time1);
}


// add a leading 0 if necessary
string mohasebegar_owghat::twoDigitsFormat (int num)
{
    int num2 ;
    string t ,temp;
    num2= num;
    while(num2 > 0)
    {
        temp +=(char)(48 + num2%10);
        num2 = num2 / 10;
    }
    if (num < 10)
        temp += "0";

    for (int i = temp.size() - 1; i >= 0 ; i--)
        t += temp[i];
	return t;
}



//---------------------- Julian Date Functions -----------------------


// calculate julian date from a calendar date
long double mohasebegar_owghat::julianDate(int year, int month,int day)
{
	if (month <= 2)
	{
		year -= 1;
		month += 12;
	}
	long double A = (int)(year/ 100);
	long double B = 2- A+ (int)(A/ 4);

	long double JD = (int)(365.25* (year+ 4716))+ (int)(30.6001* (month+ 1))+ day+ B- 1524.5;
	return JD;
}

//---------------------- Time-Zone Functions -----------------------


// compute local time-zone for a specific date
long double mohasebegar_owghat::getTimeZone ( tm *date)
{
	//var localDate = new Date(date.getFullYear(), date.getMonth(), date.getDate(), 0, 0, 0, 0);
	//string GMTString = localDate.toGMTString();
	//var GMTDate = new Date(GMTString.substring(0, GMTString.lastIndexOf(' ')- 1));
	//long double hoursDiff = (localDate- GMTDate)/ (1000* 60* 60);
	//return hoursDiff;
	return 0;
}


// return effective timezone for a given date
long double mohasebegar_owghat::effectiveTimeZone(int year, int month,int day,long double timeZone)
{
    tm*date = new tm;
	date->tm_year = year;
	date->tm_mon = month- 1;
	date->tm_mday = day;
	if (timeZone == NULL)
		timeZone = getTimeZone(date);
	return  timeZone;
}

//---------------------- Trigonometric Functions -----------------------

// degree sin
long double mohasebegar_owghat::dsin(long double d)
{
    return sin(dtr(d));
}

// degree cos
long double mohasebegar_owghat::dcos(long double d)
{
    return cos(dtr(d));
}

// degree tan
long double mohasebegar_owghat::dtan(long double d)
{
    return tan(dtr(d));
}

// degree arcsin
long double mohasebegar_owghat::darcsin(long double x)
{
    return rtd(asin(x));
}

// degree arccos
long double mohasebegar_owghat::darccos(long double x)
{
    return rtd(acos(x));
}

// degree arctan
long double mohasebegar_owghat::darctan(long double x)
{
    return rtd(atan(x));
}

// degree arctan2
long double mohasebegar_owghat::darctan2(long double y,long double x)
{
    return rtd(atan2(y, x));
}

// degree arccot
long double mohasebegar_owghat::darccot(long double x)
{
    return rtd(atan(1/x));
}

// degree to radian
long double mohasebegar_owghat::dtr (long double d)
{
    return (d * PI) / 180.0;
}

// radian to degree
long double mohasebegar_owghat::rtd (long double r)
{
    return (r * 180.0) / PI;
}

// range reduce angle in degrees.
long double mohasebegar_owghat::fixangle (long double a)
{
	a = a - 360.0 * ((int)(a / 360.0));
	a = a < 0 ? a + 360.0 : a;
	return a;
}

// range reduce hours to 0..23
long double mohasebegar_owghat::fixhour(long double a)
{
	a = a - 24.0 * ((int)(a / 24.0));
	a = a < 0 ? a + 24.0 : a;
	return a;
}

string mohasebegar_owghat::num2str(int num)
{
    string t ,temp;
    while(num > 0)
    {
        temp +=(48 + num%10);
        num = num / 10;
    }
    for (int i = temp.size(); i >= 0 ; i--)
        t+=temp[i];
	return t;
}
















