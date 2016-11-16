/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/** 
 * @version 1.0
 */

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

/******************************************************************************
 ****************************** INLINE DEFINITION *****************************
 ******************************************************************************/

/******************************************************************************
 * This method create and initialize a new Event object.
 *
 * @return the created event
 ******************************************************************************/
inline PerformanceTimer::Event PerformanceTimer::createEvent() const
{
    PerformanceTimer::Event evt;

	getHighResolutionTime( &evt.timerStartEvt );
	getHighResolutionTime( &evt.timerStopEvt );

	return evt;
}

/******************************************************************************
 * This method set the start time of the given event to the current time.
 *
 * @param evt a reference to the event.
 ******************************************************************************/
inline void PerformanceTimer::startEvent( PerformanceTimer::Event& pEvent ) const
{
	getHighResolutionTime( &pEvent.timerStartEvt );
}

/******************************************************************************
 * This method set the stop time of the given event to the current time.
 *
 * @param evt a reference to the event.
 ******************************************************************************/
inline void PerformanceTimer::stopEvent( PerformanceTimer::Event& pEvent ) const
{
	getHighResolutionTime( &pEvent.timerStopEvt );
}

/******************************************************************************
 * This method return the duration of the given event
 *
 * @param evt a reference to the event.
 *
 * @return the duration of the event in milliseconds
 ******************************************************************************/
inline float PerformanceTimer::getEventDuration( PerformanceTimer::Event& pEvent ) const
{
	float tms;

	/*tms = ( pEvent.timerStopEvt.tv_sec - pEvent.timerStartEvt.tv_sec ) * 1000.0f // sec -> msec
	+ ( pEvent.timerStopEvt.tv_nsec - pEvent.timerStartEvt.tv_nsec ) * 1e-6f;  // nano -> milli*/
	tms = convertTimeDifferenceToSec( &pEvent.timerStopEvt, &pEvent.timerStartEvt ) * 1000.0f;

	return tms;
}

/******************************************************************************
 * This method return the difference between the begin of two events
 *
 * @param evt0 a reference to the first event
 * @param evt1 a reference to the second event
 *
 * @return the difference between the two events in milliseconds
 ******************************************************************************/
inline float PerformanceTimer::getStartToStartTime( PerformanceTimer::Event& pEvent0, PerformanceTimer::Event& pEvent1 ) const
{
	float tms;

	/*tms = ( pEvent1.timerStartEvt.tv_sec - pEvent0.timerStartEvt.tv_sec ) * 1000.0f // sec -> msec
	+ ( pEvent1.timerStartEvt.tv_nsec - pEvent0.timerStartEvt.tv_nsec ) * 1e-6f;  // nano -> milli*/

	tms = convertTimeDifferenceToSec( &pEvent1.timerStartEvt, &pEvent0.timerStartEvt ) * 1000.0f;

	return tms;
}

/******************************************************************************
 * Get high resolution time
 *
 * @param pPerformanceCount ...
 ******************************************************************************/
inline void PerformanceTimer::getHighResolutionTime( PerformanceTimer::timerStruct* pPerformanceCount ) const
{
#ifdef WIN32
	// Retrieves the current value of the high-resolution performance counter
	// - parameter :
	// ---- A pointer to a variable that receives the current performance-counter value, in counts.
	QueryPerformanceCounter( pPerformanceCount );
#else
	clock_gettime( CLOCK_REALTIME, pPerformanceCount );
#endif
}

/******************************************************************************
 * Convert time difference to sec
 *
 * @param end ...
 * @param begin ...
 *
 * @return ...
 ******************************************************************************/
inline float PerformanceTimer::convertTimeDifferenceToSec( PerformanceTimer::timerStruct* pEnd, PerformanceTimer::timerStruct* pBegin ) const
{
#ifdef WIN32
	timerStruct frequency;
	// Retrieves the frequency of the high-resolution performance counter, if one exists. The frequency cannot change while the system is running.
	// - parameter :
	// ---- A pointer to a variable that receives the current performance-counter frequency, in counts per second.
	// ---- If the installed hardware does not support a high-resolution performance counter, this parameter can be zero.
	QueryPerformanceFrequency( &frequency );

	return ( pEnd->QuadPart - pBegin->QuadPart ) / static_cast< float >( frequency.QuadPart );
#else
	return ( pEnd->tv_sec - pBegin->tv_sec ) + ( 1e-9 ) * ( pEnd->tv_nsec - pBegin->tv_nsec );
#endif
}
