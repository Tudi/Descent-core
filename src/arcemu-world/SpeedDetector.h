#ifndef _SPEED_DETECTOR_H_
#define _SPEED_DETECTOR_H_

// problem : some players advance more then others by using speed cheat
// solution : check the amount of advance a player does over time to see if it is higher then should be
// situations :
//		- player playes with mouse triggering a setfacing packet every 16 ms would flood us with incorrect values
//		- client sends us every 500 ms a movement update
//		- server makes speed changes -> need to reset detector
//		- server uses scripts over charater -> delay detector
// error recovery :
//		- speed change is detected then system resets state to start mesuring again
//		- speed is mesured over interval to avoid small value problems
//		- cheat detection has to stack up to trigger real cheat alarm
// problems :
//		- it takes SPDT_WINDOW_SIZE*SPDT_SAMPLINGRATE at each speed change until it gives relevant data
//		- if you choose a small sampling rate then float errors might stack up and precision is lost
//		- if you choose a large sampling rate then distance will include 'Z' changes and simple orientation changes..stuff that change the actuall distance
//		- moving in a small area is not considered cheating (it is hard to make that)

class Player;

class SpeedCheatDetector
{
	public:
		SpeedCheatDetector();
//		void		AddSample(float x, float y, int server_stamp, int client_stamp, float player_speed);			// update the detector with new values
		void		AddSample(float x, float y, float z, int client_stamp, float player_speed, int latency);			// update the detector with new values
		char		IsCheatDetected();													// test cheater status
		void		SkipSamplingFor(int ms_delay);										// delay then reset cheat detector
		void		ReportCheater(Player *_player);										// take actions agains a cheater
		void		EventSpeedChange();													// reset internal values on speed change
		void		CheckTeleportHack();												// if current possition is very large compared to old one then it is a hack

		float			rot_x,rot_y,rot_z;		// valid if previous movement was rotation
		float			rot_xp,rot_yp,rot_zp;	// valid if previous movement was rotation
		signed char		rot_seq;				// start right, stop, start left, stop
	private:
		float			last_x,last_y;				// we generate speed based on position diff
		float			tele_x,tele_y,tele_z;		// we generate speed based on position diff
//		int				last_stamp;					// stamp that we got from client last time
		int				last_server_stamp;			// stamp we received packet last time. We use this to see if packet timestamp is getting hacked
		signed char		cheat_threat;				// don't draw quick conclusions. If player is suspicios over time then kill him
		float			last_used_speed;			// we reset if speed changed since our last measure
		float			bigest_hacked_speed_dif;
		signed char		is_tel_hack_trigger;
		signed char		continues_speed_overflow;
};

#endif
