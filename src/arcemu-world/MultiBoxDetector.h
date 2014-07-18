#ifndef _MULTIBOX_DETECTOR_H_
#define _MULTIBOX_DETECTOR_H_

/*
From same IP multiple characters login.
There is a main character and others replicate it's actions.
Smart bots do not replicate always and exactly the same way the actions. You can mix multiple char types
The only similarity that can be used is that they will be near each other most of the time

This class is designed to be attached to a map to monitor players on it
Players are grouped by IP and we will monitor the distance between them
If players are moving in the same IP group but the distance is kept at a fair similar distance then we consider them multi boxing

Later it might be required to load an exeption list for some IPs. 
Note that this does not mean that there will be no multi boxing. This simply greatly reduces the number of the bots that may be used
*/

#define MB_CHECK_INTERVAL			5000			//every x MS server will check if map has multiboxing players on it
#define MB_PROXIMITY_REQUIRED_SQ	30*30			//x yards near a player with same IP probably means using bots
#define MB_MAX_PROXIMITY_CHECKS		5				//if player is still near the other player with same IP then it is strange and we can trigger alarm

class Player;

//list of players that over time share our filtering result
struct MultiBoxGroupStateStorage
{
	uint32					m_TimesInvestigated;		// to know when we can trigger alarm for this group
	std::list< Player* >	m_InvestigatedPlayers;		// to know who to boot
	uint32					m_LastInvestigatingStamp;	// to know if we can release the query container
};

class MultiBoxDetector
{
	public:
		MultiBoxDetector();
		~MultiBoxDetector();
		void	StartCycle();			//make cleanups if required
		void	AddPlayer(Player *p);	//inspect this player if any actions are required to be made
		void	EndCycle();				//make cleanups if required 
	private:
		std::map< ULONG, uint32 >	m_IP_groups_buildup;					//see which IPs we need to investigate
		std::map< ULONG, MultiBoxGroupStateStorage * >	m_IP_groups_inspected;		//see which IPs we need to investigate
		std::list< Player * >		m_inspected_players;					//we considered these players are worth inspecting
		uint32						m_NextUpdateStamp;
};
#endif