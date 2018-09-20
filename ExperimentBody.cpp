#include "stdafx.h"
#include "ExperimentBody.h"
#include "FovealAttention.h"
#include <math.h>

const float PI = 3.1415926353f;

// ad 8 26 2013 - add two dots (as boxes at the two sides for the small posner version

///////////////////////////////////////////////////////////////////////////////////
ExperimentBody::ExperimentBody(int pxWidth, int pxHeight, int RefreshRate, CCfgFile* Params) :
	CExperiment(pxWidth, pxHeight, RefreshRate), 

	m_paramsFile(Params)
{
	setExperimentName("FovealAttention");

	CMath::srand(time(NULL));
}

///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::initialize()
{		
	CStabilizer::Instance()->enableSlowStabilization(true);
	
	m_fixation = addObject(new CImagePlane("images/fixation.tga"));
	m_fixation->enableTrasparency(true);
    m_fixation->pxSetSize(10,10);
	m_fixation->pxSetPosition(0,0);
	m_fixation->hide();
	
	m_cue = addObject(new CImagePlane("images/cue.tga"));
	m_cue->enableTrasparency(true);
	m_cue->pxSetSize(m_paramsFile->getInteger(CFG_CUE_SIZE),m_paramsFile->getInteger(CFG_CUE_SIZE));
	m_cue->pxSetPosition(0,0);
	m_cue->hide();
	
	m_neutralcue = addObject(new CImagePlane("images/neutralcue.tga"));
	m_neutralcue->enableTrasparency(true);
	m_neutralcue->pxSetSize(m_paramsFile->getInteger(CFG_CUE_SIZE),m_paramsFile->getInteger(CFG_CUE_SIZE));
	m_neutralcue->pxSetPosition(0,0);
	m_neutralcue->hide();
	
	m_box = addObject(new CImagePlane("images/box.tga"));
	m_box->enableTrasparency(true);
	m_box->pxSetSize(m_paramsFile->getInteger(CFG_BOX_SIZE),m_paramsFile->getInteger(CFG_BOX_SIZE));
	m_box->pxSetPosition(0,0);
	m_box->enableTrasparency(true);
	m_box->hide();

	

	// large and small posner
	if (m_paramsFile->getInteger(CFG_TARGET_OFFSET)>100)
	{
	m_box1 = addObject(new CImagePlane("images/box.tga"));
	m_box1->enableTrasparency(true);
	m_box1->pxSetSize(m_paramsFile->getInteger(CFG_BOX_SIZE),m_paramsFile->getInteger(CFG_BOX_SIZE));
	m_box1->pxSetPosition(0,0);
	m_box1->enableTrasparency(true);
	m_box1->hide();

	m_box2 = addObject(new CImagePlane("images/box.tga"));
	m_box2->enableTrasparency(true);
	m_box2->pxSetSize(m_paramsFile->getInteger(CFG_BOX_SIZE),m_paramsFile->getInteger(CFG_BOX_SIZE));
	m_box2->pxSetPosition(0,0);
	m_box2->enableTrasparency(true);
	m_box2->hide();
	}
	else
	{
	m_box1 = addObject(new CImagePlane("images/boxsmall.tga"));
    m_box1->pxSetSize(m_paramsFile->getInteger(CFG_TARGET_SIZE),m_paramsFile->getInteger(CFG_TARGET_SIZE));
	m_box1->hide();


	m_box2 = addObject(new CImagePlane("images/boxsmall.tga"));
    m_box2->pxSetSize(m_paramsFile->getInteger(CFG_TARGET_SIZE),m_paramsFile->getInteger(CFG_TARGET_SIZE));
	m_box2->hide();
	}
	
	m_target = addObject(new CSolidPlane(400,400,400));
    m_target->pxSetSize(m_paramsFile->getInteger(CFG_TARGET_SIZE),m_paramsFile->getInteger(CFG_TARGET_SIZE));
	// color red
	m_target->setColor(255,0,0);
	m_target->hide();





	// set the pixel increment for the test calibration procedure during the exp
	Increment = 1;
	ResponseFinalize = 0;
	xshift = 0;
	yshift = 0;
	xPos = 0;
	yPos = 0;
	TrialNumber = 1;
	m_numTestCalibration = 0;
	Curr_stim = 0;
	CurrentRTValid = 0;
	NumberValidTrials = 0;
	CurrentRTInvalid = 0;
	NumberInvalidTrials = 0;
    CurrentRTNeutral = 0;
	NumberNeutralTrials = 0;
	// set TestCalibration = 1 so that the experiment will start with a recalibration trial
	TestCalibration = 1;
	m_numCompleted  = 0;
	
	// crosses for the recalibration trials	
	m_redcross = addObject(new CImagePlane("images/redcross.tga"));
	m_redcross->enableTrasparency(true);
	m_redcross->hide();
	m_whitecross = addObject(new CImagePlane("images/whitecross.tga"));
	m_whitecross->enableTrasparency(true);
	m_whitecross->hide();	

	//Load session data
	loadSession();
	loadProgress();

	disable(CExperiment::EIS_PHOTOCELL);
	disable(CExperiment::EIS_NOTRACK_ICON);
	disable(CExperiment::EIS_STAT1);
	
	// set the distance of the target from the gaze
	XTargetOffset = m_paramsFile->getInteger(CFG_TARGET_OFFSET);
	debug = m_paramsFile->getInteger(CFG_DEBUG);
	
	hideAllObjects();
	m_state = STATE_LOADING;
	m_timer.start(1000);

	WAIT_RESPONSE = 1;
	
	/* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.*/
	
    srand( (unsigned)time( NULL ) );


}
///////////////////////////////////////////////////////////////////////////////////
// write the progress file
void ExperimentBody::finalize()
{
	// write progress
	if(m_numCompleted > 0)//check to see that some experiments have been conducted
	{
		char LocalDate[1024];
		time_t t = time(NULL);
		strftime(LocalDate, 1024, "%Y-%m-%d-%H-%M-%S", localtime(&t));

		ostringstream fstr1;
		fstr1<< "Data\\"<<m_paramsFile->getString(CFG_SUBJECT_NAME)<<"\\"<<m_paramsFile->getString(CFG_SUBJECT_NAME) <<"Progress.txt";
	
		ofstream out1(fstr1.str().c_str(), ios::app);

		if(!out1.is_open())
		{
			CEnvironment::Instance()->outputMessage(CEnvironment::ENV_MSG_ERROR, "Session data file (%sProgress.txt) could not be opened.  Please check that the file exists.", m_paramsFile->getString(CFG_SUBJECT_NAME));
			CEnvironment::Instance()->outputMessage(CEnvironment::ENV_MSG_ERROR, "Index of last saved file: %i.", m_sessionEnd);
			declareFinished();

		}
	    // keep track of the last trial played in the list and start from there in the next session
		out1<< "Recorded: " << LocalDate <<endl;
		out1<< m_sessionBegin  << endl;
		out1<< m_sessionEnd << endl;
		
	}

}
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::eventRender(unsigned int FrameCount, CEOSData* Samples)
{


	// store the stabilized trace ( --- all in px)
	if (SHOW_TARGET==0)
	{
	CStabilizer::Instance()->
				stabilize(Samples, X, Y);  
    X = X+xshift;
	Y = Y+yshift;
	}
	storeTrialStream(0, X);
	storeTrialStream(1, Y);
	// add the offset

	if (debug==1)
	{
	X = 0;
	Y = 0;
	}
	
	m_box1->pxSetPosition(X+XTargetOffset, Y);
	m_box2->pxSetPosition(X-XTargetOffset, Y);
	m_box1->show();
    m_box2->show();
	float x;
	float y;
	float dist;
	//m_box->show();
	//m_whitecross->show();
 
	switch (m_state) {
	case STATE_LOADING:

		
		COGLEngine::Instance()->clearScreen();
		glColor3d(255, 255, 255);
		printCentered(CFontEngine::FONTS_ARIAL_18, 0, 0, "Loading...");
		
		// Set the background color for the experiment
		//COGLEngine::Instance()->setBackgroundColor(100, 100, 100);
		COGLEngine::Instance()->setBackgroundColor(0, 0, 0);
		
		// Copy the parameter file into the subject directory
		if (m_timer.isExpired())
		{	
			char LocalDate[1024];
			time_t t = time(NULL);
			strftime(LocalDate, 1024, "%Y-%m-%d-%H-%M-%S", localtime(&t));

			ostringstream DestinationFileName;
			DestinationFileName << m_paramsFile->getDirectory(CFG_DATA_DESTINATION) << 
				m_paramsFile->getString(CFG_SUBJECT_NAME) << "/" << m_paramsFile->getString(CFG_SUBJECT_NAME) << 
				"-" << LocalDate << "-params.cfg";;			
			gotoFixation();
		}

		break;

	case STATE_TESTCALIBRATION:
		
		m_box1->hide();
		m_box2->hide();
		if (!m_timerCheck.isExpired())
		{
			CConverter::Instance()->a2p(Samples->x1, Samples->y1, x, y);
		}
		else
		{
			//CEnvironment::Instance()->outputMessage("State Test Calibration, ResponseFinalize: %.0i", ResponseFinalize);
		if (!(ResponseFinalize == 1))
		{
			CConverter::Instance()->a2p(Samples->x1, Samples->y1, x, y);
			moveToFront(m_redcross);
			m_redcross->pxSetPosition(x+xshift+xPos,y+yshift+yPos);			
			//m_redcross->pxSetPosition(x,y);
			m_redcross->show();
			m_whitecross->pxSetPosition(0,0);
			m_whitecross->show();
		}
		else
		{
			//CEnvironment::Instance()->outputMessage("State Test Calibration");
			TestCalibration = 0;

			xshift = xPos+xshift;
			yshift = yPos+yshift;
			//CEnvironment::Instance()->outputMessage("State Test Calibration, xshift: %.2f", xshift);
		    //CEnvironment::Instance()->outputMessage("State Test Calibration, yshift: %.2f", yshift);
			//CEnvironment::Instance()->outputMessage("----------------------------------------------------------------");


			m_redcross->hide();
			m_whitecross->hide();
			
			endTrial();
			gotoFixation();

		}

		}
				break;

    //  STATE_FIXATION: the box appears at the center of the screen, subject is instructed to move the gaze at the center of the box,
	//                  as soon as the gaze is close to the center a fixation point appears at the center of gaze
	//                  the fixation point is stabilized and fixation should be maintained for 1 s or until the gaze is stabilized
	//                  than go to the next state.
	case STATE_FIXATION:
	
	// check if the subject is looking at the center
	//use stabilized positions acquired at the beginning of the render cycle 
	Dist = sqrt((pow(0 - (X) , 2) + pow(0 - (Y) , 2)));

	if (((Dist < 100)&& 
			!CTriggers::any(Samples->triggers, Samples->samplesNumber, EOS_TRIG_1_BADDATA) &&
			!CTriggers::any(Samples->triggers, Samples->samplesNumber, EOS_TRIG_1_EMEVENT) ) && (gate==1)||
			((debug == 1) && (gate==1)))
	{		
	       // enter here only the first time stabilization is activated
	
			// this initialize stabilizer settings the first time
			//	stabilization routine is entered.
			// reset the filter with positions in arcmin
				CStabilizer::Instance()->
					resetFilter(Samples->x1, Samples->y1);
				gate = 0;
			// time for fixation ON
				TimeFixationON = m_timerExp.getTime();
				//CEnvironment::Instance()->outputMessage("State Fixation");
			// fixation should stay on for one sec or as far as the gaze is stabilized
				m_timerfixation.start(m_paramsFile->getInteger(CFG_FIXATION_TIME));
			
	}
	if (gate==0)
	{

			// update the stabilized coordinates based on the offset introduced by the subject
			if (debug == 1)
			{
				X = 0;
				Y = 0;
			}
				
			// show the stabilized fixation marker with slow stabilization
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();

			if (m_timerfixation.isExpired())
			{
			// if the subject is looking in the central area show the stabilized stimulus
			m_state = STATE_CUE;
			}

			//CEnvironment::Instance()->outputMessage("State Cue");
	}


		
		break;

	// STATE_CUE: wait until subject stabilized fixation
	//            present the cue
	//            stabilize the cue for the ISI time
	//            go to the next state and present the target
	case STATE_CUE: 

		// check if the subject is looking at the fixation and that there is a drift (no saccades or notrack)
		if (((gate==0) && 
			!CTriggers::any(Samples->triggers, Samples->samplesNumber, EOS_TRIG_1_BADDATA) &&
			!CTriggers::any(Samples->triggers, Samples->samplesNumber, EOS_TRIG_1_EMEVENT) && (m_timerfixation.isExpired()))||
			((debug==1) && (gate==0)))
		{

			if (debug==1)
			{
				X = 0;
			    Y = 0;
			}
			// show the cue at fixation and hide the fixation point
			m_fixation->hide();
			TimeFixationOFF = m_timerExp.getTime();
			// if neutral cue is on or if a neutral catch trial is on
			if ((TrialType == 2) || ((TrialType == 3) && (CueDirection == 3)))
			{
				m_neutralcue->pxSetPosition(X,Y);
				m_neutralcue->show();
			}
			else
			{
				m_cue->pxSetPosition(X,Y);
				m_cue->show();
			}
			// time for cue presentation
			TimeCueON = m_timerExp.getTime();

			gate = 1;
			// set the time for the cue 
			m_timercue.start(m_paramsFile->getInteger(CFG_CUE_TIME));

		}
		if ((!m_timercue.isExpired()) && (gate==1))
		{
			// continue to stabilize the cue
			if(debug==1)
			{
				X = 0;
				Y = 0;
			}
			if ((TrialType == 2) || ((TrialType == 3) && (CueDirection == 3)))
			{
				m_neutralcue->pxSetPosition(X,Y);
				m_neutralcue->show();
				
			}			
			else
			{
			m_cue->pxSetPosition(X,Y);
			m_cue->show();	
			m_fixation->hide();
			}
		}
		// when the time for the cue is over go to the next state 
		else if ((m_timercue.isExpired()) && (gate==1))
		{

			if (!(TrialType == 3))
			{
			
			m_state = STATE_TARGET;
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();
			
			// set the time before the target appears after the cue (based on the ISI --> take this from the parameter file)
			//m_timerisi.start(m_paramsFile->getInteger(CFG_CUE_TARGET_TIME));
	       CEnvironment::Instance()->outputMessage("Cue target timing: %.2i", CueTargetTime);

			m_timerisi.start(CueTargetTime);
			m_cue->hide();
			m_neutralcue->hide();
			gate = 0;
			TimeCueOFF = m_timerExp.getTime();
			}
			// catch trial (no target appears -> go directly to response)
			if (TrialType == 3) 
			{
			m_timerhold.start(m_paramsFile->getInteger(CFG_HOLD_TIME));
			m_state = STATE_RESPONSE;
			WAIT_RESPONSE = 0;
			m_cue->hide();
			m_neutralcue->hide();
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();
			TimeCueOFF = m_timerExp.getTime();
			} 
		}

		break;
	
	// STATE_TARGET: present the target
	//              go to the next state and wait for subject response
	case STATE_TARGET:
			
			//CEnvironment::Instance()->outputMessage("State Target");
			// continue to stabilize the fixation marker;
		
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();
			if (debug==1)
			{
				X = 0;
				Y = 0;
			}

			if (m_timerisi.isExpired() && (gate==0))
			{
		    SHOW_TARGET = 1;
			m_target->pxSetPosition(X+XTargetOffset, Y);
			m_target->show();	
			TimeTargetON = m_timerExp.getTime();
			// set target duration
			m_timertarget.start(m_paramsFile->getInteger(CFG_TARGET_TIME));
			gate = 1;
			}
			if ((gate==1) && (m_timertarget.isExpired()))
			{ // go to the next state
			
			m_state = STATE_RESPONSE;
			m_target->hide();
			//CEnvironment::Instance()->outputMessage("State Response");
			m_timerhold.start(m_paramsFile->getInteger(CFG_HOLD_TIME));
			WAIT_RESPONSE = 0;
			TimeTargetOFF = m_timerExp.getTime();
			
			}
			else if ((gate==1) && (!m_timertarget.isExpired()))
			{
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();			
			m_target->pxSetPosition(X+XTargetOffset, Y);
			m_target->show();
			}

	
	break;
	
	// STATE_RESPONSE: wait for subject response
	case STATE_RESPONSE:
			
			if (!(m_timerhold.isExpired()))
			{
			// continue to stabilize the fixation marker
			if (debug==1)
			{
				X = 0;
				Y = 0;
			}
			m_fixation->pxSetPosition(X,Y);
			m_fixation->show();
			}

			if ((m_timerhold.isExpired()) && (WAIT_RESPONSE == 0))
			{
			// response is not given
			ResponseTime = 0;	
			// hide all the objects 
			hideAllObjects();
			m_target->hide();
			m_fixation->hide();
			//m_box->hide();
			
			endTrial();
			saveData();				 
			}
			else if (WAIT_RESPONSE == 1)
			{
			// hide all the objects 
			hideAllObjects();
			m_target->hide();
			m_fixation->hide();
			//m_box->hide();
			endTrial();
			saveData();		
			}	
            	
		break;
		}
	}		
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::eventJoypad()
{
	// activate the joypad only in the state calibration	

			if (m_state == STATE_TESTCALIBRATION) 
			{				

				if (CDriver_Joypad::Instance()->getButtonStatus(CDriver_Joypad::JPAD_BUTTON_UP)) // moving the cursor up
				{
					yPos = yPos + Increment; //position of the cross
				}
			
				else if (CDriver_Joypad::Instance()->getButtonPressed(CDriver_Joypad::JPAD_BUTTON_DOWN)) // moving the cursor down
				{	
					yPos = yPos - Increment;
				}
		
				else if (CDriver_Joypad::Instance()->getButtonPressed(CDriver_Joypad::JPAD_BUTTON_RGHT)) // moving the cursor to the right
				{
					xPos = xPos + Increment;

				}

				else if (CDriver_Joypad::Instance()->getButtonPressed(CDriver_Joypad::JPAD_BUTTON_LEFT)) // moving the cursor to the left
				{
					xPos = xPos - Increment;
				
				}
				
				if (CDriver_Joypad::Instance()->getButtonStatus(CDriver_Joypad::JPAD_BUTTON_R1)) // finalize the response
				{
					//CEnvironment::Instance()->outputMessage("RsponseFinalize event Joypad");
						ResponseFinalize = 1; // click the left botton to finalize the response

				}
		   }
			if (STATE_RESPONSE)
			{
				if (CDriver_Joypad::Instance()->getButtonStatus(CDriver_Joypad::JPAD_BUTTON_R1)) // location of T1 identified
				{
					WAIT_RESPONSE = 1;
					// get the time of the response here
					ResponseTime =  m_timerExp.getTime();
				}
			}
}
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::gotoFixation() 
{
	
	SHOW_TARGET = 0;
	if (!(TestCalibration==1))
	CEnvironment::Instance()->outputMessage("Trial Number: %d", (TrialNumber));


	hideAllObjects();
	
	if (TestCalibration == 1)
	{
			m_state = STATE_TESTCALIBRATION;
			//CEnvironment::Instance()->outputMessage("Calibration trial");
			ResponseFinalize = 0;
			m_timerCheck.start(500);
	}
	else
	{
		    m_state = STATE_FIXATION;
			gate = 1;
			// show the box at the center of the screen
			//m_box->show();
	
	
	
	
		// if trial is a catch trial target does not appear
		TrialType = TrialTypeList[m_curStim+1];
		CueDirection = CueDirectionList[m_curStim+1];
		CueTargetTime = CueTargetTimingList[m_curStim+1];
		// valid trial
		if (TrialType == 1) 
		{
	      NumberValidTrials++;
		if (CueDirection==-1)
		{
		CEnvironment::Instance()->outputMessage("Valid trial - cue direction to the left ");
		// note: the cue originally points to the left, when the cue points right, it need to be rotated
		m_cue->degSetAngle(0);
		}
		if (CueDirection==1)
		{
		CEnvironment::Instance()->outputMessage("Valid trial - cue direction to the right ");
		m_cue->degSetAngle(180);
		}		
		XTargetOffset = XTargetOffset * CueDirection;		
		}
		// invalid trial
		else if (TrialType == 0)
		{
		NumberInvalidTrials++;
		if (CueDirection==-1)
		{
		CEnvironment::Instance()->outputMessage("Invalid trial - cue direction to the left ");
		m_cue->degSetAngle(0);
		}
		if (CueDirection==1)
		{
		CEnvironment::Instance()->outputMessage("Invalid trial - cue direction to the right ");
		m_cue->degSetAngle(180);
		}
		
		XTargetOffset = XTargetOffset * -CueDirection;	
	
		}
		// neutral trials (a neutral cue is shown)
		else if (TrialType == 2)
		{
	    NumberNeutralTrials++;
		if (CueDirection==-1)
		{
		CEnvironment::Instance()->outputMessage("Neutral trial - target to the right ");
		m_cue->degSetAngle(0);
		}
		if (CueDirection==1)
		{
		CEnvironment::Instance()->outputMessage("Neutral trial - target to the left");
		m_cue->degSetAngle(180);
		}
		
		XTargetOffset = XTargetOffset * -CueDirection;	
		}
		// catch trial (no target appears)
		else if (TrialType == 3)
		{
		if (CueDirection==-1)
		{
		CEnvironment::Instance()->outputMessage("Catch trial - cue direction to the left ");
		m_cue->degSetAngle(0);
		}
		if (CueDirection==1)
		{
		CEnvironment::Instance()->outputMessage("Catch trial - cue direction to the right ");
		m_cue->degSetAngle(180);
		}
		if (CueDirection==3)
		{
		CEnvironment::Instance()->outputMessage("Catch trial - cue direction neutral ");
		}
		}
	
		}

	// start the trial
	WAIT_RESPONSE=1;	
	startTrial();
	m_timerExp.start();
	m_timer.start(1000);
	gate=1;

	if(m_curStim == (CueDirectionList.size()-1) )
	{
		m_curStim = 0;//loop the stimulus list from the beginning 
		CEnvironment::Instance()->outputMessage(CEnvironment::ENV_MSG_WARNING, "Cue list exhausted.  Restarting from beginning of list.");
	}
		
}
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::saveData()
{

		if (ResponseTime>0)
	    // give confrimation of response
		Beep(600,400);

		// time of the response (locked to the start of the trial)
		storeTrialVariable("ResponseTime", ResponseTime);
		storeTrialVariable("TimeCueON", TimeCueON);
		storeTrialVariable("TimeCueOFF", TimeCueOFF);
		storeTrialVariable("TimeFixationON", TimeFixationON); 
		storeTrialVariable("TimeFixationOFF", TimeFixationOFF); 
		storeTrialVariable("TimeTargetON", TimeTargetON);
		storeTrialVariable("TimeTargetOFF", TimeTargetOFF);

		
		CEnvironment::Instance()->outputMessage("Cue target timing: %.2i", CueTargetTime);
		if (TrialType==1)
		{
		CurrentRTValid = CurrentRTValid + (ResponseTime-TimeTargetOFF);
		CEnvironment::Instance()->outputMessage("valid trial number - %i ", NumberValidTrials);
		CEnvironment::Instance()->outputMessage("RT valid (running average) - %f ", CurrentRTValid/NumberValidTrials);
		}

		if (TrialType==0)
		{
		CurrentRTInvalid = CurrentRTInvalid + (ResponseTime-TimeTargetOFF);
		CEnvironment::Instance()->outputMessage("invalid trial number - %i ", NumberInvalidTrials);
		CEnvironment::Instance()->outputMessage("RT invalid (running average) - %f ", CurrentRTInvalid/NumberInvalidTrials);
		}

		if (TrialType==2)
		{
		CurrentRTNeutral = CurrentRTNeutral + (ResponseTime-TimeTargetOFF);
		CEnvironment::Instance()->outputMessage("neutral trial number - %i ", NumberNeutralTrials);
		CEnvironment::Instance()->outputMessage("RT neutral (running average) - %f ", CurrentRTNeutral/NumberNeutralTrials);
		}

		
		//trial type 
		storeTrialVariable("TrialType", TrialType);
		// cue type 
		storeTrialVariable("CueType", CueDirection);
		// cue target timing 
		storeTrialVariable("CueTargetTiming", CueTargetTime);
		
		// save size of stimuli in px
		storeTrialVariable("FixationMarkerSize", m_paramsFile->getInteger(CFG_FIXATION_SIZE));
		storeTrialVariable("CueSize", m_paramsFile->getInteger(CFG_CUE_SIZE));
		storeTrialVariable("TargetSize", m_paramsFile->getInteger(CFG_TARGET_SIZE));
		storeTrialVariable("BoxSize", m_paramsFile->getInteger(CFG_BOX_SIZE));		
		

		// 
		storeTrialVariable("X_TargetOffset", XTargetOffset);//px
		storeTrialVariable("Subject_Name", m_paramsFile->getString(CFG_SUBJECT_NAME));
		
		// save information about the test calibration
		storeTrialVariable("TestCalibration", TestCalibration);
		storeTrialVariable("xoffset", xshift);
		storeTrialVariable("yoffset", yshift); //px

		storeTrialVariable("debug", debug); //px

		saveTrial("./Data/" + m_paramsFile->getString(CFG_SUBJECT_NAME));
		
		//update progress through session
		m_sessionEnd = m_curStim-1;
		
		// keep track of the test calibration trials
		m_numTestCalibration++;
		
		// recalibration active at each trial
		if (m_numTestCalibration == 1) 
	    {
			xPos = 0;
			yPos = 0;
			TestCalibration = 1;
			ResponseFinalize = 0;
			m_numTestCalibration = 0;
			m_timerCheck.start(100);
			m_whitecross->pxSetPosition(0,0);
			m_whitecross->show();
	    }

		m_curStim++;
		m_numCompleted ++;
		TrialNumber++;
		CEnvironment::Instance()->outputMessage("-----------------------------------------------------");
		XTargetOffset = m_paramsFile->getInteger(CFG_TARGET_OFFSET);
		gotoFixation();

}
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::loadSession()
{
	//read stmuli from the stim list 
	// [ stimList contains information about whether the cue indicates left or right and whether it is valid, invalid or neutral]
	ostringstream fstr1;
	fstr1<<"StimList.txt";
	
	string tempStr;
	int tempInt;
	ifstream in1(fstr1.str().c_str());

	if(!in1.is_open())
	{
		CEnvironment::Instance()->outputMessage(CEnvironment::ENV_MSG_ERROR, "Session data file (%sStimList.txt) could not be opened.  Please check that the file exists.", m_paramsFile->getString(CFG_SUBJECT_NAME));
		declareFinished();

	}
	
	// the first number on each sequence in the StimList indicates the trial number
	// the second number indicates whether the cue indicates left or right (-1= left 1= right)
	// the third number indicats whether the cue is valid invalid or neutral (valid = 1 invalid = 0 neutral = 2 catch = 3)

	while(in1.peek() != EOF)
	{
		in1>>tempStr;//discard *

		in1>>tempInt;// discard (trialnumber)

		in1>>tempInt; // cue direction
		CueDirectionList.push_back(tempInt);
		
		in1>>tempInt; // trial type
		TrialTypeList.push_back(tempInt);
		
	    in1>>tempInt; // timing
		CueTargetTimingList.push_back(tempInt);

	}

}
///////////////////////////////////////////////////////////////////////////////////
void ExperimentBody::loadProgress()
{
	//read in progress from [SubjectName]Progress.txt
	ostringstream fstr1;
	fstr1<< "Data\\"<<m_paramsFile->getString(CFG_SUBJECT_NAME)<<"\\"<<m_paramsFile->getString(CFG_SUBJECT_NAME) <<"Progress.txt";
	
	string tempStr1;
	string discard;
	int tempInt1, tempInt2;
	ifstream in1(fstr1.str().c_str()); 

	if(!in1.is_open())
	{
		CEnvironment::Instance()->outputMessage(CEnvironment::ENV_MSG_ERROR, "Progress data file (%sProgress.txt) could not be opened for writing.  Please check that the file exists.", m_paramsFile->getString(CFG_SUBJECT_NAME));
		declareFinished();

	}

	bool empty = true;

	while(in1.peek() != EOF)
	{
		in1>>discard;//discard date line
		in1>>tempInt1;
		in1>>tempInt2;

		empty = false;

	}

	if(empty == false)
	{
		m_sessionBegin = tempInt2+1;
		m_sessionEnd = tempInt2+1;
		m_curStim = tempInt2+1;

	}
	else
	{
		m_sessionBegin = 0;
		m_sessionEnd = 0;
		m_curStim = 0;

	}

}