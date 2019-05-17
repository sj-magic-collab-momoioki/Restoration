/************************************************************
参考URLs
	ピクセルシェーダ on ofxPostGlitch
		https://note.mu/loveandsheep/n/n0d7cfafa9794
		
	ffmpeg : mp3 切り出し
		https://qiita.com/yang_orz/items/4f19e88a456e56aadc55
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxPostGlitch.h"
#include "ofxFastFboReader.h"

#include "sj_common.h"
#include "util.h"

#include "Mask_Scan.h"
#include "Mask_Invert.h"
#include "Mask_Random.h"
#include "Mask_Repair.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		FBO__ORG,
		FBO__MASKED,
		FBO__MASK_ALL,
		FBO__REPAIRED,
		FBO__SCAN,
		FBO__GLITCH_RANDOM,
		FBO__GLITCH_INVERT,
		
		FBO__OUTPUT,
		
		NUM_FBOS_L,
	};
	enum{
		FBO__BLOCK_MOVE,
		FBO__BLOCK_TARGET,
		
		NUM_FBOS_S,
	};
	
	enum STATE{
		STATE__REPAIRED,
		STATE__REPAIRED_GLITCH,
		
		STATE__GLITCHING,
		STATE__DELETING,
		STATE__WAIT_AFTER_DELETE,
		STATE__PREP_FOR_SCAN,
		STATE__SCANNING,
		
		STATE__REPAIRING,
		STATE__REPAIRING_GLITCH,
		
		STATE__FIN,
		STATE__TO_CENTER,
	};
	
	enum{
		FONT_S,
		FONT_M,
		FONT_L,
		
		NUM_FONTSIZE,
	};
	
	enum{
		NUM_AMBIENT_SOUND = 2,
	};
	
	enum{
		NUM_GLITCH_TYPES = 17,
		MAX_NUM_GLITCHS_ONE_TIME = 5,
	};
	
	/****************************************
	****************************************/
	
	/********************
	********************/
	int png_id;
	ofTrueTypeFont font[NUM_FONTSIZE];
	
	/********************
	********************/
	STATE State;
	bool b_Start;
	float t_from;
	
	float d_Enjoy;
	
	float t_LastINT;
	
	
	
	/********************
	********************/
	bool b_Play_EnjoyingMusic;
	
	ofSoundPlayer sound_Ambient[NUM_AMBIENT_SOUND];
	ofSoundPlayer sound_Pi;
	ofSoundPlayer sound_MoveToCenter;
	
	ofSoundPlayer sound_Enjoying;
	ofSoundPlayer sound_Noise;
	
	ofImage image;
	ofFbo fbo_L[NUM_FBOS_L];
	ofFbo fbo_S[NUM_FBOS_S];
	ofPixels pix_image;
	ofxFastFboReader FboReader;
	
	MASK_SCAN* Mask_Scan;
	MASK_INVERT* Mask_Invert;
	MASK_RANDOM* Mask_Random;
	MASK_REPAIR* Mask_Rapair;
	
	/********************
	********************/
	ofShader shader_Mask;
	ofShader shader_Invert;
	
	/********************
	********************/
	const ofPoint DispPos__Left			= ofPoint(90, 40);
	const ofPoint DispPos__Right_Left	= ofPoint(980, 40);
	const ofPoint DispPos__Right_Right	= ofPoint(1360, 40);
	const ofPoint DispPos__Center		= ofPoint(500, 40);
	
	ofPoint DispPos_ToCenter;
	
	float d_DispPos_to_Center;
	
	/********************
	********************/
	ofxPostGlitch	myGlitch;
	int Glitch_Ids[NUM_GLITCH_TYPES];
	int NumGlitch_Enable;
	
	/****************************************
	****************************************/
	void set_dEnjoy();
	void setup_Gui();
	void setup_sound(ofSoundPlayer& sound, string FileName, bool b_Loop, float vol);
	void StateChart(float now);
	void drawFbo_org();
	void drawFbo_masked();
	void drawFbo_Inverted();
	void drawFbo_PartsOfOrg(ofFbo& fbo, ofVec2f _pos);
	
	void draw_FboToScreen(ofFbo& fbo, float width, float height, ofVec2f _pos);
	void draw_RepairingCursor(ofVec2f _pos);
	void draw_Fbo_Ovelay(ofFbo& fbo, ofVec2f _pos, float alpha);
	
	void draw_Scale();
	void draw_Scale_x(ofVec2f _pos, ofVec2f _scale);
	void draw_Scale_y(ofVec2f _pos, ofVec2f _scale);
	
	void drawText_CrossPos();
	void drawText_RGB(ofFbo& fbo, ofVec2f pos, bool b_DynamicColor);
	void drawText_Progress();
	
	void draw_NextRepairingTarget(ofVec2f _pos);
	
	void State_To_Glitching(float now);
	void State_To_Fin(float now);
	
	/********************
	********************/
	void Set_myGlitch(int key, bool b_switch);
	void Clear_AllGlitch();
	

	
public:
	/****************************************
	****************************************/
	ofApp(bool _b_Play_EnjoyingMusic);
	~ofApp();
	
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
