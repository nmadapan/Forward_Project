#ifndef __TIMERECORDER__
#define __TIMERECORDER__

#include <time.h>
#include "Text2Speech.h"

/*usage
TimeRecorder time_obj;
boost::thread TimeRecorder_Thread (&TimeRecorder::runTimer, &time_obj);
*/

class TimeRecorder{
private:
	SAM sam;
	clock_t task_start_time;
	float task_duration;
	bool recording_task_time;
public:
	TimeRecorder(){
		recording_task_time = false;
		task_start_time = clock();
	}
	void runTimer ()
	{
		printf ("Time Recorder started to run! Press t to toggle the time recording!! \n");
		while (1)
		{
			if (GetAsyncKeyState('T') < 0)
			{
				if  (recording_task_time == false)
				{
					if ( (float(clock() - task_start_time)/CLOCKS_PER_SEC) > 2) // when key is pressed down, it's going to be detected multiple times. 
																				// So any consecutive key strokers (within 2 seconds, not very likely to happen) will be ignored.
					{
						recording_task_time = true;
						task_start_time = clock();
						sam.speak("task start");
					}
				}
				if  (recording_task_time == true)
				{
					if ( (float(clock() - task_start_time)/CLOCKS_PER_SEC) > 2) // when key is pressed down, it's going to be detected multiple times. 
																				// So any consecutive key strokers (within 2 seconds, not very likely to happen) will be ignored.
					{
						recording_task_time = false;
						task_duration = float(clock() - task_start_time)/CLOCKS_PER_SEC;
						printf ("Task duration (seconds): %f", task_duration);
						sam.speak("task end");
						task_start_time = clock(); // just for calculation of the time span in the (recording_task_time == false) case.
					}
				}
			}
		}

	}
};
#endif