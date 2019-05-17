#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char** argv ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	/********************
	********************/
	bool b_Play_EnjoyingMusic = false;

	/********************
	********************/
	printf("---------------------------------\n");
	printf("> parameters\n");
	printf("\t-p:b_Play_EnjoyingMusic(false)\n");
	printf("---------------------------------\n");
	
	for(int i = 1; i < argc; i++){
		if( strcmp(argv[i], "-p") == 0 ){
			if(i+1 < argc){
				if(atoi(argv[i+1]) == 1)	b_Play_EnjoyingMusic = true;
			}
		}
	}
	
	ofRunApp(new ofApp(b_Play_EnjoyingMusic));
}
