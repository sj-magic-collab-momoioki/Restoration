/************************************************************
************************************************************/
#include "Mask_Invert.h"

/************************************************************
************************************************************/

/******************************
******************************/
MASK_INVERT::MASK_INVERT()
: State(STATE__SLEEP)
, b_Sleep(false)
, d_interval_min(10.0)
, d_interval_max(25.0)
, ScanBarHeight_ratio(0.2)
, ScanSpeed(2.5)
, t_LastInt(0)
{
}

/******************************
******************************/
MASK_INVERT::~MASK_INVERT()
{
}

/******************************
******************************/
void MASK_INVERT::exit()
{
}

/******************************
******************************/
void MASK_INVERT::setup()
{
	setup_sound(sound, "sound/Invert/test-tone-white-noise_f1gdOhVd.wav");
}

/******************************
******************************/
void MASK_INVERT::setup_sound(ofSoundPlayer& sound, string FileName)
{
	sound.load(FileName.c_str());
	if(sound.isLoaded()){
		sound.setLoop(true);
		sound.setMultiPlay(true);
		sound.setVolume(1.0);
	}else{
		printf("%s load Error\n", FileName.c_str());
		fflush(stdout);
	}
}

/******************************
******************************/
void MASK_INVERT::update(float now)
{
	/*
	printf("%d\r", State);
	fflush(stdout);
	*/
	
	switch(State){
		case STATE__SLEEP:
			break;
			
		case STATE__INTERVAL:
			if(b_Sleep){
				State = STATE__SLEEP;
				b_Sleep = false;
			}else if(d_interval < now - t_from){
				State = STATE__RUN;
				
				t_from = now;
				sound.play();
				y_ratio = 0;
			}
			break;
			
		case STATE__RUN:
			y_ratio += ScanSpeed * (now - t_LastInt);
			
			if(1.0 + ScanBarHeight_ratio < y_ratio){
				State = STATE__INTERVAL;
				t_from = now;
				d_interval = ofRandom(d_interval_min, d_interval_max);
				if(sound.isPlaying()) sound.stop();
			}
			break;
	}
	
	t_LastInt = now;
}

/******************************
******************************/
void MASK_INVERT::drawToFbo(ofFbo& fbo)
{
	switch(State){
		case STATE__SLEEP:
		case STATE__INTERVAL:
		
			drawToFbo_Black(fbo);
			
			break;
			
		case STATE__RUN:
			drawToFbo_Run(fbo);
			break;
	}
}

/******************************
******************************/
void MASK_INVERT::drawToFbo_Black(ofFbo& fbo)
{
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		/*
		ofSetColor(0, 0, 0, 255);
		
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		*/
		
	fbo.end();
}

/******************************
******************************/
void MASK_INVERT::drawToFbo_White(ofFbo& fbo)
{
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		ofSetColor(255, 255, 255, 255);
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		
	fbo.end();
}

/******************************
******************************/
void MASK_INVERT::drawToFbo_Run(ofFbo& fbo)
{
	float _y = fbo.getHeight() * y_ratio;
	float ScanHeight = fbo.getHeight() * ScanBarHeight_ratio;
	
	
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		// ofSetColor(0, 0, 0, 255);
		// ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		
		ofFill();
		ofSetColor(255, 255, 255, 255);
		ofDrawRectangle(0, _y, fbo.getWidth(), -ScanHeight);
	fbo.end();
}

/******************************
******************************/
void MASK_INVERT::Sleep()
{
	if(State != STATE__SLEEP)	b_Sleep = true;
}

/******************************
******************************/
void MASK_INVERT::LateRun(bool b_immediate, float now)
{
	if(State == STATE__SLEEP){
		State = STATE__INTERVAL;
		
		t_from = now;
		
		if(b_immediate)	d_interval = 0;
		else			d_interval = ofRandom(d_interval_min, d_interval_max);
	}
}

/******************************
******************************/
void MASK_INVERT::Run(bool b_1time, float now)
{
	if(State == STATE__SLEEP){
		State = STATE__RUN;
		
		t_from = now;
		sound.play();
		y_ratio = 0;
		
		if(b_1time)	b_Sleep = true;
		else		b_Sleep = false;
	}
}

/******************************
******************************/
bool MASK_INVERT::IsState_Interval()
{
	if(State == STATE__INTERVAL)	return true;
	else							return false;
}

/******************************
******************************/
bool MASK_INVERT::IsState_Sleep()
{
	if(State == STATE__SLEEP)	return true;
	else						return false;
}


