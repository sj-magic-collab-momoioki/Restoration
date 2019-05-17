/************************************************************
************************************************************/
#include "Mask_Random.h"

/************************************************************
************************************************************/

/******************************
******************************/
MASK_RANDOM::MASK_RANDOM()
: State(STATE__SLEEP)
, b_Sleep(false)
, d_interval_min(15.0)
, d_interval_max(17.0)
, t_LastInt(0)
, d_LightLength(2.0)
, d_LightInterval(0.1)
{
	/********************
	********************/
	NumBlocks_w = FBO_L_WIDTH / BLOCK_SIZE;
	if(FBO_L_WIDTH % BLOCK_SIZE) NumBlocks_w++;
	
	NumBlocks_h = FBO_L_HEIGHT / BLOCK_SIZE;
	if(FBO_L_HEIGHT % BLOCK_SIZE) NumBlocks_h++;
	
	NumBlocks = NumBlocks_w * NumBlocks_h;
	
	Piece.resize(NumBlocks);
}

/******************************
******************************/
MASK_RANDOM::~MASK_RANDOM()
{
}

/******************************
******************************/
void MASK_RANDOM::exit()
{
}

/******************************
******************************/
void MASK_RANDOM::setup()
{
	for(int i = 0; i < NUM_STROBES; i++){
		setup_sound(sound[i], "sound/Random/Cash_Register-Beep01-1.mp3");
	}
}

/******************************
******************************/
void MASK_RANDOM::setup_sound(ofSoundPlayer& sound, string FileName)
{
	sound.load(FileName.c_str());
	if(sound.isLoaded()){
		sound.setLoop(false);
		sound.setMultiPlay(true);
		sound.setVolume(0.3);
	}else{
		printf("%s load Error\n", FileName.c_str());
		fflush(stdout);
	}
}

/******************************
******************************/
void MASK_RANDOM::update(float now)
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
				L_UTIL::FisherYates(Piece);
				sound[0].play();
				c_gate = 0;
			}
			break;
			
		case STATE__RUN:
			break;
	}
	
	t_LastInt = now;
}

/******************************
******************************/
void MASK_RANDOM::drawToFbo(ofFbo& fbo)
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
void MASK_RANDOM::drawToFbo_Black(ofFbo& fbo)
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
void MASK_RANDOM::drawToFbo_Run(ofFbo& fbo)
{
	/********************
	********************/
	float now = ofGetElapsedTimef();
	float dt = now - t_from;
	
	int Last_c_gate = c_gate;
	c_gate = int(dt / d_LightInterval);
	if(Last_c_gate < c_gate){
		if(c_gate < NUM_STROBES) sound[c_gate].play();
	}
	
	/********************
	********************/
	float val_total = 0;
	
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		for(int i = 0; i < NUM_STROBES; i++){
			/* */
			float val;
			float ofs = d_LightInterval * i;
			
			if((dt < ofs) || (ofs + d_LightLength < dt)){
				val = 0;
			}else{
				float tan = -255 / d_LightLength;
				val = tan * (dt - ofs) + 255;
			}
			val_total += val;
			
			/* */
			int id = Piece[i];
			ofVec2f DispPos = L_UTIL::CalBlockPos_LeftUp(id, ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
			
			ofSetColor(255, 255, 255, val);
			ofFill();
			ofDrawRectangle(DispPos.x, DispPos.y, BLOCK_SIZE, BLOCK_SIZE);
		}
	fbo.end();
	
	/********************
	********************/
	if(val_total <= 0){
		State = STATE__INTERVAL;
		
		t_from = now;
		
		d_interval = ofRandom(d_interval_min, d_interval_max);
	}
}

/******************************
******************************/
void MASK_RANDOM::Sleep()
{
	if(State != STATE__SLEEP)	b_Sleep = true;
}

/******************************
******************************/
void MASK_RANDOM::LateRun(bool b_immediate, float now)
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
bool MASK_RANDOM::IsState_Sleep()
{
	if(State == STATE__SLEEP)	return true;
	else						return false;
}


