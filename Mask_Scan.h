/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"

/************************************************************
************************************************************/
class MASK_SCAN{
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
	
	float x_ratio;
	const float ScanBarWidth_ratio;
	const float ScanSpeed;
	
	ofSoundPlayer sound_Laputa_L;
	ofSoundPlayer sound_Laputa_H;
	bool b_SoundHalf;
	
	VBO_SET Vboset;
	
	/****************************************
	****************************************/
	/********************
	singleton
	********************/
	MASK_SCAN();
	~MASK_SCAN();
	MASK_SCAN(const MASK_SCAN&); // Copy constructor. no define.
	MASK_SCAN& operator=(const MASK_SCAN&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void setup_sound(ofSoundPlayer& sound, string FileName);
	void drawToFbo_Black(ofFbo& fbo);
	void drawToFbo_Run(ofFbo& fbo);
	void RefreshVerts(float fbo_w, float fbo_h);
	
public:
	/********************
	********************/
	static MASK_SCAN* getInstance(){
		static MASK_SCAN inst;
		return &inst;
	}
	
	void setup();
	void update(float now);
	void drawToFbo(ofFbo& fbo);
	void Sleep();
	void LateRun(bool b_immediate, float now);
	void exit();
	bool IsState_Interval();
	bool IsState_Sleep();
};

