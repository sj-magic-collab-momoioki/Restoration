/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(bool _b_Play_EnjoyingMusic)
: Mask_Scan(MASK_SCAN::getInstance())
, Mask_Invert(MASK_INVERT::getInstance())
, Mask_Random(MASK_RANDOM::getInstance())
, Mask_Rapair(MASK_REPAIR::getInstance())
, State(STATE__REPAIRED)
, b_Start(true)
, t_from(0) // Boot直後は、key inputでstart.
, d_DispPos_to_Center(1.0)
, png_id(0)
, t_LastINT(0)
, b_Play_EnjoyingMusic(_b_Play_EnjoyingMusic)
{
	fp_Log = fopen("../../../data/Log.csv", "w");
	
	font[FONT_S].load("font/RictyDiminished-Regular.ttf", 13, true, true, true);
	font[FONT_M].load("font/RictyDiminished-Regular.ttf", 14, true, true, true);
	font[FONT_L].load("font/RictyDiminished-Regular.ttf", 18, true, true, true);
}

/******************************
******************************/
ofApp::~ofApp()
{
	if(fp_Log) fclose(fp_Log);
}

/******************************
******************************/
void ofApp::exit(){
	Mask_Scan->exit();
	Mask_Invert->exit();
	Mask_Random->exit();
	Mask_Rapair->exit();
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("Restoration");
	
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	setup_Gui();
	set_dEnjoy();
	
	/********************
	********************/
	for(int i = 0; i < NUM_FBOS_L; i++)	{ fbo_L[i].allocate(FBO_L_WIDTH, FBO_L_HEIGHT, GL_RGBA); }
	for(int i = 0; i < NUM_FBOS_S; i++)	{
		fbo_S[i].allocate(BLOCK_SIZE, BLOCK_SIZE, GL_RGBA);
	}
	
	pix_image.allocate(BLOCK_SIZE, BLOCK_SIZE, GL_RGBA);
	
	myGlitch.setup(&fbo_L[FBO__OUTPUT]);
	
	/********************
	********************/
	image.load("image/momoi.png");
	drawFbo_org();
	
	/********************
	********************/
	Mask_Scan->setup();
	Mask_Invert->setup();
	Mask_Random->setup();
	Mask_Rapair->setup();
	
	/********************
	********************/
	/* */
	setup_sound(sound_Ambient[0], "sound/main/airplane.wav", true, 0.6);
	sound_Ambient[0].play();
	
	setup_sound(sound_Ambient[1], "sound/main/electrocardiogram_Loop.mp3", true, 1.0);
	sound_Ambient[1].play();
	
	/* */
	setup_sound(sound_Pi, "sound/main/Cash_Register-Beep01-1.mp3", false, 0.8);
	setup_sound(sound_MoveToCenter, "sound/main/AutoDoor.mp3", false, 0.7);
	
	/* */
	setup_sound(sound_Enjoying, "sound/main/Enjoy/bbc-051117-Smoky-Jazz-Music-Full-Mix.mp3", true, 0.0);
	if(b_Play_EnjoyingMusic)	sound_Enjoying.play();
	
	setup_sound(sound_Noise, "sound/main/radio-tuning-noise-short-waves_zydE-HEd.mp3", true, 0.0);
	sound_Noise.play();
	
	/********************
	********************/
	shader_Mask.load( "sj_shader/mask.vert", "sj_shader/mask.frag");
	shader_Invert.load( "sj_shader/invert.vert", "sj_shader/invert.frag");
	
	/********************
	********************/
	FboReader.setAsync(false); // true/ false の違いはよくわからない.
	
	/********************
	********************/
	ofSleepMillis(100);
}

/******************************
******************************/
void ofApp::set_dEnjoy()
{
	d_Enjoy = ofRandom(30, 35);
}

/******************************
******************************/
void ofApp::setup_sound(ofSoundPlayer& sound, string FileName, bool b_Loop, float vol)
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
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("Void", "gui.xml", 1000, 10);
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	ofSoundUpdate();
	
	/********************
	********************/
	float now = ofGetElapsedTimef();
	
	/********************
	********************/
	Mask_Scan->update(now);
	Mask_Invert->update(now);
	Mask_Random->update(now);
	Mask_Rapair->update(now);
	
	/********************
	********************/
	StateChart(now);
	
	/*
	printf("State = %d\r", State);
	fflush(stdout);
	*/
	printf("%5.2f\r", ofGetFrameRate());
	fflush(stdout);
	
	/********************
	********************/
	t_LastINT = now;
}

/******************************
******************************/
void ofApp::StateChart(float now)
{
	/********************
	********************/
	float Vol = sound_Enjoying.getVolume();
	const float VolFade_Speed = 0.2;
	if( (State == STATE__REPAIRED) || (State == STATE__REPAIRED_GLITCH) ){
		Vol += VolFade_Speed * (now - t_LastINT);
		if(1.0 < Vol) Vol = 1.0;
	}else{
		Vol -= VolFade_Speed * (now - t_LastINT);
		if(Vol < 0) Vol = 0.0;
	}
	
	sound_Enjoying.setVolume(Vol);
	
	/********************
	********************/
	float t = float(ofGetElapsedTimeMillis()) / 1.0e+3;
	float perlin = ofNoise(t);
	const float perlinThresh_Repaired	= 0.35;
	const float perlinThresh_Repairing	= 0.3;
	const float vol_Noise = 0.7;
	
	/********************
	********************/
	switch(State){
		case STATE__REPAIRED:
		{
			if(b_Start && (d_Enjoy < now - t_from)){
				State_To_Glitching(now);
				
			}else if(perlin < perlinThresh_Repaired){
				State = STATE__REPAIRED_GLITCH;
				sound_Noise.setVolume(vol_Noise);
				
				L_UTIL::FisherYates(Glitch_Ids, NUM_GLITCH_TYPES);
				NumGlitch_Enable = int(ofRandom(1, MAX_NUM_GLITCHS_ONE_TIME));
				for(int i = 0; i < NumGlitch_Enable; i++) { Set_myGlitch(Glitch_Ids[i], true); }
			}
		}			
			break;
			
		case STATE__REPAIRED_GLITCH:
		{
			if(b_Start && (d_Enjoy < now - t_from)){
				State_To_Glitching(now);
				
			}else if(perlinThresh_Repaired <= perlin){
				State = STATE__REPAIRED;
				sound_Noise.setVolume(0.0);
				
				// for(int i = 0; i < NumGlitch_Enable; i++) { Set_myGlitch(Glitch_Ids[i], false); }
				Clear_AllGlitch();
			}
		}			
			break;
			
		case STATE__GLITCHING:
			if(Mask_Invert->IsState_Interval()){
				State = STATE__DELETING;
				Mask_Rapair->Delete(now);
				// sound_Enjoying.setPaused(true);
			}
			break;
			
		case STATE__DELETING:
			if(Mask_Rapair->IsState_Deleted()){
				State = STATE__WAIT_AFTER_DELETE;
				t_from = now;
			}
			break;
			
		case STATE__WAIT_AFTER_DELETE:
			if(3.5 < now - t_from){
				State = STATE__PREP_FOR_SCAN;
				t_from = now;
				sound_Pi.play();
			}
			break;
			
		case STATE__PREP_FOR_SCAN:
			if(2.0 < now - t_from){
				State = STATE__SCANNING;
				Mask_Scan->LateRun(true, now);
			}
			break;
			
		case STATE__SCANNING:
			if(Mask_Scan->IsState_Interval()){
				State = STATE__REPAIRING;
				
				Mask_Rapair->Rapair(now);
				Mask_Invert->LateRun(false, now);
				Mask_Random->LateRun(false, now);
				
				t_from = now;
				
				sound_Noise.play();
				sound_Noise.setVolume(0.0);
			}
			break;
			
		case STATE__REPAIRING:
			if(Mask_Rapair->IsState_Repaired()){
				State_To_Fin(now);
				
			}else if(perlin < perlinThresh_Repairing){
				State = STATE__REPAIRING_GLITCH;
				sound_Noise.setVolume(vol_Noise);
				
				L_UTIL::FisherYates(Glitch_Ids, NUM_GLITCH_TYPES);
				NumGlitch_Enable = int(ofRandom(1, MAX_NUM_GLITCHS_ONE_TIME));
				for(int i = 0; i < NumGlitch_Enable; i++) { Set_myGlitch(Glitch_Ids[i], true); }
			}
			break;
			
		case STATE__REPAIRING_GLITCH:
			if(Mask_Rapair->IsState_Repaired()){
				State_To_Fin(now);
			}else if(perlinThresh_Repairing <= perlin){
				State = STATE__REPAIRING;
				sound_Noise.setVolume(0.0);
				
				Clear_AllGlitch();
			}
			
			break;
			
		case STATE__FIN:
			if( (Mask_Scan->IsState_Sleep()) && (Mask_Random->IsState_Sleep()) && (Mask_Invert->IsState_Sleep()) & (1.0 < now - t_from) ){
				State = STATE__TO_CENTER;
				t_from = now;
				
				DispPos_ToCenter = DispPos__Left;
				sound_MoveToCenter.play();
			}
			break;
			
		case STATE__TO_CENTER:
			{
				if(d_DispPos_to_Center <= now - t_from){
					State = STATE__REPAIRED;
					t_from = now;
					
					sound_Noise.play();
					sound_Noise.setVolume(0.0);
					
					// sound_Enjoying.setPaused(false);
					
					set_dEnjoy();
					
				}else{
					float ratio = (now - t_from) / d_DispPos_to_Center;
					DispPos_ToCenter = DispPos__Left + (DispPos__Center - DispPos__Left) * ratio;
				}
			}
			break;
	}
}

				

/******************************
******************************/
void ofApp::State_To_Glitching(float now){
	State = STATE__GLITCHING;
	Mask_Invert->Run(true, now);
	sound_Noise.stop();
	
	Clear_AllGlitch();
}

/******************************
******************************/
void ofApp::State_To_Fin(float now){
	State = STATE__FIN;
	
	Mask_Scan->Sleep();
	Mask_Random->Sleep();
	Mask_Invert->Sleep();
	
	t_from = now;
	
	sound_Noise.stop();
	Clear_AllGlitch();
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	Mask_Scan->drawToFbo(fbo_L[FBO__SCAN]);
	Mask_Invert->drawToFbo(fbo_L[FBO__GLITCH_INVERT]);
	Mask_Random->drawToFbo(fbo_L[FBO__GLITCH_RANDOM]);
	Mask_Rapair->drawToFbo(fbo_L[FBO__REPAIRED]);
	
	/********************
	********************/
	drawFbo_masked();
	drawFbo_Inverted();
	
	if(State == STATE__REPAIRING){
		drawFbo_PartsOfOrg(fbo_S[FBO__BLOCK_TARGET], Mask_Rapair->get_Target_pos_LeftUp());
		drawFbo_PartsOfOrg(fbo_S[FBO__BLOCK_MOVE], Mask_Rapair->get_Cross_pos_LeftUp());
	}
	
	/********************
	********************/
	myGlitch.generateFx(); // Apply effects
	
	/********************
	********************/
	// ofClear(0);
	ofBackground(0, 0, 0, 255);
	
	ofSetColor(255, 255, 255, 255);
	
	switch(State){
		case STATE__REPAIRED:
		case STATE__REPAIRED_GLITCH:
		case STATE__GLITCHING:
		case STATE__DELETING:
			if(State == STATE__REPAIRED_GLITCH){
				ofSetColor(50);
				ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
			}
			ofSetColor(255, 255, 255, 255);
			draw_FboToScreen(fbo_L[FBO__OUTPUT], fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight(), DispPos__Center);
			break;
			
		case STATE__WAIT_AFTER_DELETE:
			break;
			
		case STATE__PREP_FOR_SCAN:
			draw_Scale();
			break;
			
		case STATE__SCANNING:
			draw_Scale();
			draw_FboToScreen(fbo_L[FBO__OUTPUT], fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight(), DispPos__Left);
			break;
			
		case STATE__REPAIRING:
		case STATE__REPAIRING_GLITCH:
			draw_Scale();
			
			/* */
			draw_FboToScreen(fbo_L[FBO__OUTPUT], fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight(), DispPos__Left);
			
			// draw_Fbo_Ovelay(fbo_L[FBO__GLITCH_INVERT], DispPos__Left, 30);
			draw_Fbo_Ovelay(fbo_L[FBO__GLITCH_RANDOM], DispPos__Left, 80);
			draw_Fbo_Ovelay(fbo_L[FBO__SCAN], DispPos__Left, 40);
			
			draw_NextRepairingTarget(DispPos__Left);
			draw_RepairingCursor(DispPos__Left);
			
			draw_FboToScreen(fbo_S[FBO__BLOCK_MOVE], BLOCK_DRAW_SIZE, BLOCK_DRAW_SIZE, DispPos__Right_Left);
			draw_FboToScreen(fbo_S[FBO__BLOCK_TARGET], BLOCK_DRAW_SIZE, BLOCK_DRAW_SIZE, DispPos__Right_Right);
			
			drawText_CrossPos();
			drawText_RGB(fbo_S[FBO__BLOCK_MOVE], ofVec2f(DispPos__Right_Left.x, 500), false);
			drawText_RGB(fbo_S[FBO__BLOCK_TARGET], ofVec2f(DispPos__Right_Right.x, 500), false);
			drawText_Progress();
			break;
			
		case STATE__FIN:
			draw_Scale();
			draw_FboToScreen(fbo_L[FBO__OUTPUT], fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight(), DispPos__Left);
			break;
			
		case STATE__TO_CENTER:
			draw_FboToScreen(fbo_L[FBO__OUTPUT], fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight(), DispPos_ToCenter);
			break;
	}
}

/******************************
******************************/
void ofApp::drawText_Progress()
{
	Mask_Rapair->get_Progress();
	int get_NumPieces();

	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	/********************
	********************/
	char buf[BUF_SIZE_S];
	ofSetColor(255, 255, 255, 100);
	
	/* */
	sprintf(buf, "%5d/%5d  %5.0f[s]", Mask_Rapair->get_Progress(), Mask_Rapair->get_NumPieces(), ofGetElapsedTimef() - t_from);
	
	font[FONT_M].drawString(buf, DispPos__Right_Left.x + BLOCK_DRAW_SIZE - font[FONT_M].stringWidth("xxxxx/xxxxx  xxxxx[s]"), 450);
}

/******************************
******************************/
void ofApp::drawText_RGB(ofFbo& fbo, ofVec2f pos, bool b_DynamicColor)
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	/********************
	********************/
	char buf[BUF_SIZE_S];
	ofSetColor(255, 255, 255, 100);
	
	/********************
	********************/
	const int NUM_LINES = 30;
	const int NUM_PER_LINE = 4;
	const int STEP_Y = font[FONT_S].stringHeight("FF FF FF") * 2.0;
	const int STEP_X = font[FONT_S].stringWidth("FF FF FF") * 1.3;
	
	fbo.readToPixels(pix_image);
	// FboReader.readToPixels(fbo, pix_image); // 上手く読み取りできなかった(All zero)
	
	ofPushMatrix();
	ofTranslate(pos);
	for(int _y = 0; _y < NUM_LINES; _y++){
		for(int _x = 0; _x < NUM_PER_LINE; _x++){
			ofColor col = pix_image.getColor(_x, _y);
			sprintf(buf, "%02X %02X %02X", col.r, col.g, col.b);
			
			if(b_DynamicColor)	ofSetColor(col.r, col.g, col.b, 100);
			else				ofSetColor(255, 255, 255, 100);
			font[FONT_S].drawString(buf, STEP_X * _x, STEP_Y * _y);
		}
	}
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::drawText_CrossPos()
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	/********************
	********************/
	char buf[BUF_SIZE_S];
	ofSetColor(255, 255, 255, 100);
	
	/* */
	ofVec2f pos = Mask_Rapair->get_Cross_pos();
	sprintf(buf, "%4d, %4d", int(pos.x), int(pos.y));
	
	font[FONT_L].drawString(buf, DispPos__Right_Left.x, 450);
	
	/* */
	pos = Mask_Rapair->get_Target_pos();
	sprintf(buf, "%4d, %4d", int(pos.x), int(pos.y));
	
	font[FONT_L].drawString(buf, DispPos__Right_Right.x, 450);
}

/******************************
******************************/
void ofApp::draw_Scale()
{
	draw_Scale_x(ofVec2f(90, 0), ofVec2f(1, 1));
	draw_Scale_x(ofVec2f(90, 1080), ofVec2f(1, -1));
	
	draw_Scale_y(ofVec2f(45, 40), ofVec2f(1, 1));
	draw_Scale_y(ofVec2f(935, 40), ofVec2f(-1, 1));
}

/******************************
******************************/
void ofApp::draw_Scale_x(ofVec2f _pos, ofVec2f _scale)
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 255, 150);
	glPointSize(1.0);
	// glLineWidth(1);
	ofSetLineWidth(1);
	
	/********************
	********************/
	const float step = 5;
	
	ofPushMatrix();
		ofTranslate(_pos);
		ofScale(_scale.x, _scale.y);
		
		int id;
		float _x;
		for(id = 0, _x = 0; _x < FBO_L_WIDTH; _x += step, id++){
			if(id % 10 == 0)		ofDrawLine(_x, 0, _x, 20);
			else if(id % 5 == 0)	ofDrawLine(_x, 0, _x, 15);
			else					ofDrawLine(_x, 0, _x, 10);
		}
		
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::draw_Scale_y(ofVec2f _pos, ofVec2f _scale)
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 255, 150);
	glPointSize(1.0);
	// glLineWidth(1);
	ofSetLineWidth(1);
	
	/********************
	********************/
	const float step = 5;
	
	ofPushMatrix();
		ofTranslate(_pos);
		ofScale(_scale.x, _scale.y);
		
		int id;
		float _y;
		for(id = 0, _y = 0; _y < FBO_L_HEIGHT; _y += step, id++){
			if(id % 10 == 0)		ofDrawLine(0, _y, 20, _y);
			else if(id % 5 == 0)	ofDrawLine(0, _y, 15, _y);
			else					ofDrawLine(0, _y, 10, _y);
		}
		
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::draw_Fbo_Ovelay(ofFbo& fbo, ofVec2f _pos, float alpha)
{
	/********************
	oFでは、加算合成とスムージングは同時に効かない。その為、加算合成の前にofDisableSmoothing()を記述。
		https://qiita.com/y_UM4/items/b03a66d932536b25b51a
	********************/
	ofDisableSmoothing();
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 255, alpha);
	
	/********************
	********************/
	ofPushMatrix();
	ofPushStyle();
		
		ofTranslate(_pos);
		fbo.draw(0, 0, fbo.getWidth(), fbo.getHeight());
		
	ofPopStyle();
	ofPopMatrix();
	
}

/******************************
******************************/
void ofApp::draw_RepairingCursor(ofVec2f _pos)
{
	/********************
	oFでは、加算合成とスムージングは同時に効かない。その為、加算合成の前にofDisableSmoothing()を記述。
		https://qiita.com/y_UM4/items/b03a66d932536b25b51a
	********************/
	// ofEnableSmoothing();
	
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 0, 100);
	glPointSize(1.0);
	// glLineWidth(1);
	ofSetLineWidth(1);
	
	/********************
	********************/
	ofPushMatrix();
	ofPushStyle();
		
		ofTranslate(_pos);
		
		ofVec2f Cross = Mask_Rapair->get_Cross_pos();
		
		ofDrawLine(0, Cross.y, fbo_L[FBO__REPAIRED].getWidth(), Cross.y);
		ofDrawLine(Cross.x, 0, Cross.x, fbo_L[FBO__REPAIRED].getHeight());
		
	ofPopStyle();
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::draw_NextRepairingTarget(ofVec2f _pos)
{
	/********************
	oFでは、加算合成とスムージングは同時に効かない。その為、加算合成の前にofDisableSmoothing()を記述。
		https://qiita.com/y_UM4/items/b03a66d932536b25b51a
	********************/
	// ofEnableSmoothing();
	
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 255, 20);
	ofFill();
	
	glPointSize(1.0);
	// glLineWidth(1);
	ofSetLineWidth(1);
	
	/********************
	********************/
	ofPushMatrix();
	ofPushStyle();
		
		ofTranslate(_pos);
		
		ofVec2f LeftUp = Mask_Rapair->get_Target_pos_LeftUp();
		
		ofDrawRectangle(LeftUp.x, LeftUp.y, BLOCK_SIZE, BLOCK_SIZE);
		
	ofPopStyle();
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::draw_FboToScreen(ofFbo& fbo, float width, float height, ofVec2f _pos)
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	ofSetColor(255, 255, 255, 255);
	
	/********************
	********************/
	ofPushMatrix();
	ofPushStyle();
		
		ofTranslate(_pos);
		fbo.draw(0, 0, width, height);
		
	ofPopStyle();
	ofPopMatrix();
}

/******************************
******************************/
void ofApp::drawFbo_org()
{
	ofDisableAlphaBlending();
	
	fbo_L[FBO__ORG].begin();
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		
		image.draw(0, 0, fbo_L[FBO__ORG].getWidth(), fbo_L[FBO__ORG].getHeight());
		
	fbo_L[FBO__ORG].end();
}

/******************************
******************************/
void ofApp::drawFbo_masked()
{
	ofDisableAlphaBlending();
	
	fbo_L[FBO__MASKED].begin();
	shader_Mask.begin();
	
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
		
		shader_Mask.setUniformTexture( "mask_0", fbo_L[FBO__REPAIRED].getTexture(), 1 );
		shader_Mask.setUniformTexture( "mask_1", fbo_L[FBO__SCAN].getTexture(), 2 );
		shader_Mask.setUniformTexture( "mask_2", fbo_L[FBO__GLITCH_RANDOM].getTexture(), 3 );
		
		fbo_L[FBO__ORG].draw(0, 0, fbo_L[FBO__ORG].getWidth(), fbo_L[FBO__ORG].getHeight());
		
	shader_Mask.end();
	fbo_L[FBO__MASKED].end();
}

/******************************
******************************/
void ofApp::drawFbo_Inverted()
{
	ofDisableAlphaBlending();
	
	fbo_L[FBO__OUTPUT].begin();
	shader_Invert.begin();
		ofClear(0, 0, 0, 0);
		ofSetColor(255, 255, 255, 255);
	
		shader_Invert.setUniformTexture( "mask", fbo_L[FBO__GLITCH_INVERT].getTexture(), 1 );
		
		fbo_L[FBO__MASKED].draw(0, 0, fbo_L[FBO__OUTPUT].getWidth(), fbo_L[FBO__OUTPUT].getHeight());
		
	shader_Invert.end();
	fbo_L[FBO__OUTPUT].end();
}

/******************************
******************************/
void ofApp::drawFbo_PartsOfOrg(ofFbo& fbo, ofVec2f _pos)
{
	ofDisableAlphaBlending();
	
	fbo.begin();
		ofPushMatrix();
			
			ofTranslate(-_pos);
			
			ofClear(0, 0, 0, 0);
			ofSetColor(255, 255, 255, 255);
			fbo_L[FBO__ORG].draw(0, 0, fbo_L[FBO__ORG].getWidth(), fbo_L[FBO__ORG].getHeight());
			
		ofPopMatrix();
	fbo.end();
}

/******************************
******************************/
void ofApp::Set_myGlitch(int key, bool b_switch)
{
	if (key == 0)	myGlitch.setFx(OFXPOSTGLITCH_INVERT			, b_switch);
	if (key == 1)	myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, b_switch);
	if (key == 2)	myGlitch.setFx(OFXPOSTGLITCH_GLOW			, b_switch);
	if (key == 3)	myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, b_switch);
	if (key == 4)	myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, b_switch);
	if (key == 5)	myGlitch.setFx(OFXPOSTGLITCH_TWIST			, b_switch);
	if (key == 6)	myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, b_switch);
	if (key == 7)	myGlitch.setFx(OFXPOSTGLITCH_NOISE			, b_switch);
	if (key == 8)	myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, b_switch);
	if (key == 9)	myGlitch.setFx(OFXPOSTGLITCH_SWELL			, b_switch);
	if (key == 10)	myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, b_switch);
	if (key == 11)	myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, b_switch);
	if (key == 12)	myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, b_switch);
	if (key == 13)	myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, b_switch);
	if (key == 14)	myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, b_switch);
	if (key == 15)	myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, b_switch);
	if (key == 16)	myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, b_switch);
}

/******************************
******************************/
void ofApp::Clear_AllGlitch()
{
	for(int i = 0; i < NUM_GLITCH_TYPES; i++){
		Set_myGlitch(i, false);
	}
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case 'g':
			b_Start = true;
			break;
			
		case ' ':
			{
				char buf[BUF_SIZE_S];
				
				sprintf(buf, "image_%d.png", png_id);
				ofSaveScreen(buf);
				// ofSaveFrame();
				printf("> %s saved\n", buf);
				
				png_id++;
			}
			break;
		/*
		case '0':
			Mask_Rapair->Delete(ofGetElapsedTimef());
			break;
			
		case '1':
			Mask_Rapair->Rapair(ofGetElapsedTimef());
			break;
		*/
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
