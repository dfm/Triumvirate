#ifndef TRIUM_COMMON_H_INCLUDED_
#define TRIUM_COMMON_H_INCLUDED_

/// Initialise process trackers.
int thisTask = 0;  // ???: current task
int numTasks = 1;  // ???: total number of tasks

double bytes = 0;  ///< memory usage in bytes

double timeStart;  ///< process start time
double durationInSec;  ///< process duration in seconds

/// Initialise line-of-sight struct.
struct losStruct {
	double pos[3];
};

#endif
