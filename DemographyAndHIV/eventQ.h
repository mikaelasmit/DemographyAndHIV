////////////////////////////////////////////////////////////////////////////////////
//    Created by Mikaela Smit on 30/10/2014.									  //
//    This script is to make the EventQ itself.									  //
//    N.B. can add personalised 'push' 'pop' etc here as Jack did if necessary.   //
////////////////////////////////////////////////////////////////////////////////////



#include <stdio.h>							// Refers to C StandarD Input Output library - uses streams to operate with physical devices including keyboards
#include <queue>							// Needed for the priorityQ
#ifndef eventQ_h							
#define eventQ_h
using namespace std;


//// --- DEFINE THE COMPARISON CLASS AND DEFINE OPERATOR --- ////
struct timeComparison {bool operator()(const event *lhs, const event *rhs) const;};	


//// --- CREATING EVENTQ --- ////
class eventQ{
public:

	eventQ();								// Constructor

	};

#endif	




