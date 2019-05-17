/************************************************************
************************************************************/
#include "Mask_Scan.h"

/************************************************************
************************************************************/

/******************************
******************************/
MASK_SCAN::MASK_SCAN()
: State(STATE__SLEEP)
, b_Sleep(false)
, d_interval_min(28.0)
, d_interval_max(30.0)
, ScanBarWidth_ratio(0.5)
, ScanSpeed(0.3)
, t_LastInt(0)
{
}

/******************************
******************************/
MASK_SCAN::~MASK_SCAN()
{
}

/******************************
******************************/
void MASK_SCAN::exit()
{
}

/******************************
******************************/
void MASK_SCAN::setup()
{
	setup_sound(sound_Laputa_L, "sound/Scan/Laputa_L.mp3");
	setup_sound(sound_Laputa_H, "sound/Scan/Laputa_H.mp3");
	
	Vboset.setup(4); /* square */
	Vboset.set_Color(0, ofColor(255, 255, 255, 0));
	Vboset.set_Color(1, ofColor(255, 255, 255, 0));
	Vboset.set_Color(2, ofColor(255, 255, 255, 255));
	Vboset.set_Color(3, ofColor(255, 255, 255, 255));
}

/******************************
******************************/
void MASK_SCAN::setup_sound(ofSoundPlayer& sound, string FileName)
{
	sound.load(FileName.c_str());
	if(sound.isLoaded()){
		sound.setLoop(false);
		sound.setMultiPlay(true);
		sound.setVolume(1.0);
	}else{
		printf("%s load Error\n", FileName.c_str());
		fflush(stdout);
	}
}

/******************************
******************************/
void MASK_SCAN::update(float now)
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
				sound_Laputa_L.play();
				x_ratio = 0;
				b_SoundHalf = false;
			}
			break;
			
		case STATE__RUN:
			x_ratio += ScanSpeed * (now - t_LastInt);
			
			if( (0.5 < x_ratio) && !b_SoundHalf){
				b_SoundHalf = true;
				sound_Laputa_H.play();
			}
			
			if(1.0 + ScanBarWidth_ratio < x_ratio){
				State = STATE__INTERVAL;
				t_from = now;
				d_interval = ofRandom(d_interval_min, d_interval_max);
			}
			break;
	}
	
	t_LastInt = now;
}

/******************************
******************************/
void MASK_SCAN::RefreshVerts(float fbo_w, float fbo_h)
{
	float _x = fbo_w * x_ratio;
	float ScanWidth = fbo_w * ScanBarWidth_ratio;
	
	Vboset.VboVerts[0].set( _x -  ScanWidth + 1, fbo_h);
	Vboset.VboVerts[1].set( _x -  ScanWidth + 1, 0);
	Vboset.VboVerts[2].set( _x, 0);
	Vboset.VboVerts[3].set( _x, fbo_h);
}

/******************************
******************************/
void MASK_SCAN::drawToFbo(ofFbo& fbo)
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
void MASK_SCAN::drawToFbo_Black(ofFbo& fbo)
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
void MASK_SCAN::drawToFbo_Run(ofFbo& fbo)
{
	ofDisableAlphaBlending();
	
	RefreshVerts(fbo.getWidth(), fbo.getHeight());
	Vboset.update();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		/*
		ofSetColor(0, 0, 0, 255);
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		*/
		
		ofSetColor(255, 255, 255, 255);
		glPointSize(1.0);
		glLineWidth(1);
		ofFill();
		
		Vboset.draw(GL_QUADS);
	fbo.end();
}

/******************************
******************************/
void MASK_SCAN::Sleep()
{
	if(State != STATE__SLEEP)	b_Sleep = true;
}

/******************************
******************************/
void MASK_SCAN::LateRun(bool b_immediate, float now)
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
bool MASK_SCAN::IsState_Interval()
{
	if(State == STATE__INTERVAL)	return true;
	else							return false;
}

/******************************
******************************/
bool MASK_SCAN::IsState_Sleep()
{
	if(State == STATE__SLEEP)	return true;
	else						return false;
}

