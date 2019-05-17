/************************************************************
************************************************************/
#include "util.h"

/************************************************************
************************************************************/

/******************************
******************************/
void L_UTIL::Array_Forward(int* Array, int Num)
{
	for(int i = 0; i < Num; i++){
		Array[i] = i;
	}
}

/******************************
******************************/
void L_UTIL::Array_Forward(vector<int>& Array)
{
	int Num = Array.size();
	
	for(int i = 0; i < Num; i++){
		Array[i] = i;
	}
}

/******************************
******************************/
void L_UTIL::Array_Reverse(int* Array, int Num)
{
	for(int i = 0; i < Num; i++){
		Array[i] = Num - 1 - i;
	}
}

/******************************
******************************/
void L_UTIL::Array_Reverse(vector<int>& Array)
{
	int Num = Array.size();
	
	for(int i = 0; i < Num; i++){
		Array[i] = Num - 1 - i;
	}
}

/******************************
******************************/
void L_UTIL::FisherYates(int* Array, int Num)
{
	/********************
	********************/
	for(int i = 0; i < Num; i++){
		Array[i] = i;
	}

	/********************
	********************/
	int i = Num;
	while(i--){
		/********************
		int j = rand() % (i + 1);
		********************/
		int j = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (i + 1) );
		
		/********************
		********************/
		int temp = Array[i];
		Array[i] = Array[j];
		Array[j] = temp;
	}
}

/******************************
******************************/
void L_UTIL::FisherYates(vector<int>& Array)
{
	int Num = Array.size();
	
	/********************
	********************/
	for(int i = 0; i < Num; i++){
		Array[i] = i;
	}

	/********************
	********************/
	int i = Num;
	while(i--){
		/********************
		int j = rand() % (i + 1);
		********************/
		int j = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * (i + 1) );
		
		/********************
		********************/
		int temp = Array[i];
		Array[i] = Array[j];
		Array[j] = temp;
	}
}

/******************************
******************************/
int L_UTIL::Dice_index(int *Weight, int NUM)
{
	/***********************
	cal sum of Weight
	***********************/
	int TotalWeight = 0;
	int i;
	for(i = 0; i < NUM; i++){
		TotalWeight += Weight[i];
	}
	
	if(TotalWeight == 0) { return -1; }
	
	/***********************
	random number
	***********************/
	// int RandomNumber = rand() % TotalWeight;
	int RandomNumber = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * TotalWeight );
	
	/***********************
	define play mode
	***********************/
	int index;
	for(TotalWeight = 0, index = 0; /* */; index++){
		TotalWeight += Weight[index];
		if(RandomNumber < TotalWeight) break;
	}
	
	return index;
}

/******************************
description
	引数"NUM"は、Weightの全体数でなく、確認する数なので、必要.
******************************/
int L_UTIL::Dice_index(vector<int>& Weight, int NUM)
{
	/***********************
	***********************/
	if(Weight.size() < NUM){
		Weight.resize(NUM);
	}
	
	/***********************
	cal sum of Weight
	***********************/
	int TotalWeight = 0;
	int i;
	for(i = 0; i < NUM; i++){
		TotalWeight += Weight[i];
	}
	
	if(TotalWeight == 0) { return -1; }
	
	/***********************
	random number
	***********************/
	// int RandomNumber = rand() % TotalWeight;
	int RandomNumber = (int)( ((double)rand() / ((double)RAND_MAX + 1)) * TotalWeight );
	
	/***********************
	define play mode
	***********************/
	int index;
	for(TotalWeight = 0, index = 0; /* */; index++){
		TotalWeight += Weight[index];
		if(RandomNumber < TotalWeight) break;
	}
	
	return index;
}

/******************************
******************************/
ofVec2f L_UTIL::CalBlockPos_LeftUp(int id, ofVec2f BlockSize, int NUM_BLOCKS_W)
{
	int id_h = int(id / NUM_BLOCKS_W);
	int id_w = int(id % NUM_BLOCKS_W);
	
	ofVec2f LeftUp(BlockSize.x * id_w, BlockSize.y * id_h);
	return LeftUp;
}

/******************************
******************************/
ofVec2f L_UTIL::CalBlockPos_Center(int id, ofVec2f BlockSize, int NUM_BLOCKS_W)
{
	ofVec2f LeftUp = CalBlockPos_LeftUp(id, BlockSize, NUM_BLOCKS_W);
	return ExchangeBlockPos_LeftUp_to_Center(LeftUp, BlockSize);
}

/******************************
******************************/
ofVec2f L_UTIL::ExchangeBlockPos_LeftUp_to_Center(ofVec2f _pos, ofVec2f BlockSize)
{
	return ofVec2f(_pos.x + BlockSize.x/2, _pos.y + BlockSize.y/2);
}

/******************************
******************************/
ofVec2f L_UTIL::ExchangeBlockPos_Center_to_LeftUp(ofVec2f _pos, ofVec2f BlockSize)
{
	return ofVec2f(_pos.x - BlockSize.x/2, _pos.y - BlockSize.y/2);
}



