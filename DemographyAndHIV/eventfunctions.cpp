/////////////////////////////////////////////////////////////////
//    Created by Mikaela Smit on 07/11/2014.				   //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.    //
//    These event are added to the EventQ.					   //
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include "eventfunctions.h"
#include "event.h"															// Need to add these to be able to	
#include "eventQ.h"															// [...]run Global Time and Recurrent events pointers
#include "person.h"
#include "errorcoutmacro.h"


using namespace std;


//// --- OUTSIDE INFORMATION --- ////
extern double *p_GT;														// Tell this .cpp that there is pointer to Global Time defined externally
extern int EndYear;															// Include endyear so that we can avoid putting unnecessary items into EventQ
extern priority_queue<event*, vector<event*>, timeComparison> *p_PQ;		// Tell this .cpp that there is a priorty_queue externall and define pointer to it
extern int total_population;												// Update total population for output and for next new entry
extern person** MyArrayOfPointersToPeople;									// Pointer to MyArrayOfPointersToPeople
extern int *p_PY;															// Pointer to show which year range we are on
extern vector<event*> Events;


//// --- Key parameters for HIV natural history, treatment and mortality --- ////
// The following arrays are for [Sex][Age], where Sex: 0-Men & 1=Women and Age: 0=0-24, 1=25-34, 2=35-44, and 3= over 45
float CD4_startarray[2][4][3]={												// Array for proportion with specific CD4 count ranges upon seroconversion (Men and women)
	{{0.57905, 0.83195, 1},		{0.53669, 0.79935, 1},		{0.49260, 0.76275, 1},		{0.42467, 0.70094, 1}},
	{{0.62750, 0.84933, 1},		{0.58635, 0.81863, 1},		{0.54277, 0.78372, 1},		{0.47411, 0.72378, 1}}
};
		
float ARTDeath_event[2][4][7]={												// Array to see if this event will be either ART start or death will orcurr in this CD4 cat - see Excel for calculation
	{{0.048, 0.066, 0.088, 0.100, 0.145, 0.089, 1},		{0.052, 0.072, 0.092, 0.105, 0.152, 0.098, 1},		{0.056, 0.079, 0.097, 0.111, 0.160, 0.108, 1},		{0.065, 0.095, 0.109, 0.124, 0.177, 0.130, 1}},
	{{0.052, 0.072, 0.095, 0.108, 0.156, 0.096, 1},		{0.056, 0.079, 0.100, 0.113, 0.164, 0.106, 1},		{0.061, 0.086, 0.106, 0.120, 0.172, 0.118, 1},		{0.072, 0.104, 0.119, 0.134, 0.191, 0.142, 1}}
};
	
float ART_start[2][4][7]={													// If number is smaller than that in array then patient starts ART if its bigger they will die
	{{0.678, 0.608, 0.773, 0.773, 0.778, 0.556, 0.042},		{0.627, 0.553, 0.730, 0.731, 0.736, 0.499, 0.034},		{0.576, 0.501, 0.687, 0.687, 0.693, 0.447, 0.027},		{0.487, 0.421, 0.605, 0.606, 0.612, 0.361, 0.019}},								
	{{0.670, 0.599, 0.766, 0.766, 0.771, 0.546, 0.040},		{0.618, 0.543, 0.723, 0.723, 0.729, 0.490, 0.032},		{0.567, 0.491, 0.679, 0.679, 0.685, 0.437, 0.026},		{0.478, 0.403, 0.596, 0.596, 0.603, 0.352, 0.019}}
};

float CD4_rate_change[2][4][7]={											// Time to progression to next CD4 stage
	{{4.96, 2.96, 2.91, 1.82, 1.68, 1.11, 1.18},	{4.94, 2.94, 2.89, 1.90, 1.67, 1.09, 0.94},		{4.92, 2.91, 2.88, 1.89, 1.65, 1.08, 0.76},		{4.87, 2.86, 2.84, 1.86, 1.61, 1.05, 0.53}},
	{{5.35, 3.18, 3.14, 2.06, 1.81, 1.19, 1.14},	{5.33, 3.16, 3.12, 2.05, 1.79, 1.17, 0.91},		{5.30, 3.13, 3.10, 2.03, 1.77, 1.16, 0.74},		{5.24, 3.07, 3.05, 2.00, 1.72, 1.13, 0.51}}
};


//////////////////////////////////////
//// --- FUNCTIONS FOR EVENTS --- ////
//////////////////////////////////////

//// --- NEW YEAR FUNCTION --- ////
void EventTellNewYear(person *MyPointerToPerson){				
																		
	cout << "A new year has started, it is now " << *p_GT << endl;				// This whole function to output every time a new year starts - to make sure the model is actually running
	
	// Lets get the pointer to the right year range
	if (*p_GT<1955){*p_PY = 0; };
	if (*p_GT >= 1955 && *p_GT<1960){*p_PY = 1; };
	if (*p_GT >= 1960 && *p_GT<1965){*p_PY = 2; };
	if (*p_GT >= 1965 && *p_GT<1970){*p_PY = 3; };
	if (*p_GT >= 1970 && *p_GT<1975){*p_PY = 4; };
	if (*p_GT >= 1975 && *p_GT<1980){*p_PY = 5; };
	if (*p_GT >= 1980 && *p_GT<1985){*p_PY = 6; };
	if (*p_GT >= 1985 && *p_GT<1990){*p_PY = 7; };
	if (*p_GT >= 1990 && *p_GT<1995){*p_PY = 8; };
	if (*p_GT >= 1995 && *p_GT<2000){*p_PY = 9; };
	if (*p_GT >= 2000 && *p_GT<2005){*p_PY = 10;};
	if (*p_GT >= 2005){*p_PY = 11; };
	E(cout << "Let's check relevant things have been updated... *p_PY: " << *p_PY << " and Global Time: " << *p_GT << endl;)

	// Schedule event for next year
	event * RecurrentTellNewYear = new event;
	Events.push_back(RecurrentTellNewYear);
	RecurrentTellNewYear->time = *p_GT + 1;										
	RecurrentTellNewYear->p_fun = &EventTellNewYear;
	p_PQ->push(RecurrentTellNewYear);

	E(cout << "We have finished telling you the new year and setting fertility variables for the year." << endl;)	// Error message - can be switched on/off
}


//// --- DEATH EVENT --- ////	
void EventMyDeathDate(person *MyPointerToPerson){

	if (MyPointerToPerson->Alive==1){MyPointerToPerson->Alive=0;}
	E(cout << "Person " << MyPointerToPerson->PersonID << " just died. Their life status now is: " << MyPointerToPerson->Alive << endl;)
}


//// --- BIRTH EVENT AND MAKING NEW PERSON --- ////
void EventBirth(person *MyPointerToPerson){								
		
	E(cout << "A birth is about to happen and my life status: " << endl;)		// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Only let woman give birth if she is still alive 
		
		total_population=total_population+1;									// Update total population for output and for next new entry
		MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get age at birth for output
		

		// Creating a new person 
		MyArrayOfPointersToPeople[total_population-1]=new person();
		(MyArrayOfPointersToPeople[total_population-1])->PersonIDAssign(total_population-1);
		(MyArrayOfPointersToPeople[total_population-1])->Alive=1;
		(MyArrayOfPointersToPeople[total_population-1])->GenderDistribution();
		(MyArrayOfPointersToPeople[total_population-1])->GetMyDoBNewEntry();
		(MyArrayOfPointersToPeople[total_population-1])->GetDateOfDeath();
		(MyArrayOfPointersToPeople[total_population-1])->GetDateOfBaby();
		(MyArrayOfPointersToPeople[total_population-1])->GetMyDateOfHIVInfection();

	
		// Link Mother and Child
		(MyArrayOfPointersToPeople[total_population-1])->MotherID=MyPointerToPerson->PersonID;			// Give child their mothers ID
		MyPointerToPerson->ChildIDVector.push_back((MyArrayOfPointersToPeople[total_population-1]));	// Give mothers their child's ID
			
		E(cout << "We have finished giving birth " << endl;)					// Error message - can be switched on/off
	
	}
}


//// --- HIV EVENT --- ////
void EventMyHIVInfection(person *MyPointerToPerson){
	
	E(cout << "Somebody is about to get infected with HIV: " << endl;)			// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Only execute this is patient is still alove
	
		MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below
	

		//// --- Get my CD4 count at start --- ////
		double	h = ((double)rand() / (RAND_MAX));								// Gets a random number between 0 and 1.
		int i=0;
		int j=0;
		int a=25;
	
		while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arrays
		while (h>CD4_startarray[MyPointerToPerson->Sex-1][i][j] && j<3){j++;}	// To get the corresponding CD4 count for correct age and sex from random 'h' generated above
	
		MyPointerToPerson->CD4_cat=0+j;											// CD4 count cat (variable over time)
		MyPointerToPerson->CD4_cat_start=0+j;									// CD4 count cat at start (to keep in records)

	
		//// --- Let's see if I will die or start ART in this CD4 stage ---- ////
		float e = ((double) rand() / (RAND_MAX));
		float ee = ((double) rand() / (RAND_MAX));

		if (e<=ARTDeath_event[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){
		
			if (ee<=ART_start[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){
				MyPointerToPerson->ART=1;
			}
		
			else if (ee>ART_start[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]) {
			
				float HIVdeath=0;
				HIVdeath=MyPointerToPerson->HIV+CD4_rate_change[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];

				if (HIVdeath<MyPointerToPerson->DateOfDeath){					// If HIV death is before natural death we need to bring death forward
					MyPointerToPerson->DateOfDeath=HIVdeath;						
					
					if (MyPointerToPerson->DateOfDeath<2011){
					event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
					Events.push_back(DeathEvent);
					DeathEvent->time = MyPointerToPerson->DateOfDeath;											
					DeathEvent->p_fun = &EventMyDeathDate;
					DeathEvent->person_ID = MyPointerToPerson;
					p_PQ->push(DeathEvent);
					}
				}
			}
		}


		//// --- If a patient doesn't die or start ART we need to make the next CD4 count progression!!!
		else if (e>ARTDeath_event[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){

			float TestNextCD4Change=-999;
			TestNextCD4Change=MyPointerToPerson->HIV+CD4_rate_change[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
	
			event * CD4change = new event;										// Lets feed next CD4 count change into into the eventQ									
			Events.push_back(CD4change);
			CD4change->time = TestNextCD4Change;													
			CD4change->p_fun = &EventCD4change;
			CD4change->person_ID = MyPointerToPerson;
			p_PQ->push(CD4change);
		}
	}

	E(cout << "Somebody has just been infected with HIV!" << endl;)				// Error message - can be switched on/off
}


void EventCD4change(person *MyPointerToPerson){
	
	E(cout << "Somebody is about to experience a drop in CD4 count: " << endl;)	// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Add any additional things to progression of CD4 count
	
		MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below


		//// --- Let's get the right index for all relevant arrays used here ---- ////
		MyPointerToPerson->CD4_cat=MyPointerToPerson->CD4_cat+1;
		double	h = ((double)rand() / (RAND_MAX));								// Gets a random number between 0 and 1.
		int i=0;
		int j=0;
		int a=25;
		while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arraysfloat e = ((double) rand() / (RAND_MAX));
	
	
		//// --- Let's see if I will die or start ART in this CD4 stage ---- ////
		float e = ((double) rand() / (RAND_MAX));
		float ee = ((double) rand() / (RAND_MAX));

		if (e<=ARTDeath_event[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){
		
			if (ee<=ART_start[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){
				MyPointerToPerson->ART=1;
			}
		
			else if (ee>ART_start[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]) {
			
				float HIVdeath=0;
				HIVdeath=*p_GT+CD4_rate_change[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];

				if (HIVdeath<MyPointerToPerson->DateOfDeath){					// If HIV death is before natural death we need to bring death forward
					MyPointerToPerson->DateOfDeath=HIVdeath;						
																		
					if (MyPointerToPerson->DateOfDeath<EndYear){
						event * DeathEvent = new event;							// In that case we need to add the new death date to the EventQ		
						Events.push_back(DeathEvent);
						DeathEvent->time = MyPointerToPerson->DateOfDeath;													
						DeathEvent->p_fun = &EventMyDeathDate;
						DeathEvent->person_ID = MyPointerToPerson;
						p_PQ->push(DeathEvent);
					}
				}
			}
		}	


		//// --- If a patient doesn't die or start ART we need to make the next CD4 count progression!!!
		else if (e>ARTDeath_event[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat]){

			float TestNextCD4Change=-999;
			TestNextCD4Change=*p_GT+CD4_rate_change[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
					
			event * CD4change = new event;										// Lets feed next CD4 count change into into the eventQ 								
			Events.push_back(CD4change);
			CD4change->time = TestNextCD4Change;													
			CD4change->p_fun = &EventCD4change;
			CD4change->person_ID = MyPointerToPerson;
			p_PQ->push(CD4change);
		}
	}
	
	E(cout << "Somebody has just experiences a drop in CD4 count!" << endl;)	// Error message - can be switched on/off
}
	








