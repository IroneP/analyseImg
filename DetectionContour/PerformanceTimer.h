/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/** 
 * @version 1.0
 */

#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// System
#ifdef WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#else
	#include <time.h>
#endif

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

/** 
 * @class PerformanceTimer
 *
 * @brief The PerformanceTimer class provides a host performance timer.
 *
 * Allows timing CPU events.
 */
class PerformanceTimer
{

	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/

public:

	/****************************** INNER TYPES *******************************/

	/**
	 * Type definition of types required by timer functions (Operating System dependent)
	 */
#ifdef WIN32
		typedef LARGE_INTEGER timerStruct;
#else
		typedef struct timespec timerStruct;
#endif

	/**
	 * Structure used to store start end stop time of an event
	 */
	struct Event
	{
		timerStruct timerStartEvt;
		timerStruct timerStopEvt;
	};

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 */
    PerformanceTimer();

	/**
	 * Destructor
	 */
    virtual ~PerformanceTimer();

	/**
	 * This method create and initialize a new Event object.
	 *
	 * @return the created event
	 */
	inline Event createEvent() const;

	/**
	 * This method set the start time of the given event to the current time.
	 *
	 * @param evt a reference to the event.
	 */
	inline void startEvent( Event& pEvent ) const;

	/**
	 * This method set the stop time of the given event to the current time.
	 *
	 * @param evt a reference to the event.
	 */
	inline void stopEvent( Event& pEvent ) const;

	/**
	 * This method return the duration of the given event
	 *
	 * @param evt a reference to the event.
	 *
	 * @return the duration of the event in milliseconds
	 */
	inline float getEventDuration( Event& pEvent )  const;

	/**
	 * This method return the difference between the begin of two events
	 *
	 * @param evt0 a reference to the first event
	 * @param evt1 a reference to the second event
	 *
	 * @return the difference between the two events in milliseconds
	 */
	inline float getStartToStartTime( Event& pEvent0, Event& pEvent1 ) const;
		
	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/**
	 * Get high resolution time
	 *
	 * @param pPerformanceCount ...
	 */
	inline void getHighResolutionTime( timerStruct* pPerformanceCount ) const;

	/**
	 * Convert time difference to sec
	 *
	 * @param end ...
	 * @param begin ...
	 *
	 * @return ...
	 */
	inline float convertTimeDifferenceToSec( timerStruct* pEnd, timerStruct* pBegin ) const;

	/******************************** METHODS *********************************/

	/**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**
	 * Copy constructor forbidden.
	 */
    PerformanceTimer( const PerformanceTimer& );

	/**
	 * Copy operator forbidden.
	 */
    PerformanceTimer& operator=( const PerformanceTimer& );

};

/**************************************************************************
 ***************************** INLINE SECTION *****************************
 **************************************************************************/

#include "PerformanceTimer.inl"

#endif // PERFORMANCE_TIMER_H
