//FETCHMASTER5000
//Nathan Leung, Benjamin Lo, Rares Gosman, Yifan Wu


//DRINK DATA
//Coke: Colour = Blue, number = 0
//Sprite: Colour = green, number = 1
//Fanta: Colour = yellow, number = 2
//Canada Dry = red, number = 3

//Two motors are used on a Lego car controlled by Bluetooth through a phone app


//FUNCTIONS FOR DISPLAYING A FACE ONSCREEN

void displayFace()
{
	eraseDisplay();
	nxtDisplayString (0,"  ___       ___   ");
	nxtDisplayString (1," | _ |     | _ |  ");
	nxtDisplayString (2,"| | | |   | | | | ");
	nxtDisplayString (3,"|_| |_|   |_| |_| ");
	nxtDisplayString (5,"      _____       ");
	nxtDisplayString (6,"     |_____|      ");
}

void displayFaceSleep()
{
	eraseDisplay();
	nxtDisplayString (1,"  /\        /\  ");
	nxtDisplayString (2," |  |      |  | ");
	nxtDisplayString (3," |  |      |  | ");

	nxtDisplayString (6,"     \____/    ");
}

void startup()
{
	displayFaceSleep();

	while(nNxtButtonPressed == -1);
	while(nNxtButtonPressed != -1);

	for (int i =0; i <3; i++) //blinking awake
	{
		displayFaceSleep();
		wait1Msec(250);
		displayFace();
		wait1Msec(250);
	}
	wait1Msec(2000);
	eraseDisplay();
	nxtDisplayString (2,"  Hello World!");
	nxtDisplayString (4,"    I am the");
	nxtDisplayString (5," FetchMaster5000");

	wait1Msec(2000);
}

void end()
{
	displayFace();

	for (int i =0; i <3; i++) //blinking asleep
	{
		displayFace();
		wait1Msec(250);
		displayFaceSleep();
		wait1Msec(250);
	}
	wait1Msec(1000);
}


//FUNCTIONS FOR MENU OPERATION

int choice(int choiceIndicator, int lineStart) //general selection menu
{
	string arrow[4] = {">"," "," "," "}, choices[4]; //4 is max possible number of choices in this program
	int currentChoice = 0, numChoices;

	time10[T1] = 0;

	if(choiceIndicator == 1) //The following if statements are coded since arrays cannot be passed into functions in Robot C.
	{
		choices[0] = "Coke";
		choices[1] = "Sprite";
		choices[2] = "Fanta";
		choices[3] = "Canada Dry";
		numChoices = 4;
	}
	else if (choiceIndicator == 2)
	{
		choices[0] = "Yes";
		choices[1] = "No";
		numChoices = 2;
	}
	else if (choiceIndicator == 3)
	{
		choices[0] = "1";
		choices[1] = "2";
		choices[2] = "3";
		numChoices = 3;
	}

	while(nNxtButtonPressed !=3) //displays all drink choices
	{
		if (time10[T1]%100 < 50) //arrow blinks on
			for (int i=0; i < numChoices; i++)
				nxtDisplayString(lineStart+i, "%s%s", arrow[i], choices[i]);
		else //arrow blinks off
			for (int i=0; i < numChoices; i++)
				nxtDisplayString(lineStart+i, " %s", choices[i]);

		if (nNxtButtonPressed == 1 && currentChoice+1 < numChoices) //moves arrow down onscreen; if arrow is on last choice, do nothing
		{
			while (nNxtButtonPressed == 1);
			arrow[currentChoice] = " ";
			currentChoice += 1;
			arrow[currentChoice] = ">";
		}
		else if (nNxtButtonPressed == 2 && currentChoice-1 >= 0) //moves arrow up; if arrow is on first choice, do nothing
		{
			while (nNxtButtonPressed == 2);
			arrow[currentChoice] = " ";
			currentChoice -= 1;
			arrow[currentChoice] = ">";
		}
	}
	while(nNxtButtonPressed == 3);
	eraseDisplay();
	return currentChoice;
}

bool completelyEmpty(int cansLeft1, int cansLeft2, int cansLeft3, int cansLeft4)
{
	if (cansLeft1+cansLeft2+cansLeft3+cansLeft4 > 0) //checks if machine is completely empty
		return false;
	return true;
}


//FUNCTIONS WITH OVERLOADING

int order() //simple order
{
	bool notCorrect;
	int drink;
	string soda[4] = {"Coke","Sprite","Fanta","Canada Dry"};

	do
	{
		eraseDisplay();
		nxtDisplayString(2,"Choose a drink:");
		drink = choice(1,3); //selection menu: displays all drinks; return to drink menu

		nxtDisplayString(2, "You chose:");
		nxtDisplayString(3, "%s", soda[drink]);
		nxtDisplayString(4, "Is this correct?");
		notCorrect = choice(2,5); //selection menu: yes or no; return to drink menu
	} while(notCorrect);
	return drink;
}

int order(int & numOfCans) //complex order
{
	int drinkType = order();
	nxtDisplayString(2, "How many cans?");
	numOfCans = choice(3,3)+1;//selection menu: 1,2, or 3 return 0, 1, 2; user chooses desired number of cans of a certain type
	return drinkType;
}


//FUNCTIONS FOR DISPENSING DRINKS

void rotate(int choice) //rotate until the certain colour which corresponds to a drink is seen
{
	while(choice != SensorValue[S3]-2)
	{
		motor[motorA] = 15;
	}
	motor[motorA]=0;
}

void armMovement()
{
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -100;
	while (nMotorEncoder[motorC] > -240); //arm movement
	motor[motorC] = 100;
	while (nMotorEncoder[motorC] <= -40); //encoder is not completely accurate; should be 0 otherwise
	motor[motorC] = 0;
}

bool dispense() //dispense a can
{
	armMovement();
	time1[T1] = 0;
	while (SensorValue[S2] == 0) //touch sensor detects can
	{
		if (time1[T1] > 2000)
		{
			nxtDisplayString (0, "Dispensing Error!");
			nxtDisplayString (1, "Retrying...");
			wait1Msec(1000);
			for(int i=0; i<3;i++) //try three times to redispense
			{
				armMovement();
				time1[T2] = 0;
				while(SensorValue[S2] == 0 && time1[T2] <2000);
				if (SensorValue[S2] == 1)
					return true;
			}
			return false;
		}
	}
	return true;
}


//MAIN PROGRAM

task main()
{
	SensorType[S3] = sensorCOLORFULL;
	SensorType[S2] = sensorTouch;

 	startup();
	eraseDisplay();

	int numLeft[4] = {3,3,3,3};	//assume inital number of drinks of each type is 3
	bool anotherOrder, working = true;

	do
	{
		anotherOrder = false;
		int numOrders = 0, numCans[12] = {0,0,0,0,0,0,0,0,0,0,0,0}, drinkChoice[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; //assume max number of orders is 12

		nxtDisplayString(2, "Order more than");
		nxtDisplayString(3, "one can?");
		int choiceType = choice(2,4); //more than 1 can? (0=yes, 1=no)

		if (choiceType == 1) //only 1 can
		{
			bool enoughCans;

			do {
				enoughCans = true;
				drinkChoice[0] = order(); //index 0 gets type of drink
				eraseDisplay();

				if (numLeft[drinkChoice[0]]-1 < 0) //if no cans left
				{
					enoughCans = false;
					nxtDisplayString(3, "Not enough cans!");
					nxtDisplayString(4, "Order again.");
					wait1Msec(2000);
				}
				else
				{
					numLeft[drinkChoice[0]]--;
					numOrders = 1;
					numCans[0] = 1;
				}

			}while(!enoughCans); //repeat single-can order
		}

		else
		{
			bool anotherCan;

			do {
				anotherCan = false;

				drinkChoice[numOrders] = order(numCans[numOrders]); //drinkChoice array gets the type; numCans array gets number ordered

				if (numLeft[drinkChoice[numOrders]] - numCans[numOrders] < 0) //not enough cans?
				{
					anotherCan = true;
					eraseDisplay();
					nxtDisplayString(3, "Not enough cans!");
					nxtDisplayString(4, "Order again.");
					wait1Msec (2000);
					eraseDisplay();
				}
				else
				{
					numLeft[drinkChoice[numOrders]] -=numCans[numOrders]; //decrease number of cans of that type
					numOrders++;

					if (!completelyEmpty(numLeft[0],numLeft[1],numLeft[2],numLeft[3]))
					{
						nxtDisplayString(2, "Another drink?");
						if(!choice(2,3)) //selection menu: another can (yes=0, no=1)
							anotherCan = true;
					}
				}

			} while(anotherCan && numOrders <=12 ); //maximum 12 orders
		}
		displayFace();

		for(int i = 0; i<numOrders && working; i++) //increment through drinks
		{
			for(int n=0;n<numCans[i] && working;n++) //increment through number of cans for each drink
			{
				rotate(drinkChoice[i]);
				displayFaceSleep();
				wait1Msec(500);

				working = dispense(); //returns a bool
				displayFace();
				wait1Msec(1000);
			}
		}
		eraseDisplay();

		if (working && !completelyEmpty(numLeft[0],numLeft[1],numLeft[2],numLeft[3])) //since array cannot be passed into function
		{
			nxtDisplayString(2, "Another Order?");
			if(!choice(2,3)) //selection menu: another can
				anotherOrder = true;
		}
	}	while(anotherOrder && working);

	nxtDisplayString(3, "Goodbye!");
	wait1Msec(2000);
	end();
}