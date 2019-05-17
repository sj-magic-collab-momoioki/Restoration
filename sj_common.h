/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "stdio.h"

#include "ofMain.h"
#include "ofxGui.h"

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/
enum{
	WINDOW_WIDTH	= 1800,	// 切れの良い解像度でないと、ofSaveScreen()での画面保存が上手く行かなかった(真っ暗な画面が保存されるだけ).
	WINDOW_HEIGHT	= 1080,
};

enum{
	FBO_L_WIDTH		= 800,
	FBO_L_HEIGHT	= 1000,
};

enum{
	BLOCK_SIZE = 135,
	
	BLOCK_DRAW_SIZE = 350,
};

enum{
	BUF_SIZE_S = 500,
	BUF_SIZE_M = 1000,
	BUF_SIZE_L = 6000,
};

/************************************************************
************************************************************/

/**************************************************
Derivation
	class MyClass : private Noncopyable {
	private:
	public:
	};
**************************************************/
class Noncopyable{
protected:
	Noncopyable() {}
	~Noncopyable() {}

private:
	void operator =(const Noncopyable& src);
	Noncopyable(const Noncopyable& src);
};

/**************************************************
**************************************************/
struct VBO_SET : private Noncopyable{
	ofVbo Vbo;
	vector<ofVec3f> VboVerts;
	vector<ofFloatColor> VboColor;
	
	void setup(int size){
		VboVerts.resize(size);
		VboColor.resize(size);
		
		Vbo.setVertexData(&VboVerts[0], VboVerts.size(), GL_DYNAMIC_DRAW);
		Vbo.setColorData(&VboColor[0], VboColor.size(), GL_DYNAMIC_DRAW);
	}
	
	void set_singleColor(const ofColor& color){
		for(int i = 0; i < VboColor.size(); i++) { VboColor[i].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255); }
	}
	
	void set_Color(int id, int NumPerId, const ofColor& color){
		for(int i = 0; i < NumPerId; i++){
			VboColor[id * NumPerId + i].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255);
		}
	}
	
	void set_Color(int id, const ofColor& color){
		VboColor[id].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255);
	}
	
	void update(){
		Vbo.updateVertexData(&VboVerts[0], VboVerts.size());
		Vbo.updateColorData(&VboColor[0], VboColor.size());
	}
	
	void draw(int drawMode){
		Vbo.draw(drawMode, 0, VboVerts.size());
	}
	
	void draw(int drawMode, int total){
		if(VboVerts.size() < total) total = VboVerts.size();
		Vbo.draw(drawMode, 0, total);
	}
};

/**************************************************
**************************************************/
class GUI_GLOBAL{
private:
	/****************************************
	****************************************/
	
public:
	/****************************************
	****************************************/
	void setup(string GuiName, string FileName = "gui.xml", float x = 10, float y = 10);
	
	ofxGuiGroup Group_Graph;
		ofxFloatSlider LPFAlpha_dt__FFTGain;
		ofxFloatSlider LPFAlpha_dt__FFTPhase;
		
		ofxFloatSlider Val_DispMax__FFTGain;
		ofxFloatSlider Val_DispMax__Sin;
		
	ofxGuiGroup Group_LMH;
		ofxIntSlider Id_Low;
		ofxIntSlider Id_Middle;
		ofxIntSlider Id_High;
		
	ofxGuiGroup Group_Overlay;
		ofxToggle b_Do_Overlay;
		ofxFloatSlider Overlay_Alpha;
		
		ofxToggle b_Overlay_L;
		ofxToggle b_Overlay_M;
		ofxToggle b_Overlay_H;
		
	ofxGuiGroup Group_Camera;
		ofxToggle b_Cam;
		ofxColorSlider gui_VideoColor;
		
	
	/****************************************
	****************************************/
	ofxPanel gui;
};

/************************************************************
************************************************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt);
double LPF(double LastVal, double CurrentVal, double Alpha);
double sj_max(double a, double b);

/************************************************************
************************************************************/
extern GUI_GLOBAL* Gui_Global;

extern FILE* fp_Log;

extern int GPIO_0;
extern int GPIO_1;


/************************************************************
************************************************************/

