///////////////////////////////////////////////////////////////////////////
//    Created by Mikaela Smit on 07/11/2014.							 //
//    This script provides properties of 'events' added to the EventQ.   // 
///////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "person.h"										// Need to include person header as we make pointer to persons in below
#ifndef event_h										
#define event_h	
using namespace std;


//// --- EVENT Q --- ////

class event {
public:

	event();											// Class constructor
 	
	double time;	

 	void (*p_fun)(person *MyPointerToPerson);			// Function pointer - when an event is called in the Q it can point to what functions need to be executed e.g. If person gets pos HIVTest tell him to schedule GP appointment
														// [...] the bit inside the brackets "(person *MyPointerToPerson) means function can carry argument which is pointer 'pointer' to class 'person'.  
	person * person_ID;		 							
														// This means that there is a pointer called 'person_ID' which is pointing to class 'person'
	};													// [...] allows to make sure all events, including new ones, point to the correct person to whom this event is occurring.  
	
#endif													





