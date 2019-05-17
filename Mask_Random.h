/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"
#include "util.h"

/************************************************************
************************************************************/
class MASK_RANDOM{
private:
	/****************************************
	****************************************/
	enum STATE{
		STATE__SLEEP,
		STATE__INTERVAL,
		STATE__RUN,
	};
	
	enum{
		NUM_STROBES = 6,
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
	
	int NumBlocks_w;
	int NumBlocks_h;
	int NumBlocks;
	vector<int> Piece;
	
	const float d_LightLength;
	const float d_LightInterval;
	int c_gate;
	
	ofSoundPlayer sound[NUM_STROBES];
	
	/****************************************
	****************************************/
	/********************
	singleton
	********************/
	MASK_RANDOM();
	~MASK_RANDOM();
	MASK_RANDOM(const MASK_RANDOM&); // Copy constructor. no define.
	MASK_RANDOM& operator=(const MASK_RANDOM&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void setup_sound(ofSoundPlayer& sound, string FileName);
	void drawToFbo_Black(ofFbo& fbo);
	void drawToFbo_Run(ofFbo& fbo);
	
public:
	/********************
	********************/
	static MASK_RANDOM* getInstance(){
		static MASK_RANDOM inst;
		return &inst;
	}
	
	void setup();
	void update(float now);
	void drawToFbo(ofFbo& fbo);
	void Sleep();
	void LateRun(bool b_immediate, float now);
	void exit();
	bool IsState_Sleep();
};

