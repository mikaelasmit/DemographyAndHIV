////////////////////////////////////////////////////////////////////////////////////
//    Created by Mikaela Smit on 30/10/2014.									  //
//    This script is to make the EventQ itself.       							  //
//    N.B. can add personalised 'push' 'pop' etc here as Jack did if necessary.   //
////////////////////////////////////////////////////////////////////////////////////


#include "event.h"
#include "eventQ.h"


//// --- OUTSIDE INFORMATION --- ////
bool timeComparison::operator()(const event *lhs, const event *rhs) const{
	return lhs->time > rhs->time;}
 	
//// --- CLASS (EVENTQ) CONSTRUCTOR --- ////
eventQ::eventQ()
{} 


