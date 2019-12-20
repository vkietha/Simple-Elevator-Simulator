/******************************************************************************
Elevator Pick-UP Algorithm
by Keith Ha 
Started on: July 29, 2019
Last Updated: Wed, Dec 19, 2019
*******************************************************************************/

// READ ME //
/**************************************************************************************************************************
  This source code contains 3 sections contanting the functions, and the main
The program implements an elevator by splitting the system into two parts:
- The ToDo List controlling the elvator's path, telling how the elevator goes to pick up and drop off passengers
- The elevator itself moving in the path arranged by the ToDo List.

A "building" with a number of floors is generated
Each floor is a structure that contains a passenger
A passenger is a structure that contains information of their desination floor.
The floors are put into a static array of structures that contain the passenger structures

The ToDo List can access the array of floors
It accesses each floor to check each passenger's information of their destination floors
Then it inserts the information of the floor where the passenger is, and the destination floor
The algorithm also allows the ToDo List to sort the floor number while doing the insertion
The ToDo List is implemented as a Linked List data structure in order to make the insertion more efficient
more than if it was implemented as a static or even dynamic array.

After the ToDo List has done arranging the path,
based on the ToDo List, the elevator only goes to and stops at the floors in the ToDo List
Without the ToDo List, the elevator would have to each floor to check whether there is passenger to pick up or drop off

The elevator is implemented as a structure that contains a list of passengers
The list passengers inside the elevator is a Linked List
When the elevator reaches each floor based on the ToDo List, it checks its list of passenger contained
then removes the passengers that have reached their desination floor
The ToDo List also removes the floor from the list after finishing transporting the passenger
then it picks up the passenger from that floor
The Linked List of passenger makes this process of adding and removing passengers more efficient.

The ToDo List, as an electronic/software component, can easily access each floor to check for passengers
The elevator itself, as a mechanical component, stopping at each floor to check for passengers would be very inefficient
Therefore, in this algorithm, the elevator only stops at the floors in the path pre-determined by the ToDo List

In terms of GUI, the passenger is represented by the "*(destination floor number)"
srand() is put in main() so that the program generates new random passengers every time it runs
//*************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_FLOOR 1
#define MAX_FLOOR 10
#define MAX_NUM_FLOORS 32

#define MAX_NUM_PAS 20
#define NUM_PASS 5

#define UP 1
#define DOWN 0

//////// ALL DATATYPES ////////
//typedef enum directions {DOWN, UP} Dir;
//typedef enum flags {starting, destination} FlagType;

typedef struct passengerS{
	int dest;
	int dir;
	struct passengerS* next;
} Passenger;

typedef struct nodeS {
   int data;
   struct nodeS* next;
} Node;

typedef struct floorS{
	int num;
	int numPas;
	Passenger* pas;
} Floor;

typedef struct elevatorS{
	int cur;
	int dir;
	Passenger* pas;
} Elevator;

//*******************************************************************************************************
//                                         ALL FUNCTIONS
//*******************************************************************************************************

//--------------------------------------------------------------------------------------------------------
//                          1. INITILALIZING/ SET UP
//--------------------------------------------------------------------------------------------------------

int rb (int min, int max){
	return rand() % (max-min+1) + min;
}

Floor* mallocFloors (int num){
	Floor* floors = (Floor*) malloc (num*sizeof(Floor));
	
	for (int i = 0; i < num; i++){
		floors[i].num = i+1;
		floors[i].numPas = 0;
		floors[i].pas = (Passenger*) malloc(sizeof(Passenger));
	}
	
	return floors;
}

Elevator* mallocElevator (){
	Elevator* elvt = (Elevator*) malloc(sizeof(Elevator));
	elvt->cur = MIN_FLOOR;
	elvt->dir = UP;
	elvt->pas = NULL;
	
	return elvt;
}

void setPas (Floor* floors, int numFloors){
	floors[0].numPas = 1;
	floors[0].pas->dir = UP;
	floors[0].pas->dest = rb(MIN_FLOOR+1, MAX_FLOOR);
	//just to make sure that the first floor always has a passenger, for now
	
	for (int i = 1; i < numFloors; i++){
		floors[i].numPas = rb(0,1);
		if (floors[i].numPas == 1){
			if (floors[i].num == MAX_FLOOR) 
				floors[i].pas->dir = DOWN;
			else if (floors[i].num == MIN_FLOOR) 
				floors[i].pas->dir = UP;
			else
				floors[i].pas->dir = rb(0,1);
	
			//=> just to avoid person at MAX_FLOOR still wants to go UP
			//	or person at MIN_FLOOR still wants to go DOWN
	
			if (floors[i].pas->dir == UP){
				floors[i].pas->dest = rb(floors[i].num+1, MAX_FLOOR);
			}
			else if (floors[i].pas->dir == DOWN){
				floors[i].pas->dest = rb(MIN_FLOOR, floors[i].num-1);
			}
			
			floors[i].pas->next = NULL;
		}
		else if (floors[i].numPas == 0){
			floors[i].pas->dir = -1;
			floors[i].pas->dest = 0;
			//floors[i].pas->next = NULL;
			//so that these information don't interfere with the existing passengers
		}
		
	}
	
}


//--------------------------------------------------------------------------------------------------------
//                                 2. LINKED LIST OPERRATIONS
//--------------------------------------------------------------------------------------------------------

Node* insertFirst (int data, const Node* head){
	Node* newNode = (Node*) malloc (sizeof(Node));
	newNode->data = data;
	newNode->next = (Node*) head;
	
	return newNode;
}

Node* insertToDo (int toInsert, int dir, Node* head){
	Node* cur = head;
	
	while (cur->next != NULL){		// no traversal if it's at the last node, if cur->next == NULL then cur->next->data == NULL->data, which does not make sense
		if (dir == UP){
			if (cur->data < toInsert && toInsert <= cur->next->data) break;		// if right place then break
		}	
		else if (dir == DOWN){
			if (cur->data > toInsert && toInsert >= cur->next->data) break;
		}	
		
		cur = cur->next;
	}
	
	if (cur != NULL) cur->next = insertFirst (toInsert, cur->next);
	
	return head;
}

Node* delToDo (Node* head){		// for Node* todo, I only want to remove the first node, which is the head
	//Node* cur = head;
	//Node* prev = NULL;
	Node* toFree = NULL;
	
	if (head != NULL){
		toFree = head;
		head = head->next;
		free(toFree);
	}
	return head;
}

Passenger* insertFirstPas (Passenger* data, const Passenger* head){
	Passenger* newNode = (Passenger*) malloc (sizeof(Passenger));
	newNode->dest = data->dest;
	newNode->dir = data->dir;			//dest, dir are the data of type Passenger
	newNode->next = (Passenger*) head;
	
	return newNode;
}

Passenger* delPas (int toDel, Passenger* head){
	Passenger* cur = head;
	Passenger* prev = NULL;
	Passenger* toFree = NULL;
	if (head != NULL){		//check if list is empty, then only do if list is not empty
		if (head->dest == toDel){			//delete head
			toFree = head;
			head = head->next;
			free(toFree);
			return head;
		}
		else {
			while (cur != NULL && cur->dest != toDel){		//traversal
				prev = cur;
				cur = cur->next;
			}
			
			if (cur != NULL){
				toFree = cur;
				prev->next = cur->next;
				free(toFree);
			}
		}
	}
	return head;
}

void printList (const Node* head){
	Node* temp = (Node*) head;
	while (temp != NULL){
		printf ("%i ---> ", temp->data);
		temp = temp->next;
	}
	printf ("NULL\n");
}

void printPas (const Passenger* head){
	//printf ("<<< ");
	Passenger* temp = (Passenger*) head;
	while (temp != NULL){
		printf ("*%i ", temp->dest);
		temp = temp->next;
	}
}

//-----------------------------------------------------------------------------------------------------
//                                     3. PRINT FUNCTIONS
//-----------------------------------------------------------------------------------------------------
void printFloors (Floor* floors, int numFloors){
	for (int i = 0; i < numFloors; i++){
		printf ("%i    ", floors[i].num);
		if (floors[i].numPas != 0) printf ("*%i ", floors[i].pas->dest);
		printf ("\n");
	}
}

void draw (const Floor* floors, int numFloors, const Elevator* elvt){
	for (int i = 0; i < numFloors; i++){
		printf ("%i    ", floors[i].num);
		if (floors[i].numPas != 0) printf ("*%i ", floors[i].pas->dest);
		if (elvt->cur == floors[i].num) {
			printf ("<<< ");
			printPas (elvt->pas);
		}
		printf ("\n");
	}
}


//-------------------------------------------------------------------------------------------------------------------
//                                           MAIN
//-------------------------------------------------------------------------------------------------------------------

int main (){
	srand(time(NULL));
	int numFloors = MAX_FLOOR-MIN_FLOOR+1;
	Floor* floors = mallocFloors (numFloors);
	
	setPas(floors, numFloors);
	printFloors (floors, numFloors);
	
	Elevator* elvt = mallocElevator();			
	
	/////////////////////////////////////// Processing ToDo List /////////////////////////////////////////////////
	Node* todo = NULL;
	todo = insertFirst (floors[0].num, todo);		//putting the passenger on 1st floor to todo list
	todo = insertToDo (floors[0].pas->dest, UP, todo);
	
	for (int i = 1; i < numFloors; i++){
		int goingUp = floors[i].pas->dir == UP;		// if the floor[i] does not have a passenger, floors[i].pas->dir == -1
		
		if (goingUp){
			todo = insertToDo (floors[i].num, UP, todo);
			todo = insertToDo (floors[i].pas->dest, UP, todo);
		}
		printList(todo);
	}
	
	Node* rear = todo;		// pointer (Node* rear) points to the last node of "todo"
	while (rear->next != NULL){
		rear = rear->next;
	}
	
	printList (rear);
	
	for (int i = 1; i < numFloors; i++){
		int goingDown = floors[i].pas->dir == DOWN;
		
		if (goingDown){
			rear = insertToDo (floors[i].num, DOWN, rear);
			rear = insertToDo (floors[i].pas->dest, DOWN, rear);
		}
	}
	
	printList(rear);
	printList(todo);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	system ("pause");
	
	/////////////////////////////////////// ELEVATOR OPERATIONS /////////////////////////////////////
	while (todo != NULL){
		system("cls");
		draw(floors, numFloors, elvt);
		while (todo != NULL && elvt->cur == todo->data){
			system ("pause");
			// Removing the passengers that have reached their floors //
			if (elvt->pas != NULL){
				Passenger* temp = elvt->pas;
				while (temp != NULL){
					if (temp->dest == elvt->cur) {
						elvt->pas = delPas(elvt->cur, elvt->pas);
						break;
					}
					temp = temp->next;
				}
			}
			//------------------------------------------------------------
			
			// if that floor has a passenger, pick up the passenger
			if (floors[elvt->cur-1].numPas != 0){
				elvt->pas = insertFirstPas(floors[elvt->cur-1].pas, elvt->pas);
				
				// Removing the passenger from the floor
				floors[elvt->cur-1].numPas = 0;
				floors[elvt->cur-1].pas = delPas (floors[elvt->cur-1].pas->dest, floors[elvt->cur-1].pas);
			}
			//-----------------------------------------------------------------
			
			todo = delToDo (todo);		// Removing the floor from the list after the transportation
			
			// Redraw after processing the elevator's passengers //
			system ("cls");
			draw (floors, numFloors, elvt);
		}
		if (elvt->cur == MIN_FLOOR) elvt->dir = UP;
		else if (elvt->cur == MAX_FLOOR) elvt->dir = DOWN;
		
		if (elvt->dir == UP) (elvt->cur)++;
		else if (elvt->dir == DOWN) (elvt->cur)--;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Not even neccessary to free anything else,
	// because the deletion of nodes in the ToDo List and the passenger lists already free() the unused memory
	//free(floors);
	//free(elvt);
	//free(todo);
	return 0;
}

