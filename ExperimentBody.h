#pragma once

#include "emil/EMIL.hpp"
#include <queue>
#include <io.h>

using namespace std;

class ExperimentBody: public CExperiment
{
public:
	ExperimentBody(int pxWidth, int pxHeight, int RefreshRate, CCfgFile* Params);

	/// Standard event handlers
	void initialize();
	void finalize();

	void eventRender(unsigned int FrameCount, CEOSData* Samples);
	//void eventKeyboard(unsigned char key, int x, int y);
	void eventJoypad();

private:

	// Current experiment state
	enum STATE {
		STATE_LOADING,
		STATE_TESTCALIBRATION,
		STATE_FIXATION,
		STATE_CUE,
		STATE_TARGET,
		STATE_RESPONSE,
	};

	STATE m_state;

	void gotoLoading();
	void gotoFixation();
    void saveData();
	void loadProgress();
	void loadSession();// load session master list
	int ExperimentBody::RandomNumber(int high, int low);

	// Configuration file
	CCfgFile* m_paramsFile;

	//CSolidPlane* m_fixation;
	CSolidPlane* m_Ltarget; //*
	CSolidPlane* m_Rtarget; //*
	CImagePlane* m_cue;
	CImagePlane* m_neutralcue;
	CImagePlane* m_box;
	CImagePlane* m_box1;
	CImagePlane* m_box2;


	// Stimuli for the test calibration
	CImagePlane* m_redcross;
	CImagePlane* m_whitecross;
	CImagePlane* m_fixation;

	// timers
	CTimer m_timer;
	CTimer m_timerCheck;
	CTimer m_timerExp;
	CTimer m_timerfixation;
	CTimer m_timercue; 
	CTimer m_timerisi;
	CTimer m_timerhold;
	CTimer m_timertarget;
	
	float XTargetOffset;
	float YTargetOffset; //*
	int TrialType;
	float ResponseTime;
	int CueDirection;
	//* int CueTargetTime;
	int LGapDirection; //*
	int RGapDirection; //*
	int GapSize; //*
	int AnswerDirection;

	int Curr_stim;
	int Dist;
	int Increment;
	int m_numCompleted;
	int m_numTestCalibration;
	int m_sessionBegin;
	int m_sessionEnd;
	int ResponseFinalize;
	int TestCalibration;
	int WAIT_RESPONSE;
	int m_curStim;
	int gate;
	int TrialNumber;
	int debug;
	int NumberValidTrials;
	int NumberInvalidTrials;
	int NumberNeutralTrials;
	

	float TimeTargetON;
	float TimeTargetOFF;
	float TimeCueON;
	float TimeCueOFF;
	float TimeFixationON;
	float TimeFixationOFF;
	float X;
	float Y;
	float Xstab;
	float Ystab;
	float xPos;
	float yPos;
	float xshift;
	float yshift;
	float CurrentRTValid;
	float CurrentRTInvalid;
	float CurrentRTNeutral;
	
	bool SHOW_TARGET;
	bool isCorrect;
	bool isAnswered;


	vector<int> TrialTypeList;
	vector<int> CueDirectionList;
	//* vector<int> CueTargetTimingList;
	vector<int> LGapDirectionList; //*
	vector<int> RGapDirectionList; //*
	vector<int> GapSizeList; //*

};

//*: changed
