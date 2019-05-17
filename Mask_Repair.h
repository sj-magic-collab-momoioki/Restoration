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
class MASK_REPAIR{
private:
	/****************************************
	****************************************/
	enum STATE{
		STATE__REPAIRED,
		STATE__DELETING,
		STATE__DELETED,
		STATE__REPAIRING,
		STATE__PAUSE,
	};
	
	/****************************************
	****************************************/
	STATE State;
	
	float t_from;
	float t_LastInt;
	
	float y_ratio;
	const float DeleteSpeed;
	const float RepairSpeed;
	
	ofSoundPlayer sound_Pi;
	ofSoundPlayer sound_Del;
	
	int NumBlocks_w;
	int NumBlocks_h;
	int NumBlocks;
	vector<int> Piece;
	
	int id;
	ofVec2f Cross;
	ofVec2f Cross_from;
	ofVec2f dir;
	float SquareDistance;
	
	/****************************************
	****************************************/
	/********************
	singleton
	********************/
	MASK_REPAIR();
	~MASK_REPAIR();
	MASK_REPAIR(const MASK_REPAIR&); // Copy constructor. no define.
	MASK_REPAIR& operator=(const MASK_REPAIR&); // コピー代入演算子. no define.
	
	/********************
	********************/
	void setup_sound(ofSoundPlayer& sound, string FileName, bool b_Loop, float vol);
	void drawToFbo_Black(ofFbo& fbo);
	void drawToFbo_White(ofFbo& fbo);
	void drawToFbo_Deleting(ofFbo& fbo);
	void drawToFbo_Repairing(ofFbo& fbo);
	
public:
	/********************
	********************/
	static MASK_REPAIR* getInstance(){
		static MASK_REPAIR inst;
		return &inst;
	}
	
	void setup();
	void update(float now);
	void drawToFbo(ofFbo& fbo);
	
	void Delete(float now);
	void Rapair(float now);
	float get_y_ratio();
	ofVec2f get_Cross_pos();
	ofVec2f get_Target_pos();
	ofVec2f get_Cross_pos_LeftUp();
	ofVec2f get_Target_pos_LeftUp();
	void exit();
	bool IsState_Deleted();
	bool IsState_Repaired();
	int get_Progress();
	int get_NumPieces();
};

