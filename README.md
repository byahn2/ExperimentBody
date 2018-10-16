# ExperimentBody
This is the presentation code for the replication study we are completing for BCS 206


What we added in ExperimentBody.h file:

int: LGapDirection, RGapDirection, GapSize, answerDirection//the direction button subject pressed

vector<int>: LGapDirectionList, RGapDirectionList, GapSizeList
  
float: YTargetOff//the vertical distance 

CSolidPlane: m_LTarget, m_RTarget//we need 2 target

bool: isCorrect//whether the answer is correct, isAnswered//whether subject pressed button in certain time limit.
