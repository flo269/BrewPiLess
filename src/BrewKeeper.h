#ifndef BrewKeeper_H
#define BrewKeeper_H
#include <Arduino.h>
#include "espconfig.h"
#include "GravityTracker.h"

#if EnableGravitySchedule

typedef int16_t Gravity;

#define INVALID_GRAVITY -1
#define IsGravityValid(g) ((g)>0)
#define FloatToGravity(f) ((Gravity)((f) * 1000.0 +0.5))
#define GravityToFloat(g) (((float)(g) / 1000.0))

typedef struct _ProfileStep{
 float    temp;
 float    days;
 Gravity  sg;
 uint8_t  stableTime;
 uint8_t  stablePoint;
 char     condition;
} ProfileStep;

class BrewProfile
{
	time_t _startDay;
	float _OGPoints;

	int  _numberOfSteps;

	int  _currentStep;
	time_t _timeEnterCurrentStep;
	time_t _currentStepDuration;

	ProfileStep *_steps;

	bool _profileLoaded;
	bool _statusLoaded;
	char _unit;
	uint8_t _stableThreshold;

	void _tempConvert(void);
	time_t _loadBrewingStatus(void);
	void _saveBrewingStatus(void);

	void _estimateStep(time_t now,Gravity gravity);

	void _toNextStep(unsigned long time);
	bool checkCondition(unsigned long time,Gravity gravity);
	bool _loadProfile(String filename);
public:
	BrewProfile(void):_profileLoaded(false),_statusLoaded(false),_numberOfSteps(0),_unit('U'),_steps(NULL){
    	_currentStep =0;
    	_timeEnterCurrentStep = 0;
    	_stableThreshold = 1;
	}
	int numberOfSteps(void){ return _numberOfSteps;}
	bool loaded(void){return _profileLoaded;}

	void setUnit(char unit);
	bool load(String filename);
	void setOriginalGravity(float gravity);
	float tempByTimeGravity(unsigned long time,Gravity gravity);

	void reload(void){_profileLoaded=false;}
	void setStableThreshold(uint8_t threshold){ _stableThreshold=threshold; }
};

#else //#if EnableGravitySchedule

class BrewProfile
{
	time_t _startDay;
	int  _numberOfSteps;
	time_t *_times;
	float  *_setTemps;
	bool _profileLoaded;
	char _unit;

	void _tempConvert(void);
public:
	BrewProfile(void):_profileLoaded(false),_numberOfSteps(0),_unit('U'),_setTemps(NULL),_times(NULL),_startDay(0){}
	int numberOfSteps(void){ return _numberOfSteps;}
	bool loaded(void){return _profileLoaded;}

	void setUnit(char unit);
	bool load(String filename);
	float tempByTime(unsigned long time);
	void reload(void){_profileLoaded=false;}
};

#endif //#if EnableGravitySchedule

class BrewKeeper
{
protected:
	time_t _lastSetTemp;

	BrewProfile _profile;
	String _filename;

#if EnableGravitySchedule
	Gravity _lastGravity;
#endif

	void (*_write)(const char*);
	void _loadProfile(void);
public:
#if EnableGravitySchedule
	BrewKeeper(void(*puts)(const char*)):_filename(""),_write(puts),_lastGravity(INVALID_GRAVITY){}
	void updateGravity(float sg){ _lastGravity=FloatToGravity(sg);}
	void updateOriginalGravity(float sg){ _profile.setOriginalGravity(sg); }
#else
	BrewKeeper(void(*puts)(const char*)):_filename(""),_write(puts){}
#endif
	void setFile(String filename){_filename=filename;}
	void keep(time_t now);
	void reloadProfile(){ _profile.reload(); _lastSetTemp=0;}
	void setStableThreshold(uint8_t threshold){_profile.setStableThreshold(threshold);}
};

#endif
