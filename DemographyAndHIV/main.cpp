////////////////////////////////////////////////////////////////
//    Mikaela Epidemiology Model							  //
//    Created by Mikaela Smit on 7/11/2014.				      //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.   //
////////////////////////////////////////////////////////////////


#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include "person.h"
#include "event.h"
#include "eventQ.h"
#include "eventfunctions.h"
#include "errorcoutmacro.h"

#include <fstream>									// some important libraries for reading in the arrays
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;   

// TO FIX GENERAL
// 1. Change population to vector instead of array - need to keep a long long int for count of population, and find way of dealing with array of pointers (too long)
// 2. Remove all unecessary code - tidy up
// 3. When person dies, remove all related events, do something about events that should no longer happen
// 4. Make various event types (people-specific/annual events,...)
// 5. Use 'const' in model to stop model from changing variables thta it shouldn't change
// 9. Add class destructors
// 9. Make C++ run with Matlab ? THIS IS TAKING A WHILE AND I AM STILL BUSY WITH THIS.  I am going to look at SJ?s code for this
// 10. Private versus public
// 11. Think about making your own priortiy queue 
// 12. Think about how to organise this program
// 13. Think about a parameter document - transfer HIV parameter to seperate file



// TO DO List for HIV:
// 1. Add HIV status using incidence
// 2. Add HIV natural history (CD4 count)
// 3. Change mortality once they have HIV and dependant on CD4 coutn
// 4. Add more aspects of HIV care cascade (diagnosis, start cART,?)
// 5. Update HIV knowledge based on HIV test


//// --- Function related to events - to be available externally ---
double *p_GT;																// Pointer to global time
int *p_PY;																	// Pointer to show which year range we are on
int PY=0;																	// Set the first pointer to year range reference to 0
double StartYear=1950;														// Define Start Year if the model and set it to year of choice
int EndYear=2010;															// If endyear is more than 2010, some things will need to get changes, an error message below has been set up as reminder

// Some fertility parameters
float *p_BF;																// Pointer to delay to next pregancy with breastfeeding duration set below
float breastfeeding=0;														// Can change this globally here, for example to 3 months = 0.25
float pregancy=0.75;														// Duration of average pregancy is around 9 months

// Some HIV parameters
int *p_TI;																	// Pointer to tretament initiation
int ART_start_year=2004;

// Some population parameter
const long long int final_number_people=100000000;							// To determine the final size of the total population to be modeled
int init_pop =59100;															// Initial population 1st Jan 1950 as 5910 (see Excel for calculation)
int total_population=init_pop;												// Update total population for output and for next new entry

priority_queue<event*, vector<event*>, timeComparison> *p_PQ;				// Pointer to event queue so as to be able to push-in/pop-out new events that are ocurreing  as a result of 'primary' events in the queue, e.g. recurrent birthdays
person** MyArrayOfPointersToPeople = new person*[final_number_people];		// First 'person*' is a pointer (address) and 'new person' and space for x person which will point to actual person below
																			// Have to now change [init_pop] to [final_number_people] to give the final size of 'matrix'
vector<event *> Events;


//// --- RUN THE MAIN MODEL ---
int main(){

cout << "Hello, Mikaela!" << endl << endl ;								// Check if model is running
	
	
	//// Load relevant parameters
	cout << "We got to section 1 - We are loading the arrays" << endl;
	loadBirthArray();
	loadDeathArray_Women();
	loadDeathArray_Men();
	loadHIVArray_Women();
	loadHIVArray_Men();
	

	//// Some notification code
	E(cout << "NOTE: The Macro for error code is working" << endl << endl;)
	if (StartYear > 1954){cout << "NOTE: Remeber that fertility at the beginning is based on a TRF_init of 0, based on GlobalTime being between 1950 and 1954" << endl << endl;}
	if (EndYear > 2010){cout << "NOTE: The model runs after 2010 and output things need to be updated to avoid errors" << endl << endl;}

	
	//// --- Some pointers ---
	double GlobalTime=StartYear;											// Define Global Time and set it to 0 at the beginning of the model
	p_GT=&GlobalTime;														// Define the location the pointer to Global time is pointing to
	srand(time(NULL));														// Random Number generator using PC time
	priority_queue<event*, vector<event*>, timeComparison> iQ;				// Define th ePriority Q
	p_PQ=&iQ;																// Define pointer to event Q
	p_PY=&PY;
	float timetonextbaby=pregancy+breastfeeding;
	p_BF=&timetonextbaby;
	p_TI=&ART_start_year;													// Define pointer to tretament initiation (as 2004 e.g.)
	
	
	//// --- Making Population---
	cout << "We got to section 2 - We are going to create a population" << endl;
	for(int i=0; i<total_population; i++){									// REMEMBER: this needs to stay "final_number_people" or it will give error with CSV FILES!!!!
		MyArrayOfPointersToPeople[i]=new person();							// The 'new person' the actual new person
		int a=i;
		(MyArrayOfPointersToPeople[i])->PersonIDAssign(a);					// --- Assign PersonID ---
	}
	
	for(int i=0; i<total_population; i++){
		(MyArrayOfPointersToPeople[i])->Alive=1;							// --- Assign Life Status ---

		(MyArrayOfPointersToPeople[i])->GenderDistribution();				// --- Assign Sex- ---

		(MyArrayOfPointersToPeople[i])->GetMyDoB();							// --- Assign DoB/Age ---

		(MyArrayOfPointersToPeople[i])->GetDateOfDeath();					// --- Assign date of death --- 

		if (MyArrayOfPointersToPeople[i]->Sex == 2 && MyArrayOfPointersToPeople[i]->Age<50 && MyArrayOfPointersToPeople[i]->AgeAtDeath>=15) {(MyArrayOfPointersToPeople[i])->GetDateOfBaby();}		// --- Assign Birth of all Children- ---

		(MyArrayOfPointersToPeople[i])->GetMyDateOfHIVInfection();
	}							
	cout << "We got to section 2 - We finished crating a population" << endl;
	


	//// --- EVENTQ ---
	cout << "We got to section 3 - We are going to create the annual events" << endl;
		
	// Lets feed in calendar update
	event * TellNewYear = new event;										// --- Tell me every time  a new year start ---
	Events.push_back(TellNewYear);
	TellNewYear->time = StartYear;																
	TellNewYear->p_fun = &EventTellNewYear;
	iQ.push(TellNewYear);

	
	//// --- GIVE OUTPUT OF QUEUE AS IT HAPPENS --- ////
	cout << endl << endl << "The characteristics of the event queue:" << endl;
	cout << "the first event will ocurr in " << iQ.top()->time << ".  " << endl;
	cout << "the size of the event queue is " << iQ.size() << endl;
	
	while(iQ.top()->time< EndYear +1 /*|| !iQ.empty()*/){					// this loop throws up error because no recurrent birthday pushing gt over 5 yrs and iq.pop means gt cannot be updated after pop
		GlobalTime=iQ.top()->time;											// careful with order of global time update - do not touch or touch and check!!		
		iQ.top()-> p_fun(iQ.top()->person_ID);
		iQ.pop();		
	} 
	
	//// --- Output the results in a csv file ---
	FILE* csv_out = fopen("test.csv","w");
	for (int i=0; i<total_population; i++) {								// Change the i< X here as well as the "%d!!
		fprintf(csv_out,"%d,%d,%f,%f,%d,%d, %f, %d, %f, %d, %d, %f \n",
			MyArrayOfPointersToPeople[i]->PersonID,
			MyArrayOfPointersToPeople[i]->Sex,
			MyArrayOfPointersToPeople[i]->DoB,
			MyArrayOfPointersToPeople[i]->Age,
			MyArrayOfPointersToPeople[i]->MotherID,
			MyArrayOfPointersToPeople[i]->DatesBirth.size(),
			MyArrayOfPointersToPeople[i]->DateOfDeath,
			MyArrayOfPointersToPeople[i]->AgeAtDeath,
			MyArrayOfPointersToPeople[i]->HIV,
			MyArrayOfPointersToPeople[i]->CD4_cat,
			MyArrayOfPointersToPeople[i]->ART,
			MyArrayOfPointersToPeople[i]->ART_T0

			);}
	fclose(csv_out);


	//// --- LETS AVOID MEMORY LEAKS AND DELETE ALL NEW EVENTS --- ////
	cout << "Lets delete the heap! " << endl;
	for(int i=0; i<Events.size()-1; i++){
		E(cout << "Event " << Events.size() << " is " << Events.at(i)->time << endl;)
		delete Events.at(i);
		E(cout << "Event " << Events.size() << " is " << Events.at(i)->time << endl;)
	}

	for(int i=0; i<total_population; i++){									// REMEMBER: this needs to stay "final_number_people" or it will give error with CSV FILES!!!!
		delete MyArrayOfPointersToPeople[i];								// The 'new person' the actual new person
	}

	// --- End of code ---
    cout << endl << "Hi Jack, so sorry\n";
	system("pause");
    return 0;
}