/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"

/************************************************************
************************************************************/

class L_UTIL{
private:

public:
	static void Array_Forward(int* Array, int Num);
	static void Array_Forward(vector<int>& Array);
	static void Array_Reverse(int* Array, int Num);
	static void Array_Reverse(vector<int>& Array);
	static void FisherYates(int* Array, int Num);
	static void FisherYates(vector<int>& Array);
	
	static int Dice_index(int *Weight, int NUM);
	static int Dice_index(vector<int>& Weight, int NUM);
	
	static ofVec2f CalBlockPos_LeftUp(int id, ofVec2f BlockSize, int NUM_BLOCKS_W);
	static ofVec2f CalBlockPos_Center(int id, ofVec2f BlockSize, int NUM_BLOCKS_W);
	static ofVec2f ExchangeBlockPos_LeftUp_to_Center(ofVec2f _pos, ofVec2f BlockSize);
	static ofVec2f ExchangeBlockPos_Center_to_LeftUp(ofVec2f _pos, ofVec2f BlockSize);
};
