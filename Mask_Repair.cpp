/************************************************************
************************************************************/
#include "Mask_Repair.h"

/************************************************************
************************************************************/

/******************************
******************************/
MASK_REPAIR::MASK_REPAIR()
: State(STATE__REPAIRED)
, DeleteSpeed(1.5)
, RepairSpeed(500)
, t_LastInt(0)
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
MASK_REPAIR::~MASK_REPAIR()
{
}

/******************************
******************************/
void MASK_REPAIR::exit()
{
}

/******************************
******************************/
void MASK_REPAIR::setup()
{
	setup_sound(sound_Pi, "sound/Repair/pi_1.mp3", false, 0.2);
	setup_sound(sound_Del, "sound/Repair/decision1.mp3", false, 0.7);
}

/******************************
******************************/
void MASK_REPAIR::setup_sound(ofSoundPlayer& sound, string FileName, bool b_Loop, float vol)
{
	sound.load(FileName.c_str());
	if(sound.isLoaded()){
		sound.setLoop(b_Loop);
		sound.setMultiPlay(true);
		sound.setVolume(vol);
	}else{
		printf("%s load Error\n", FileName.c_str());
		fflush(stdout);
	}
}

/******************************
******************************/
void MASK_REPAIR::update(float now)
{
	/*
	printf("%d\r", State);
	fflush(stdout);
	*/
	
	switch(State){
		case STATE__REPAIRED:
			break;
			
		case STATE__DELETING:
			y_ratio -= DeleteSpeed * (now - t_LastInt);
			if(y_ratio < 0) y_ratio = 0;
			
			if(y_ratio <= 0){
				State = STATE__DELETED;
				if(sound_Del.isPlaying()) sound_Del.stop();
			}
			break;
			
		case STATE__DELETED:
			break;
			
		case STATE__REPAIRING:
			{
				ofVec2f NextPos = Cross + dir * (RepairSpeed * (now - t_LastInt));
				if(SquareDistance <= Cross_from.squareDistance(NextPos))	Cross = L_UTIL::CalBlockPos_Center(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
				else														Cross = NextPos;
			}
			break;
			
		case STATE__PAUSE:
			if(0.05 < now - t_from){
				if(id == NumBlocks - 1){
					State = STATE__REPAIRED;
				}else{
					State = STATE__REPAIRING;
					id++;
					
					/* */
					ofVec2f Target = L_UTIL::CalBlockPos_Center(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
					
					SquareDistance = Cross.squareDistance(Target);
					
					dir = Target - Cross;
					dir.normalize();
					
					Cross_from = Cross;
				}
			}
			break;
	}
	
	t_LastInt = now;
}

/******************************
******************************/
void MASK_REPAIR::drawToFbo(ofFbo& fbo)
{
	switch(State){
		case STATE__REPAIRED:
			drawToFbo_White(fbo);
			break;
			
		case STATE__DELETING:
			drawToFbo_Deleting(fbo);
			break;
			
		case STATE__DELETED:
			drawToFbo_Black(fbo);
			break;
			
		case STATE__REPAIRING:
			drawToFbo_Repairing(fbo);
			break;
			
		case STATE__PAUSE:
			// no over draw.
			break;
	}
}

/******************************
******************************/
void MASK_REPAIR::drawToFbo_Black(ofFbo& fbo)
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
void MASK_REPAIR::drawToFbo_White(ofFbo& fbo)
{
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		ofSetColor(255, 255, 255, 255);
		ofFill();
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		
	fbo.end();
}

/******************************
******************************/
void MASK_REPAIR::drawToFbo_Deleting(ofFbo& fbo)
{
	if(y_ratio < 0) y_ratio = 0;
	float _y = fbo.getHeight() * y_ratio;
	
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofClear(0, 0, 0, 0);
		
		ofSetColor(255, 255, 255, 255);
		ofFill();
		ofDrawRectangle(0, 0, fbo.getWidth(), _y);
	fbo.end();
}


/******************************
******************************/
void MASK_REPAIR::drawToFbo_Repairing(ofFbo& fbo)
{
	ofDisableAlphaBlending();
	
	fbo.begin();
		// ofClear(0, 0, 0, 0); // no clear.
		
		if(Cross == L_UTIL::CalBlockPos_Center(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w)){
			ofVec2f DispPos = L_UTIL::CalBlockPos_LeftUp(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
			
			ofSetColor(255, 255, 255, 255);
			ofFill();
			ofDrawRectangle(DispPos.x, DispPos.y, BLOCK_SIZE, BLOCK_SIZE);
			
			sound_Pi.play();
			t_from = ofGetElapsedTimef();
			
			State = STATE__PAUSE;
		}
	fbo.end();
}

/******************************
******************************/
void MASK_REPAIR::Delete(float now)
{
	if(State == STATE__REPAIRED){
		State = STATE__DELETING;
		
		y_ratio = 1.0;
		sound_Del.play();
	}
}

/******************************
******************************/
void MASK_REPAIR::Rapair(float now)
{
	if(State == STATE__DELETED){
		State = STATE__REPAIRING;
		
		L_UTIL::FisherYates(Piece);
		id = 0;
		
		Cross = L_UTIL::CalBlockPos_Center(Piece[NumBlocks - 1], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
		
		/* */
		ofVec2f Target = L_UTIL::CalBlockPos_Center(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
		
		SquareDistance = Cross.squareDistance(Target);
		
		dir = Target - Cross;
		dir.normalize();
		
		Cross_from = Cross;
		
		/* */
		sound_Pi.play();
	}
}

/******************************
******************************/
float MASK_REPAIR::get_y_ratio()
{
	return y_ratio;
}

/******************************
******************************/
ofVec2f MASK_REPAIR::get_Cross_pos()
{
	return Cross;
}

/******************************
******************************/
ofVec2f MASK_REPAIR::get_Target_pos()
{
	
	return L_UTIL::CalBlockPos_Center(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
}

/******************************
******************************/
ofVec2f MASK_REPAIR::get_Cross_pos_LeftUp()
{
	return L_UTIL::ExchangeBlockPos_Center_to_LeftUp(Cross, ofVec2f(BLOCK_SIZE, BLOCK_SIZE));
}

/******************************
******************************/
ofVec2f MASK_REPAIR::get_Target_pos_LeftUp()
{
	return L_UTIL::CalBlockPos_LeftUp(Piece[id], ofVec2f(BLOCK_SIZE, BLOCK_SIZE), NumBlocks_w);
}

/******************************
******************************/
bool MASK_REPAIR::IsState_Deleted()
{
	if(State == STATE__DELETED)	return true;
	else						return false;
}

/******************************
******************************/
bool MASK_REPAIR::IsState_Repaired()
{
	if(State == STATE__REPAIRED)	return true;
	else							return false;
}

/******************************
******************************/
int MASK_REPAIR::get_Progress()
{
	return id;
}

/******************************
******************************/
int MASK_REPAIR::get_NumPieces()
{
	return NumBlocks;
}


