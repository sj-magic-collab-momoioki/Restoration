/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"

/************************************************************
************************************************************/
class MASK_INVERT{
private:
	/****************************************
	****************************************/
	enum STATE{
		STATE__SLEEP,
		STATE__INTERVAL,
		STATE__RUN,
	};
	
	/****************************************
	****************************************/
	STATE State;
	bool b_Sleep;
	
	float t_LastInt;
	
	float t_from;
	float d_interval;
	const float d_interval_min;
	const float d_interval_max;
	
	float y_ratio;
	const float ScanBarHeight_ratio;
	const float ScanSpeed;
	
	ofSoundPlayer sound;
	
	/****************************************
	****************************************/
	/********************
	singleton
	********************/
	MASK_INVERT();
	~MASK_INVERT();
	MASK_INVERT(const MASK_INVERT&); // Copy constructor. no define.
	MASK_INVERT& operator=(const MASK_INVERT&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void setup_sound(ofSoundPlayer& sound, string FileName);
	void drawToFbo_Black(ofFbo& fbo);
	void drawToFbo_White(ofFbo& fbo);
	void drawToFbo_Run(ofFbo& fbo);
	
public:
	/********************
	********************/
	static MASK_INVERT* getInstance(){
		static MASK_INVERT inst;
		return &inst;
	}
	
	void setup();
	void update(float now);
	void drawToFbo(ofFbo& fbo);
	void Sleep();
	void LateRun(bool b_immediate, float now);
	void Run(bool b_1time, float now);
	void exit();
	bool IsState_Interval();
	bool IsState_Sleep();
};

