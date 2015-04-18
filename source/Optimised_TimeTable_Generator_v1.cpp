
//****************************************************************
//  OPTIMISED TIME-TABLE GENERATING PROGRAM
//
//  CS101, Spring Semester, 2015.
//
//  Created By,
//  R.Basuhi(140010037)
//  Shachi Shailesh Deshpande (140110047)
//  Yashaswini K. Murthy (140010054)
//
//  Submitted On: 16/04/2015
//
//****************************************************************



#include<cstdlib>
#include<cmath>
#include<iostream>
#include<fstream>
#include<conio.h>
#include<iomanip>
using namespace std;
#define DEFAULT_ADD 2
#define PROF_WT 5
#define TUT_TA_WT 2
#define LAB_TA_WT 2
#define PROF_ID_L 1000
#define PROF_ID_U 1011
void assignDefaultValues();

/*
    A BRIEF OVERVIEW OF THE SCENARIO:

    We have 4 batches or divisions, namely D1, D2, D3 and D4. There are 5 theory courses, 2 of which have tutorial classes too;
    And there are two lab courses.
    We have 10 profs with each subject being taught by 2 of them and each professor teaching 2 divisions,
    8 Lab TAs (4 for Physics and 4 for Chemistry) and 4 Tutorial Head TAs (2 for Physics and 2 for Maths).
    We assign the profs ID from 1001 to 1010; the Physics Lab TAs from 201 to 204 and Chemistry Lab TAs from 301 to 304;
    Tutorial TAs from 101-104.

    IMPLEMENTING THIS IN CODE:

    There is a class Cell which consitutes a unit of the class Timetable. There are 5X8=40 Cells in a Timetable.
    We have four such Timetables, one for each division D1, D2, D3 and D4.
    We take input from profs and this input is taken in as an instance of class Professor,
    or class TutotialTA/LabTA which ahve inherited from class Professor. Several functions have been overridden
    because of the variability in the algorithm of optimisation

    OPTIMISATION:
    This is the heart of this program and is achieved by a method called perturbation wherein pseudo random changes are made
    to an existing template of a timetable and a function called objective function keeps track of the level of optimisation.
    For the sake of simplicity and time-effectiveness, only 100 such perturbations are made in this program.

*/



/*For storing values in a file for ease of debugging*/
ofstream outfile("draft-output.txt");
string courses[]={"PH107", "CH107", "MA106", "IC102", "BB101"};
int courseHours[]={2,3,2,3,3};
string tuteCourses[]={"PH107", "MA106"};
string labCourses[]={"PH117L", "CH117L"};
string Day[]={"Monday","Tuesday","Wednesday","Thursday","Friday"};

/*  class Cell is a unit of Timetable.
    It represents one period in the timetable.
    There are 8 Cells in a day and 5 such working days.
*/

class Cell
{
        string Subject;
        string Professor_name;
        int Teacher_ID;
        char slotType;   //P,L or T for Professor's class, Lab or Tutorial.
        bool Occupied;   //True or false according to whether slot has been alloted

        public:
        Cell()                   //Cell stays unoccupied by default
        {
            Occupied=false;
        }

        void setOccupied()
        {
            Occupied=true;
        }
        void setOccupied(int TID) // Marks cell as occupied and inputs TID
        {
            Teacher_ID=TID;
            Occupied=true;
        }

        void initCellData(string sub, string pname, int TID, char stype, string venue)
        {
            Subject=sub;
            Professor_name=pname;
            Teacher_ID=TID;
            slotType=stype;
            venue=venue;
            Occupied=true;
        }

        bool isOccupied()
        {
            if(Occupied==true) return true;
            else return false;
        }


        string getSubject()
        {
            return Subject;
        }

        int getTeacher_ID()
        {
            return Teacher_ID;
        }

        void setEmpty()
        {
            Occupied=false;
        }
        void setSlotType(char c)
        {
            slotType=c;
        }
/*
    This function will be used to print individual cells of timetable
*/
        void featureCell()
        {   if(this->isOccupied())
            {
                outfile<<Subject<<"\t"<<Teacher_ID<<"\t"<<Professor_name;
                outfile<<Teacher_ID<<endl;
            }
            else
            {
              outfile<<"----\t----\t----";
              outfile<<"\t"<<endl;
            }

        }

};



/*  class Professor contains all the information like, name, ID, Subject, Divisions teaching, etc
    that pertains to a prof who wants to teach a particulat subject.
    Also there is a preference matrix, an allot matrix and a timetable matrix, explained below.
*/

class Professor
    {
        protected:

        int number_of_lectures;     //Stores number of lectures in a week required for the subject
        int Division[2];            //Division number 1 to 4 in each of the element, corresponding to divisions he teaches
        string name;
        int ID;
        string Subject;
        int Pref[5][3];     //1st element of each row stores preference value for corresponding weekday, e.g. Pref[0][0] stores preference
                            // value in integer 1 to 5 for monday
                            // 2nd and 3rd entries in each row store either 1 or 0 corresponding forenoon and afternoon slots
                            // according to  'yes' or 'no' of prof to that slot for that day
        int Allot[5][3];    //stores number of times particular day and slot is alloted to a professor

        int myTimeTable[5][8];  //stores zeroes by default, stores division number to which a lecture is assigned, otherwise


        int myObjFunction;

        public:

        void setSubject(int subject_code)
        {
            Subject=courses[subject_code];
            number_of_lectures=courseHours[subject_code];
        }

        /*The following 4 functions are used to construct default cases for ease of experimentation or verification*/



        /* TYPE 1: Profs get alloted preference values 5,4,3,2,1 for corresponding weekdays (i.e. Monday, Tuesday,etc.) and all morning slots
                    input as preferred slot for professor*/
        void setPreferenceMatrixType1(int slot=1)
        {
            int prefValue=5;
            int i=0;
            while(prefValue>0)
            {
                Pref[i][0]=prefValue;
                prefValue--;
                i++;
                Pref[i][slot]=1;
            }
         }


        /* TYPE 2: Profs get alloted 1,2,3,4,5 and all afternoon slots*/
        void setPreferenceMatrixType2(int slot=2)
        {
            int prefValue=1;
            int i=0;
            while(prefValue<6)
            {

                Pref[i][0]=prefValue;
                prefValue++;
                i++;
                Pref[i][slot]=1;
            }
         }

        /*TYPE 3: Profs will get 5,4,3,2,1 in alternating slots*/
        void setPreferenceMatrixType3()
        {   int i=0;
            int prefValue=5;
            while(prefValue>0)
            {
                Pref[i][0]=prefValue;
                prefValue--;
                Pref[i][1+i%2]=i%2;
                Pref[i][1+(1+i)%2]=(1+i)%2;
                i++;
            }
         }

        /*TYPE 4: Different days get assigned different slots. The entry may start with anyday of the week depending on value of perm.*/
        void setPreferenceMatrixType4(int perm, int slot)
        {
                int prefValue=1;
                int i=perm%5;                     //Alloting preference values to days starts at any number 'i' which is decided by
                                                   //value of 'perm' as argument to function.
                while(prefValue<6)
                {
                Pref[i][0]=prefValue;
                prefValue++;
                Pref[i][slot]=1;
                i=(i+1)%5;
                }
        }

        /*Used to stream the output to preference to a file*/

        void displayPreference()
        {
            outfile<<"Preference Matrix\n";
            for(int i=0;i<5;i++)
            {
                for(int j=0;j<3;j++)
                {
                outfile<<Pref[i][j]<<" ";
                }
            outfile<<endl;
            }
        }
/* Used to display individual timetable of Professor  */
        void displayMyTimeTable()
        {
            outfile<<"MyTimeTable\n";
            for(int i=0;i<5;i++)
            {
                for(int j=0;j<8;j++)
                {
                outfile<<myTimeTable[i][j]<<" ";
                }
                outfile<<endl;
            }
         }
/*   Used to display the Allot matrix, which keeps track of actual lecture slots alloted to the Professor */
        void displayAllot()
         {
            for(int i=0;i<5;i++)
            {
                for(int j=0;j<3;j++)
                {
                    outfile<<Allot[i][j]<<" ";
                }
                    outfile<<endl;
            }
         }

        Professor()
        {
            outfile<<"Constructor of Professor\n";
            for(int i=0;i<5;i++)                    //initialize all elements of Professor's timetable to zero.
            {   for(int j=0;j<8;j++)
                myTimeTable[i][j]=0;
            }

            for(int i=0;i<5;i++)                    //initialize entire Allot matrix to zero
            {   for(int j=0;j<3;j++)
                Allot[i][j]=0;
            }
            for(int i=0;i<5;i++)                   //initializing preference matrix to zero.
            {
                  Pref[i][0]=i+1;
                  Pref[i][1]=1;
                  Pref[i][2]=0;
            }
            number_of_lectures=2;                    //number of lectures initially put zero.

        }

        void setDivision(int div,int number)    //sets 1 division at a time
         {
          if(number==1)
             Division[0]=div;
          if(number==2)
             Division[1]=div;
         }


        /* This function will take input for the 5x3 matrix Pref*/
        void getPrefOrder()
        {   cout<<"\nNow you will be giving the prefernce order of you available days. If Monday is least preferred assign it 1, and if most preferred assign it 5. And so on...Also each day must have a distinct preference value. And thus 1-5 numbers must be covered.";


            for(int i=0;i<5;i++)
            {   int choice; bool wrong=false, repeating=false;


                cout<<"\n\nDay:  "<<Day[i]<<"\n";
                do
                {
                repeating=false;
                cout<<"\nEnter Preference for  "<<Day[i]<<"\t";
                cin>>Pref[i][0];

                if(Pref[i][0]>5 || Pref[i][0]<1)
                {   repeating=true;
                    cout<<"The preference must be between 1 and 5. No other numbers/characters are allowed.";

                }

                for(int j=0;j<i;j++)
                {
                    if(Pref[i][0]==Pref[j][0])
                    {   repeating=true;
                        cout<<"\nPreference must be distinct and unique for each day. Enter again.\n";
                        break;
                    }
                }
                } while(repeating);

                do
                {
                wrong=false;
                cout<<"\nEnter your slot preference: \n 1. Morning\n 2. Afternoon\n 3. Both\n\nYour choice:";
                cin>>choice;
                switch(choice)
                {
                    case 1: {Pref[i][1]=1;
                             Pref[i][2]=0;
                             break;}
                    case 2: {Pref[i][1]=0;
                             Pref[i][2]=1;
                             break;}
                    case 3: {Pref[i][1]=1;
                             Pref[i][2]=1;
                             break;}
                    default: {cout<<"\nWrong choice entered. Enter again, phew.";
                              wrong=true;  }

                }
                }while(wrong);
            }

        }



        void getInput(int pseudo_sub_ID, int prof_ID, int div)
        {   cout<<"Enter your name(only first name):";
            cin>>name;
            ID=prof_ID; //we assign this based on the subject he chooses.
            Subject=courses[pseudo_sub_ID-1];
            number_of_lectures=courseHours[pseudo_sub_ID-1];
            if(div==0)
            {Division[0]=1; Division[1]=2;}
            else
            {Division[0]=3; Division[1]=4;}
            getPrefOrder();
            cout<<"\n\nThank you! Have a nice day!";
        }


        int getnumber_of_lectures()
        {
            return number_of_lectures;
        }

        /*  Returns index of day set as next max preference by the prof
            If order=2, then function returns 2nd most favourable day proposed by prof*/

        int findNextMaxPrefDay(int order)
        {
            int preference_array[5]={1,2,3,4,5};//1 means monday, 2 means tuesday and so on
            int tempPref[5][3];
            for(int i=0;i<5;i++)
             {
                 for(int j=0;j<3;j++)
                  {
                    tempPref[i][j]=Pref[i][j];
                  }
             }

            for(int i=0;i<4;i++)
            {
                                              // selection sort, sorts our preference array defined in function along input array.
                for(int j=i+1;j<5;j++)
                {
                    if(tempPref[i][0]<tempPref[j][0])
                    {
                    int temp=tempPref[j][0];
                    tempPref[j][0]=tempPref[i][0];
                    tempPref[i][0]=temp;
                    temp=preference_array[j];
                    preference_array[j]=preference_array[i];
                    preference_array[i]=temp;
                    }
                }
            }
                outfile<<"findNextMaxPrefDay executed Preference_array[order-1]="<<preference_array[order-1]<<endl;
                return preference_array[order-1];
        }//sort array of Pref, store index of sorted elements and return the index defined by 'order'

        int getID()
        {
            return ID;
        }
        void setID(int TID)
        {
            ID=TID;
        }

        int getObjFunction()
        {
            objectiveFunction(Pref,Allot);
            return myObjFunction;
        }
/*   This function checks value of objective function of the individual professor for probable allotment stored in
     tempAllot matrix  */
        int tryGetObjFunction(int Pref[][3],int tempAllot[][3])
        {
            outfile<<"into trygetobjfunction()\n";
            objectiveFunction(Pref,tempAllot);
            int tempObjFunction=myObjFunction;
            objectiveFunction(Pref,Allot);
            outfile<<"trygetfunc successful tempobjfunction="<<tempObjFunction<<endl;
            return tempObjFunction;
        }
/*  This function calculates objective function, which reflects satisfaction level of the Professor by comparing his preferences
    to weekdays and corresponding slots versus the actual allotment done. Also the value of objective function is stored in variablr
    myObjFunction.
    */
        void objectiveFunction(int sPref[][3],int sAllot[][3])
        {
            outfile<<"into objectivefunction()\n";
            int sum=0;
            int i=0;
            for(i=0;i<5;i++)
            {                                                     /* sees if profs got demanded forenoon or
                                                                    afternoon slots by comparing corresponding value*/

                if(sAllot[i][0]!=0)
                {
                sum+=PROF_WT*sAllot[i][0]*sPref[i][0]*(DEFAULT_ADD+(Pref[i][1]*sAllot[i][1]+sPref[i][2]*sAllot[i][2]));
                }
            }
            outfile<<"value of objective function "<<sum<<endl;
            myObjFunction=sum;
        }
        /*  Takes in day and corresponding slots to be exchanged .
            Returns the modified objective function in case such exchange is done
            day1=source;day2=destination of exchange*/

        int tryObjectiveFunction(int day1,int day2,int slot)
        {
            outfile<<"into try objectivefunction()\n";
            int tempAllot[5][3];

            for(int i=0;i<5;i++)        //copied Allot matrix into tempAllot matrix
            {
                for(int j=0;j<3;j++)
                {
                tempAllot[i][j]=Allot[i][j];
                }
            }
            tempAllot[day2-1][0]+=1;
            tempAllot[day2-1][slot]+=1;
            tempAllot[day1-1][0]-=1;
            tempAllot[day1-1][slot]-=1;

            return tryGetObjFunction(Pref,tempAllot);
        }



        int getDivision(int number)    // returns one of the divisions alloted to prof, specified as number 1 or 2
        {

            if(number==1)
            return(Division[0]);
            else if(number==2)
            return(Division[1]);
            else
            return -1;
        }

        string getSubject()
        {
            return Subject;
        }

        int profSlotPreference(int day)  // returns the slot either forenoon(1) or afternoon(2) or both(0) as preferred by the Prof
                                         // on particular day (1 for monday and so on) as argument to the function
        {
            if(Pref[day-1][1]==1&&Pref[day-1][2]==1)
            {
            return 0;
            }
            else if(Pref[day-1][1]==1)
            {
            return 1;
            }
            else if(Pref[day-1][2]==1)
            {
            return 2;
            }
            else return -1;
       }

        bool checkIfFree(int day, int subslot)  // checks if subslot corresponding to particular subslot is free for prof
        {
            if(myTimeTable[day-1][subslot-1]==0)
            {
            outfile<<"I am free\n";
            return true;
            }
            else
            {
            outfile<<"I am not free\n";
            return false;
            }
        }

        /*  Whenever prof is alloted particular day, corresponding entries in
            Allot matrix are increased by 1. e.g. If day=1 and slot=1, then corresponding entries in Allot matrix, i.e.
            Allot[1][0] and Allot[1][1] will be increased by 1.
        */


        void modifyAllotMatrix(int day, int slot)
        {
            Allot[day-1][0]=Allot[day-1][0]+1;
            Allot[day-1][slot]=Allot[day-1][slot]+1;
            outfile<<"modifyAllotMatrix() executed \n";
            displayAllot();
        }

        /*
            Modifies allot matrix and myTimeTable of Professor after particular exchange is done.
        */

        void accomodateExchange(int day1,int day2,int slot,int subslot1,int subslot2,int division)
        {
            Allot[day1-1][0]-=1;
            Allot[day2-1][0]+=1;
            Allot[day1-1][slot]-=1;
            Allot[day2-1][slot]+=1;
            myTimeTable[day1-1][subslot1-1]=0;
            myTimeTable[day2-1][subslot2-1]=division;
            objectiveFunction(Pref,Allot);                   //value of new objective function updated
            outfile<<"accomodateExchange() executed\n";
            outfile<<"day1 "<<day1<<" exchanged with day2 "<<day2<<" in division "<<division<<" with subslots "<<subslot1<<subslot2<<endl;
        }

        /*  Marks a prof available for particular subslot for particular day*/


        void freeSubSlot(int day,int subslot)
        {
            myTimeTable[day-1][subslot-1]=0;
        }


        /*  Occupies particular subslot of timetable of Professor for particular division.*/

        void occupySubSlot(int day,int subslot,int division)
        {
            myTimeTable[day-1][subslot-1]=division;
        }
/*  find the allotment which makes more contribution to reduction of objective function */
        int findImpactingAllotment()
        {   int i;
            for(i=4;i>=0;i--)
            {
            if(Allot[findNextMaxPrefDay(i+1)-1][0]!=0) //checks if there is allotment to day which is LESS preffered, i.e. of i'th
                                                       //order where i runs from 4 to 0
                {
                    break;
                }
            }
           outfile<<"findImpactingAllotment() executed-the day is "<<findNextMaxPrefDay(i+1);
           return(findNextMaxPrefDay(i+1));
        }
/*   gives the subslot in given slot when the professor teaches a particular division on particular day  */
        int getSubSlot(int day,int slot,int division)
        {
           if(slot==0)                                 //slot=0 means that both morning and afternoon slots are to be checked
           {
            for(int i=0;i<8;i++)
            {
                outfile<<"myTimeTable["<<day<<"-1]["<<i<<"]="<<myTimeTable[day-1][i];
                if(myTimeTable[day-1][i]==division)
                {
                    return i+1;
                    outfile<<"getSubSlot() executed-"<<i+1<<endl;
                }
                else
                {
                   continue;
                }
            }
            }
            else
            {
            for(int i=(slot-1)*4;i<(slot-1)*4+4;i++)                           //scans only the subslots corresponding to slot passed as argument
            {
                outfile<<"myTimeTable["<<day<<"-1]["<<i<<"]="<<myTimeTable[day-1][i];
                if(myTimeTable[day-1][i]==division)
                {
                    return i+1;
                    outfile<<"getSubSlot() executed-"<<i+1<<endl;
                }
                else
                {
                   continue;
                }
            }
            }
            return 0;             // in case there is no sub-slot in which prof teaches the division given as input to function
         }
/*     Marks particular hour in prof's timetable as occupied by particular division number so that it is not used
       again for scheduling other lecture */


        void modifyMyTimeTable(int day,int subslot,int division)
        {
            myTimeTable[day-1][subslot-1]=division;
            outfile<<"modifyMyTimeTable() executed at day and subslot "<<day<<" "<<subslot<<endl;
        }

   };

/*
    Tutorial_TA inherits from Professor.
    It behaves almost very like Professor and has very few overridden functions

*/

class TutorialTA : public Professor
{
        public:
        TutorialTA()             //number of lectures and prference matrix initialized
        {
         for(int i=0;i<5;i++)
          {
          Pref[i][0]=0;
          Pref[i][1]=0;
          Pref[i][2]=0;
          }
          number_of_lectures=1;
        }

        void getInput(int pseudo_tute_ID, int TTA_ID, int div)
        {   cout<<"Enter your name:";
            cin>>name;
            ID=TTA_ID; //we assign this based on the subject he chooses.

            Subject=tuteCourses[pseudo_tute_ID-1];
            number_of_lectures=1; //actually number of TUTS, but well.
            if(div==0)
            {Division[0]=1; Division[1]=2;}
            else
            {Division[0]=3; Division[1]=4;}
            getPrefOrder();
            cout<<"Thank You! Have a Nice Day";
        }

         void setSubject(int tute_subject_code)
        {
            Subject=tuteCourses[tute_subject_code];
            number_of_lectures=1;

        }
/*  This Function computes objective function of Tutorial TA with a lesser weight as compared to the Professor  */
         void objectiveFunction(int Pref[][3],int Allot[][3])
        {
            int sum=0;
            int i=0;
            for(i=0;i<5;i++)
            {
            if(Allot[i][0]!=0)
            {
            sum+=TUT_TA_WT*Allot[i][0]*Pref[i][0]*(DEFAULT_ADD+(Pref[i][1]*Allot[i][1]+Pref[i][2]*Allot[i][2]));    //sees if Head TA got demanded forenoon or
                                                                                                                   //afternoon slots by comparing corresponding
                                                                                                                   //value
            }
            }
            myObjFunction=sum;
      }

};

/*
    class LabTA also inherits from Professor but has significant number of overridden functions owing to the fact that
    the allotment and perturbation of lab slots is very different.

*/

class LabTA : public Professor
    {
        public:
        LabTA()        //number of lectures and timetable of lab TA initialized
        {
            number_of_lectures=2;
            for(int i=0;i<8;i++)
            {
                for(int j=0;j<5;j++)
                {
                    myTimeTable[i][j]=0;
                }
            }
        }

        void setID(int TID)
        {
            ID=TID;
            number_of_lectures=2;
        }

        void setnumber_of_lectures(int n)
        {
            number_of_lectures=n;
        }

        void setLabSubject(int lab_subject_code)
        {
            Subject=labCourses[lab_subject_code];
            number_of_lectures=2;
        }

        string getSubject()
        {
            return Subject;
        }



        void getInput(int pseudo_lab_ID, int LTA_ID, int div) //overriden fucntion
        {   cout<<"Enter your name:";
            cin>>name;
            ID=LTA_ID; //we assign this based on the subject he chooses.

            Subject=labCourses[pseudo_lab_ID-1];
            number_of_lectures=2; //actually number of labs, but well.

            for(int i=0;i<8;i++)
            {
                for(int j=0;j<5;j++)
            {
                myTimeTable[i][j]=0;
            }
            }

            if(div==0)
            {Division[0]=1; Division[1]=2;}
            else
            {Division[0]=3; Division[1]=4;}
            getPrefOrder();
            cout<<"Thank You. Have a nice day!";

        }
/*     Following functions work nearly same as those in class Professor, with some changes, since one lab session continues for 4 hours,
       while 1 lecture in class Professor meant just one hour   */
        void modifyAllotMatrix(int day,int slot)
        {
            Allot[day-1][0]+=4;              //1 allotment equivalent to 4 hours
            Allot[day-1][slot]+=4;
        }

        void modifyMyTimeTable(int day,int slot,int division)
        {
            for(int i=(slot-1)*4;i<(slot-1)*4+4;i++)    //1 allotment actually occupies 4 hours in TA's own timetable as well
            myTimeTable[day-1][i]=division;
        }

        int getObjFunction()
        {
            objectiveFunction(Pref,Allot);
            return myObjFunction;
        }


        int tryGetObjFunction(int Pref[][3],int tempAllot[][3])
        {
            outfile<<"into trygetobjfunction()\n";
            objectiveFunction(Pref,tempAllot);
            int tempObjFunction=myObjFunction;
            objectiveFunction(Pref,Allot);
            outfile<<"trygetfunc successful tempobjfunction="<<tempObjFunction<<endl;
            return tempObjFunction;
        }

        void objectiveFunction(int sPref[][3],int sAllot[][3])
        {
            outfile<<"into objectivefunction()\n";
            int sum=0;
            int i=0;
            for(i=0;i<5;i++)
            {
            if(sAllot[i][0]!=0)
            {
                sum+=LAB_TA_WT*sAllot[i][0]*sPref[i][0]*(DEFAULT_ADD+(Pref[i][1]*sAllot[i][1]+sPref[i][2]*sAllot[i][2]));//sees if profs got demanded forenoon or
                                                                                                                         //afternoon slots by comparing corresponding
                                                                                                                          //value
            }
            }
            outfile<<"value of objective function "<<sum<<endl;
            myObjFunction=sum;
        }

        /*takes in day and corresponding slots to be exchanged returns modified
        objective function in case such exchange is done
        day1=source;day2=destination of exchange . Similarly for slot 1(source) and slot 2(destination)- This is CHANGE from corresponding function in
        parent class Professor , since in that case we don't consider exchanging allotment of professors in different slots, but this
        is necessary for exchanges in case of labs */

        int tryObjectiveFunction(int day1,int day2,int slot1,int slot2)
        {
            outfile<<"into try objectivefunction()\n";
            int tempAllot[5][3];

            for(int i=0;i<5;i++)
            {
                for(int j=0;j<3;j++)
                {
                tempAllot[i][j]=Allot[i][j];
                }
            }
            tempAllot[day2-1][0]+=1;
            tempAllot[day2-1][slot2]+=1;
            tempAllot[day1-1][0]-=1;
            tempAllot[day1-1][slot1]-=1;

            return tryGetObjFunction(Pref,tempAllot);
        }


        /*
            Modifies allot matrix and myTimeTable of Professor after particular exchange is done. Updates objective function
        */

        void accomodateExchange(int day1,int day2,int slot1,int slot2)
        {
            Allot[day1-1][0]-=4;
            Allot[day2-1][0]+=4;
            Allot[day1-1][slot1]-=4;
            Allot[day2-1][slot2]+=4;
            for(int i=(slot1-1)*4;i<(slot1-1)*4+4;i++)
            {
            myTimeTable[day1-1][i]=0;
            }
            for(int i=(slot2-1)*4;i<(slot2-1)*4+4;i++)
            {
            myTimeTable[day2-1][i]=1;
            }
            objectiveFunction(Pref,Allot);
            outfile<<"accomodateExchange() executed\n";
            outfile<<"day1 "<<day1<<" exchanged with day2 "<<day2<<" with claslots "<<slot1<<slot2<<endl;
        }
};
/* Declaration of global variables used in this program  */
Professor P[10];
LabTA PL[4];
LabTA CL[4];
TutorialTA T[4];

/*  Find prof of specified order in array of profs sorted according to their satisfaction level
    indicated by prof's objective function. TECHNICALLY WE GET ORDERED LIST OF PROFS. SO MOST
    SATISFIED PROF COULD ALSO BE FOUND BY GIVING ORDER 10 IN ARGUMENT
*/

int findLessSatisfiedProf(int order,Professor P[])
{
    int profObjFuncValues[10];
    for(int i=0;i<10;i++)
    {
       profObjFuncValues[i]=P[i].getObjFunction();
       outfile<<"P["<<i<<"].getObjFunction()"<<P[i].getObjFunction()<<endl;
    }
    int orderOfProfs[10]={1,2,3,4,5,6,7,8,9,10};
    for(int i=0;i<9;i++)
    {
        for(int j=i+1;j<10;j++)
        {
            if(profObjFuncValues[i]>profObjFuncValues[j])               //selection sort
            {

                int temp=orderOfProfs[i];
                orderOfProfs[i]=orderOfProfs[j];
                orderOfProfs[j]=temp;
                temp=profObjFuncValues[i];
                profObjFuncValues[i]=profObjFuncValues[j];
                profObjFuncValues[j]=temp;
            }
        }
    }
    for(int i=0;i<10;i++)
    {
        outfile<<"orderOfProfs["<<i<<"]"<<orderOfProfs[i]<<endl;
        outfile<<"profObjFuncValues["<<i<<"]"<<profObjFuncValues[i]<<endl;
    }
    outfile<<"Less satisfied prof found order="<<orderOfProfs[order-1]<<endl;
    return(orderOfProfs[order-1]);
}

/* Similar function as the one above, but customised for searching array of LabTA's to find less satisfied TA in sorted array
   (according to satisfaction level of LabTA)  of specified order */
int findLessSatisfiedProf(int order,LabTA* P) //For LabTA.
{
    int profObjFuncValues[4];
    for(int i=0;i<4;i++)
    {
       profObjFuncValues[i]=P[i].getObjFunction();
       outfile<<"P["<<i<<"].getObjFunction()"<<P[i].getObjFunction()<<endl;
    }
    int orderOfProfs[4]={1,2,3,4};
    for(int i=0;i<3;i++)
    {
        for(int j=i+1;j<4;j++)
        {
            if(profObjFuncValues[i]>profObjFuncValues[j])           //selection sort
            {

                int temp=orderOfProfs[i];
                orderOfProfs[i]=orderOfProfs[j];
                orderOfProfs[j]=temp;
                temp=profObjFuncValues[i];
                profObjFuncValues[i]=profObjFuncValues[j];
                profObjFuncValues[j]=temp;
            }
        }
    }

    for(int i=0;i<4;i++)
    {
         outfile<<"orderOfProfs["<<i<<"]"<<orderOfProfs[i]<<endl;
         outfile<<"profObjFuncValues["<<i<<"]"<<profObjFuncValues[i]<<endl;
    }
    outfile<<"Less satisfied prof found order="<<orderOfProfs[order-1]<<endl;
    return(orderOfProfs[order-1]);
}

/*  Cell c1 and Cell c2 are cells to be exchanged, day1 and day2 are days corresponding to c1 and c2
    and slot is the slot of TimeTable used in exchange.
    This function checks if this exchange can actually improve the total objective function.*/

bool isExchangeGood(Cell c1,Cell c2,int P1Index,int P2Index,int day1,int day2,int slot)
{
    if(P2Index+1001>PROF_ID_L&&P2Index+1001<PROF_ID_U)     //checks if the cell with which exchange is being done is occupied
                                                            //by another Professor's lecture
    {
    Professor P1=P[P1Index];
    Professor P2=P[P2Index];
    outfile<<"into isexchange good function\n";
    if(P1.getID()!=P2.getID())      //checks if slots of same prof are not exchanged, ensures efficiency of code
    {
        outfile<<"prof ID's different.\n";
    /* Actually comparing if the exchange will improve total objective function  */
        if(P1.tryObjectiveFunction(day1,day2,slot)+P2.tryObjectiveFunction(day2,day1,slot)>P1.getObjFunction()+P2.getObjFunction())
        {
            outfile<<"Exchange good!\n";
            return true;
        }
        else
        {
            outfile<<"exchange not good\n";
            return false;
        }
    }
    else
    {
        return false;
    }
    }
    else                //if Cell c2 is occupied by Tutorial TA , checking if the exchange improves total objective function
    {
        Professor P1=P[P1Index];
        TutorialTA P2=T[P2Index];
        outfile<<"into isexchange good function\n";
        if(P1.tryObjectiveFunction(day1,day2,slot)+P2.tryObjectiveFunction(day2,day1,slot)>P1.getObjFunction()+P2.getObjFunction())
        {
            outfile<<"Exchange good!\n";
            return true;
        }
        else
        {
            outfile<<"exchange not good\n";
            return false;
        }
    }
}

bool isExchangeGood(Cell c1,Cell c2,int PIndex,int day1,int day2,int slot) //used in case the destination of exchange is not occupied by a professor,i.e. vacant slot
{
    if(P[PIndex].tryObjectiveFunction(day1,day2,slot)>P[PIndex].getObjFunction())
    {
        outfile<<"Exchange Good!\n";
        return true;
    }
    else
    {
         return false;
    }
}
/*   This function computes objective function of the entire scheduling process, but does not include Lab TA's, as that is
     completely different type of perturbation */

int TotalobjectiveFunction(Professor *P,TutorialTA *T)
    {
        int sum=0;

        for(int i=0;i<10;i++)
        {
            sum+=P->getObjFunction();
            outfile<<P->getObjFunction();
            P++;
            outfile<<"totalobjfunc sum="<<sum<<endl;
        }
        for(int i=0;i<4;i++)
        {
            sum+=T->getObjFunction();
            T++;
            outfile<<"totalobjfunc sum="<<sum<<endl;
        }
        return sum;
    }
/*  returns Professor object corresponding to particular ID entered */
Professor getProfessor(int ID,Professor* P)
{
    for(int i=0;i<10;i++)
    {

        if(P[i].getID()==ID)
        {
            outfile<<"getProfessor() executed P["<<i<<"] got\n";
            return P[i];
        }
        else continue;
    }
            return P[0];                //default return

}
/* Similar to the function above, but here TutorialTA object corresponding to ID is returned  */
TutorialTA getTutorialTA(int ID,TutorialTA* T)
{
    for(int i=0;i<4;i++)
    {

        if(T[i].getID()==ID)
        {
            outfile<<"getTutorialTA() executed T["<<i<<"] got\n";
            return T[i];
        }
        else continue;
    }
        return T[0];

}
/* This class stores the elements which are objects of type Cell defined earlier. These elements form a 5*8 2D array
   and store information pertaining to each Cell of a timetable. Rows correspond to week days and columns correspond to
   the 8 subslots in particular day.(subslot 1 means 1st lecture in morning, and so on) */
class Timetable
{   public:
    Cell elements[5][8];
/* This function sets all cells of a slot of day to Cell c passed as argument. This cell contains information
   of lab slot to be assigned */
    void setLabCells(Cell c,int day,int slot)
    {
        for(int i=(slot-1)*4;i<(slot-1)*4+4;i++)
        {
        elements[day-1][i]=c;
        }
    }
/* Checks if a subslot of a day is occupied by lecture  */
    bool isOccupied(int day,int subSlot)
    {
        return elements[day-1][subSlot-1].isOccupied();
    }

    Timetable()           //default constructor
    {}


    /*
        Takes in day and slot as 0, 1 or 2(both,or forenoon or afternoon respectively), moves one by one
        sequentially through its cell elements of that slot and day, checking which slot is free.
        Allots first free slot to prof, marks that slot as booked and enters primary info
        of prof into that element. Returns false or true according to success of operation
    */

    bool setElement(int day, int slot, int ID,int profIndex, int division)
    {
        bool done=false;  //signifies if allotment of slot is done
        if(ID>PROF_ID_L&&ID<PROF_ID_U)      //checks if ID corresponds to Professor
        {
            if(slot==1)
        {
            for(int i=0;i<4;i++)
            {
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==P[profIndex].getID())
                {
                    break;            //to ensure same subject does not occur twice on the same day for a batch
                }
                if(elements[day-1][i].isOccupied()==false&&P[profIndex].checkIfFree(day,i+1)==true)  //if true, all element is set
                {
                    elements[day-1][i].setOccupied(ID);
                    P[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    P[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<P[profIndex].getID()<<"inside loop ";
                    P[profIndex].displayAllot();
                    break;
                }

            }
        }

        else if(slot==2)
        {
            for(int i=4;i<8;i++)
            {
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==P[profIndex].getID())
                {
                break;            //to ensure same subject does not occur twice on the same day for a batch HARD CONSTRAINT
                }
                if(elements[day-1][i].isOccupied()==false&&P[profIndex].checkIfFree(day,i+1)==true)
                {
                    elements[day-1][i].setOccupied(ID);
                    P[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    P[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<P[profIndex].getID()<<"inside loop ";
                    P[profIndex].displayAllot();
                    break;
                }
           }
        }

        if(slot==0||done==false)
        {
            for(int i=0;i<8;i++)
            {
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==P[profIndex].getID())
                {
                break;            //to ensure same subject does not occur twice on the same day for a batch HARD CONSTRAINT
                }
                if(elements[day-1][i].isOccupied()==false&&P[profIndex].checkIfFree(day,i+1)==true)
                {
                    elements[day-1][i].setOccupied(ID);
                    outfile<<day<<" "<<i<<" "<<"occupied\n";
                    P[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    P[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<P[profIndex].getID()<<"inside loop ";
                    P[profIndex].displayAllot();
                    break;
                }
            }
        }
            outfile<<"outside loop";
            P[profIndex].displayAllot();
            return done;
        }
        else               //if the ID passed as argument to function does not correspond to Professor, it corresponds to Tutorial TA
                            //Exactly same process of allotment to TA
        {
            if(slot==1)
            {
                for(int i=0;i<4;i++)
                {
                outfile<<"searching slot 1\n";
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==T[profIndex].getID())
                {
                outfile<<"Exited searching slot "<<slot<<" to avoid same tut \n";
                outfile<<T[profIndex].getID()<<" is ID of Tut TA\n";
                break;            //to ensure same subject does not occur twice on the same day for a batch HARD CONSTRAINT
                }
                if(elements[day-1][i].isOccupied()==false&&T[profIndex].checkIfFree(day,i+1)==true)
                {
                    elements[day-1][i].setOccupied(ID);
                    T[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    T[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<T[profIndex].getID()<<"inside loop ";
                    T[profIndex].displayAllot();
                    break;
                }

                }
            }

            else if(slot==2)
            {
                outfile<<"searching slot 2\n";
                for(int i=4;i<8;i++)
                {
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==T[profIndex].getID())
                {
                    outfile<<"Exited searching slot "<<slot<<" to avoid same tut \n";
                    outfile<<T[profIndex].getID()<<" is ID of Tut TA\n";
                    break;            //to ensure same subject does not occur twice on the same day for a batch HARD CONSTRAINT
                }
                if(elements[day-1][i].isOccupied()==false&&T[profIndex].checkIfFree(day,i+1)==true)
                {
                    elements[day-1][i].setOccupied(ID);
                    T[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    T[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<T[profIndex].getID()<<"inside loop ";
                    T[profIndex].displayAllot();
                    break;
                }
            }
            }
            if(slot==0||done==false)
            {
                outfile<<"searching both slot 1 and 2\n";
                for(int i=0;i<8;i++)
                {
                if(elements[day-1][i].isOccupied()==true&&elements[day-1][i].getTeacher_ID()==T[profIndex].getID())
                {
                outfile<<"Exited searching slot "<<slot<<" to avoid same tut \n";
                outfile<<T[profIndex].getID()<<" is ID of Tut TA\n";
                break;            //to ensure same subject does not occur twice on the same day for a batch HARD CONSTRAINT
                }
                if(elements[day-1][i].isOccupied()==false&&T[profIndex].checkIfFree(day,i+1)==true)
                {
                    elements[day-1][i].setOccupied(ID);
                    outfile<<day<<" "<<i<<" "<<"occupied\n";
                    T[profIndex].modifyAllotMatrix(day,(i/4)+1);
                    T[profIndex].modifyMyTimeTable(day,i+1,division);
                    done=true;
                    outfile<<T[profIndex].getID()<<"inside loop ";
                    T[profIndex].displayAllot();
                    break;
                }
                }
            }
            outfile<<"outside loop";
            T[profIndex].displayAllot();
            return done;

        }

     }

     /* Reserve lab slot according to institute rules for particular batch, does not affect optimization.
       In this function, entire slot of morning or afternoon is set Occupied so that lectures are not set in this slot*/
     void setLabSlot(int slot)
     {
        if(slot==1)
        {
            for(int i=0;i<5;i++)
            {
                for(int j=0;j<4;j++)
                {
                    elements[i][j].setOccupied();
                    elements[i][j].setSlotType('L');
                }
            }
        }
        if(slot==2)
        {
            for(int i=0;i<5;i++)
            {
                for(int j=4;j<8;j++)
                {
                    elements[i][j].setOccupied();
                    elements[i][j].setSlotType('L');
                }
            }
        }
     }


    /* setting particular element of TimeTable as occupied without checking if it was
        previously occupied,directly at the said day and subslot with given Teacher's ID
    */

    void absSetElement(int day,int subslot,int ID)
    {
        elements[day-1][subslot-1].setOccupied(ID);
    }
/* This function actually exchanges the schedule of 2 of the lectures in timetable, and makes corresponding changes to
   Allot matrices and timetable of individual Professor or Tutorial TA corresponding to exchange being done */


    void exchangeSlots(int day1,int day2,int subslot1,int subslot2,int profIndex1,int profIndex2,int slot,int division)
    {
        P[profIndex1].accomodateExchange(day1,day2,slot,subslot1,subslot2,division);
        P[profIndex2].accomodateExchange(day2,day1,slot,subslot2,subslot1,division);
        Cell temp;
        temp=elements[day1-1][subslot1-1];
        elements[day1-1][subslot1-1]=elements[day2-1][subslot2-1];
        elements[day2-1][subslot2-1]=temp;

    }
/* This functionover-rides the function just above, in case the subslot 2 with whom the current exchange of professor's lecture
   schedule is to be done, is not occupied by other Professor's lecture, or a tutorial (i.e. empty)  */
    void exchangeSlots(int day1,int day2,int subslot1,int subslot2,int profIndex,int slot,int division)
    {
        P[profIndex].accomodateExchange(day1,day2,slot,subslot1,subslot2,division);
        Cell temp;
        temp=elements[day1-1][subslot1-1];
        elements[day1-1][subslot1-1]=elements[day2-1][subslot2-1];
        elements[day2-1][subslot2-1]=temp;
    }
/* This function makes perturbations to the existing timetable by exchanging Cells of TimeTable so as to improve overall
   objective function */
    void perturbTimeTable(int slot,int division)
    {
        outfile<<"perturbation started\n";
        bool isExchangeDone=false;
        int i=0;   //counts number of iterations of while(true) loop

        while(true)
        {
            i++;
            if(i==5)     //stop executing while (true) loop once 5 large iterations are done-hope to considerably optimise after this number of iterations
            {
                break;
            }
            outfile<<"while true loop begins\n";
            int profIndex=findLessSatisfiedProf(1,P)-1;     //Professor with low satisfaction in terms of low objective function found
            int day1=P[profIndex].findImpactingAllotment(); //The allotment(day of allotment) which is reducing the individual objective
                                                             // function of professor is found
            int subSlot=P[profIndex].getSubSlot(day1,slot,division);//subslot number in which the professor teaches
                                                                    //particuar division is found. It is zero if professor
                                                                    //is not teaching that particular division on that day
                                                                    //and slot passed as arguments
            outfile<<"threshold for inner subSlot block, subSlot="<<subSlot<<endl;
            if(subSlot!=0)
            {
                outfile<<"into subSlot!=0 block\n";
                for(int j=1;j<3;j++)                                    //scanning through top 2 preferences of Professor P[i]
                {
                    outfile<<"into j loop number"<<j<<"\n";
                    int day2;
                    day2=P[profIndex].findNextMaxPrefDay(j);  //try exchanging less preferred day of professor with a day he prefers more
                    outfile<<endl<<"try exchanging professor "<<profIndex+1<<"from "<<day1<<" to "<<day2<<endl;
                     for(int k=(slot-1)*4;k<((slot-1)*4+4);k++)         //scanning subslots corresponding to slot of day2
                      {
                       outfile<<"into k loop number "<<k<<endl;
                        if(elements[day2-1][k].isOccupied()==true)       //if destination of exchange is already occupied
                        {
                           outfile<<"into block meant for exchanging prof to prof or TA\n";
                           int testID=elements[day2-1][k].getTeacher_ID();   //Take in ID of the faculty who has lecture scheduled
                                                                             //in the Cell being scanned
                           outfile<<"occupied slot by ID"<<testID<<endl;
                           if(P[profIndex].checkIfFree(day2,k+1)==0)         //checking if the Professor is actually free
                                                                              //on his preferred day at this particular subslot to take up
                             {
                               continue;
                             }
                           if(testID>PROF_ID_L&&testID<PROF_ID_U)  //checking if the subslot of exchange is occupied by another professor
                           {
                             outfile<<"probable exchange with prof\n";
                            if(isExchangeGood(elements[day1-1][subSlot-1],elements[day2-1][k],profIndex,testID-1001,day1,day2,slot)==true)//if exchanging actually improves
                                                                                                                                         //overall objective function
                            {
                                exchangeSlots(day1,day2,subSlot,k+1,profIndex,testID-1001,slot,division);
                                isExchangeDone=true;
                                outfile<<"Exchange "<<i<<" done\n";
                                break;
                            }
                            else     //in case exchanging does not improve objective function, do nothing
                            {
                                continue;
                            }

                            }
                            else   // in case the subslot is occupied already by a TutorialTA. Same process as above for this exchange too.
                            {
                             outfile<<"probable exchange with TA\n";

                            if(isExchangeGood(elements[day1-1][subSlot-1],elements[day2-1][k],profIndex,testID-101,day1,day2,slot)==true)//if exchanging actually improves
                                                                                                             //overall objective function
                            {
                                exchangeSlots(day1,day2,subSlot,k+1,profIndex,testID-101,slot,division);
                                isExchangeDone=true;
                                outfile<<"Exchange "<<i<<" done\n";
                                break;
                            }
                            else
                            {
                                continue;
                            }
                            }
                        }
                        else          //in case the destination of exchange is vacant
                        {
                            if(isExchangeGood(elements[day1-1][subSlot-1],elements[day2-1][k],profIndex,day1,day2,slot)==true)
                            {
                                exchangeSlots(day1,day2,subSlot,k+1,profIndex,slot,division);
                                isExchangeDone=true;
                                outfile<<"Exchange "<<i<<" done\n";
                                break;
                            }
                            else     //in case exchanging with the empty slot does not improve overall objesective function, do nothing
                            {
                                continue;
                            }
                        }

                    }
                    if(isExchangeDone==true)
                    {
                        break;                     // if exchange is done , we exit from the j loop at beginning of function which scans less
                                                   //satisfied professors' preferred day
                    }
                    else
                    {
                        continue;                     //if exchange is not successful, we go on to find the next most preferred day of
                                                       //professor in the next iteration of j loop
                    }

                }
            }
            else   //do nothing if subslot is 0 ,i.e. professor has not been alloted this particular division
            {

            }

        }
 }
};
/* This class has 5*2 2D matrix of Cell objects, which stores lab schedule of 1 division in a more efficient way,
    and helps handle lab perturbations better. Functions similar to those designed in class TimeTable are customised here  */
class LabSchedule
{
    Cell elements[5][2];
    public:
    Cell getCell(int day,int slot)
    {
        return elements[day-1][slot-1];
    }
/* Exchanging 2 lab slots */
    void exchangeSlots(int day1,int day2,int slot1,int slot2,int profIndex1,int profIndex2,LabTA * TA)
    {
        TA[profIndex1].accomodateExchange(day1,day2,slot1,slot2);
        TA[profIndex2].accomodateExchange(day2,day1,slot2,slot1);
        Cell temp;
        temp=elements[day1-1][slot1-1];
        elements[day1-1][slot1-1]=elements[day2-1][slot2-1];
        elements[day2-1][slot2-1]=temp;

    }
/* exchange slots in case 2nd slot of exchange is vacant */
    void exchangeSlots(int day1,int day2,int slot1,int slot2,int profIndex,LabTA * TA)
    {
        TA[profIndex].accomodateExchange(day1,day2,slot1,slot2);
        Cell temp;
        temp=elements[day1-1][slot1-1];
        elements[day1-1][slot1-1]=elements[day2-1][slot2-1];
        elements[day2-1][slot2-1]=temp;
    }
/*   cell c1 and Cell c2 are cells to be exchanged, day1 and day2 are days corresponding to c1 and c2;slot is the
     slot of TimeTable used in exchange  . Very similar to corresponding function in class timetable. Checks if a particular
     exchange actually improves total objective function */
    bool isExchangeGood(Cell c1,Cell c2,int P1Index,int P2Index,int day1,int day2,int slot1,int slot2,LabTA* TA)
    {
        LabTA P1=TA[P1Index];
        LabTA P2=TA[P2Index];
        outfile<<"into isexchange good function\n";
        if(P1.getID()!=P2.getID())            //checks if slots of same prof are not exchanged,ensures efficiency of code
        {
        outfile<<"TA ID's different.\n";
        if(P1.tryObjectiveFunction(day1,day2,slot1,slot2)+P2.tryObjectiveFunction(day2,day1,slot2,slot1)>P1.getObjFunction()+P2.getObjFunction())
        {
            outfile<<"Exchange good!\n";
            return true;
        }
        else
        {
            outfile<<"exchange not good\n";
            return false;
        }
        }
        else
        {
            outfile<<"exchange not good\n";
            return false;
        }
    }
/* used in case the destination of exchange is not
    occupied by a professor,i.e. vacant slot */
    bool isExchangeGood(Cell c1,Cell c2,int PIndex,int day1,int day2,int slot1,int slot2,LabTA* TA)
    {
        if(PIndex>300)  TA=CL;
        else            TA=PL;
        if(TA[PIndex].tryObjectiveFunction(day1,day2,slot1,slot2)>TA[PIndex].getObjFunction()&&checkOverAllotment(day2,slot2)==false)
        {
            outfile<<"Exchange Good!\n";
            return true;
        }
        else
        {
            outfile<<"exchange not good\n";
            return false;
        }
    }
/* Checks if there are more than 4 schedulings of lab slot in labschedule of 1 division .
   for example, if already 4 labs are scheduled in the morning slot(slot=1) of entire week labschedule together,
   this function returns true, as 1 division cannot have more than 4 lab sessions in the course structure*/
    bool checkOverAllotment(int day,int slot)
    {
        int elem_index;
        int num_allot=0;
        for(int i=0;i<4;i++)
        {
            elem_index=(day+i)%5;
            if(elements[elem_index][slot-1].isOccupied()==true)
            {
                outfile<<"element of index and slot"<<elem_index<<" "<<slot<<"occupied\n";
                num_allot++;
            }
        }
        if(num_allot==4)
        {
            outfile<<"lab not set due to overallotment\n";
            return true;
        }
        return false;
    }
/* This function perturbs a labschedule already formed, so as to improve overall objective function of all
   Lab TA's together . Very similar mechanism as that of perturbTimeTable() function in class timetable*/
    void perturbLabSchedule(LabTA * TA)
    {
        outfile<<"perturbation started\n";
        bool isExchangeDone=false;
        int i=0;   //counts number of iterations of while(true) loop

        while(true)
        {
            i++;
            if(i==5)     //stop executing while (true) loop once 5 large iterations are done-hope to considerably optimise after this number of iterations
            {
                break;
            }
            outfile<<"while true loop begins\n";
            int profIndex=findLessSatisfiedProf(1,TA)-1;
            int day1=TA[profIndex].findImpactingAllotment();
            int subSlot=TA[profIndex].getSubSlot(day1,0,1);  //returns 1 OR 5 OR 0,0 if prof does not teach that particular division in that slot and day
            int slot1=subSlot/4+1;                           //slot corresponding to the subslot calculated
            outfile<<"threshold for inner subSlot block, subSlot="<<subSlot<<endl;
            if(subSlot!=0)
            {
                outfile<<"into subSlot!=0 block\n";
                for(int j=1;j<3;j++)   //scanning through top 2 preferences of Professor P[i]
                {
                    outfile<<"into j loop number"<<j<<"\n";
                    int day2;
                    day2=TA[profIndex].findNextMaxPrefDay(j);
                    outfile<<endl<<"try exchanging TA "<<profIndex+1<<"from "<<day1<<" to "<<day2<<endl;
                     for(int k=0;k<2;k++)   //scanning subslots corresponding to slot of day2
                    {
                        int slot2=k+1;
                       outfile<<"into k loop number "<<k<<endl;
                        if(elements[day2-1][k].isOccupied()==true)   //if destination of exchange is already occupied
                        {
                           outfile<<"into block meant for exchanging TA to TA\n";
                           int testID=elements[day2-1][slot2-1].getTeacher_ID();
                           int Index;
                           if(testID>300)  Index=testID-301;
                           else            Index=testID-201;
                           outfile<<"occupied slot by ID"<<testID<<endl;
                           if(TA[profIndex].checkIfFree(day2,(slot2-1)*4+1)==0)
                             {
                               continue;
                             }

                            if(isExchangeGood(elements[day1-1][slot1-1],elements[day2-1][slot2-1],profIndex,Index,day1,day2,slot1,slot2,TA)==true)//if exchanging actually improves
                                                                                                             //overall objective function
                            {
                                exchangeSlots(day1,day2,slot1,slot2,profIndex,Index,TA);
                                isExchangeDone=true;
                                outfile<<"Exchange "<<i<<" done\n";
                                break;
                            }
                            else
                            {
                                continue;
                            }

                            }
                        else          //in case the destination of exchange is vacant
                        {
                            if(isExchangeGood(elements[day1-1][slot1-1],elements[day2-1][slot2-1],profIndex,day1,day2,slot1,slot2,TA)==true)
                            {
                                exchangeSlots(day1,day2,slot1,slot2,profIndex,TA);
                                isExchangeDone=true;
                                outfile<<"Exchange "<<i<<" done\n";
                                break;
                            }
                            else
                            {
                                continue;
                            }
                        }

                    }
                    if(isExchangeDone==true)
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }

            }
            else   //do nothing if subslot is 0 ,i.e. professor has not been alloted this particular division
            {

            }

       }
    }

/* This function creates the 1st labSchedule with considerable optimisation, which is to be perturbed later */
    bool setLabs(int day,int slot,int ID)
    {

        outfile<<"Into setLabs for day, slot and ID"<<day<<" "<<slot<<" "<<ID<<endl;
        int Index;
        LabTA* TA;
        if(ID<300)
        {
            Index=ID-201;
            TA=PL;
        }                                   //HIGHLY DEPENDENT ON THE WAY ID'S OF TA'S ARE ASSIGNED ID'S
        else
        {
            Index=ID-301;
            TA=CL;
        }
        if(slot!=0)
        {
        outfile<<"Into slot!=0 loop\n";
        if(elements[day-1][(slot-1)].isOccupied()==false&&TA[Index].checkIfFree(day,(slot-1)*4+1)==true&&checkOverAllotment(day,slot)==false)
            {
                 outfile<<"into regular check\n";
                 elements[day-1][(slot-1)].setOccupied(ID);
                 TA[Index].modifyAllotMatrix(day,slot);
                 TA[Index].modifyMyTimeTable(day,slot,1);
                 outfile<<TA[Index].getID()<<"inside loop ";
                 TA[Index].displayAllot();
                 return true;

            }
         else if(elements[day-1][(slot-1)].isOccupied()==true&&TA[Index].checkIfFree(day,(slot-1)*4+1)==true) //in case slot is occupied,
                                                                                                               //search in complementary
                                                                                                               //slot stored as tempslot
                                                                                                               //as follows
             {
              outfile<<"into tempslot check\n";
              int tempslot;
              if(slot==1)       tempslot=2;
              else if(slot==2)  tempslot=1;
              outfile<<"isoccupied "<<elements[day-1][(tempslot-1)].isOccupied()<<"is TA free "<<TA[Index].checkIfFree(day,(tempslot-1)*4)<<endl;

              if(elements[day-1][(tempslot-1)].isOccupied()==false&&TA[Index].checkIfFree(day,(tempslot-1)*4+1)==true&&checkOverAllotment(day,tempslot)==false)
                 {
                 elements[day-1][tempslot-1].setOccupied(ID);
                 TA[Index].modifyAllotMatrix(day,tempslot);
                 TA[Index].modifyMyTimeTable(day,tempslot,1);
                 outfile<<TA[Index].getID()<<"inside loop ";
                 TA[Index].displayAllot();
                 return true;
                 }
             }
        }
        if(slot==0)        //when bothe slot 1 and slot 2 are preferred by LabTA
        {
        outfile<<"into slot==0 block\n";
        if(elements[day-1][0].isOccupied()==false&&TA[Index].checkIfFree(day,1)==true&&checkOverAllotment(day,1)==false)
        {
            outfile<<"into 1st slot check\n";
            elements[day-1][0].setOccupied(ID);
            TA[Index].modifyAllotMatrix(day,1);
            TA[Index].modifyMyTimeTable(day,1,1);
            outfile<<TA[Index].getID()<<"inside loop ";
            TA[Index].displayAllot();
            return true;

        }
        else if(elements[day-1][1].isOccupied()==false&&TA[Index].checkIfFree(day,5)==true&&checkOverAllotment(day,2)==false)
        {
            outfile<<"into 2nd slot check\n";
            elements[day-1][1].setOccupied(ID);
            TA[Index].modifyAllotMatrix(day,2);
            TA[Index].modifyMyTimeTable(day,2,1);
            outfile<<TA[Index].getID()<<"inside loop ";
            TA[Index].displayAllot();
            return true;
        }
        }
        return false;
    }
};


Timetable TT[4];
LabSchedule P_Lab,C_Lab;
/*  To display if labschedules and timetables are occupied at day i+1 */
void displayLabSetStatus(int i)
{
    outfile<<TT[0].isOccupied(i+1,1)<<"for TT[0]\n";;
    outfile<<TT[1].isOccupied(i+1,1)<<"for TT[1]\n";
    outfile<<TT[2].isOccupied(i+1,5)<<"for TT[2]\n";
    outfile<<TT[3].isOccupied(i+1,5)<<"for TT[3]\n";
    outfile<<P_Lab.getCell(i+1,1).isOccupied()<<"status of P_LabgetCelloccupied slot 1\n";
    outfile<<C_Lab.getCell(i+1,1).isOccupied()<<"status of C_LabgetCelloccupied slot 1\n";
    outfile<<P_Lab.getCell(i+1,2).isOccupied()<<"status of P_LabgetCelloccupied slot 2\n";
    outfile<<C_Lab.getCell(i+1,2).isOccupied()<<"status of C_LabgetCelloccupied slot 2\n";
}
/* To integrate the contents of labSchedule object with TimeTable object after  the labSchedule object is optimised
   independently */
void integrateLabsAndTimeTable()
{
    outfile<<"into integratelabsandtimetable\n";
    outfile<<"Occupied Status of TT ";
    displayLabSetStatus(0);
    int counter1=0,counter2=0,counter3=0,counter4=0,counter5=0,counter6=0,counter7=0,counter8=0;//these 8 counters track number of
                                                                                                 //times particular lab is scheduled for a
                                                                                                 //particular Division
    for(int i=0;i<5;i++)
    {
        outfile<<"into i loop number"<<i<<"\n";
        if(P_Lab.getCell(i+1,1).isOccupied()==true&&counter1!=2&&TT[0].isOccupied(i+1,1)==false)
        {
            TT[0].setLabCells(P_Lab.getCell(i+1,1),i+1,1);
            outfile<<"TT[0].setLabCells(P_Lab.getCell(i+1,1),i+1,1);\n";

            counter1++;
        }
        else if(P_Lab.getCell(i+1,1).isOccupied()==true&&counter2!=2&&TT[1].isOccupied(i+1,1)==false)
        {
            TT[1].setLabCells(P_Lab.getCell(i+1,1),i+1,1);
            outfile<<"TT[1].setLabCells(P_Lab.getCell(i+1,1),i+1,1);\n";

            counter2++;
        }
        displayLabSetStatus(i);

        if(P_Lab.getCell(i+1,2).isOccupied()==true&&counter3!=2&&TT[2].isOccupied(i+1,5)==false)
        {
            TT[2].setLabCells(P_Lab.getCell(i+1,2),i+1,2);
            outfile<<"TT[2].setLabCells(P_Lab.getCell(i+1,2),i+1,2);\n";

            counter3++;
        }
        else if(P_Lab.getCell(i+1,2).isOccupied()==true&&counter4!=2&&TT[3].isOccupied(i+1,5)==false)
        {
            TT[3].setLabCells(P_Lab.getCell(i+1,2),i+1,2);
            outfile<<" TT[3].setLabCells(P_Lab.getCell(i+1,2),i+1,2)\n";

            counter4++;
         }
        displayLabSetStatus(i);
        if(C_Lab.getCell(i+1,1).isOccupied()==true&&counter5!=2&&TT[0].isOccupied(i+1,1)==false)
        {
            TT[0].setLabCells(C_Lab.getCell(i+1,1),i+1,1);
            outfile<<"TT[0].setLabCells(C_Lab.getCell(i+1,1),i+1,1)\n";

            counter5++;
        }
        else if(C_Lab.getCell(i+1,1).isOccupied()==true&&counter6!=2&&TT[1].isOccupied(i+1,1)==false)
        {
            TT[1].setLabCells(C_Lab.getCell(i+1,1),i+1,1);
            outfile<<"TT[1].setLabCells(C_Lab.getCell(i+1,1),i+1,1)\n";

            counter6++;
        }
        displayLabSetStatus(i);
        if(C_Lab.getCell(i+1,2).isOccupied()==true&&counter7!=2&&TT[2].isOccupied(i+1,5)==false)
        {
            TT[2].setLabCells(C_Lab.getCell(i+1,2),i+1,2);
            outfile<<"TT[2].setLabCells(C_Lab.getCell(i+1,2),i+1,2)\n";

            counter7++;
        }
        else if(C_Lab.getCell(i+1,2).isOccupied()==true&&counter8!=2&&TT[3].isOccupied(i+1,5)==false)
        {
            TT[3].setLabCells(C_Lab.getCell(i+1,2),i+1,2);
            outfile<<"TT[3].setLabCells(C_Lab.getCell(i+1,2),i+1,2)\n";

            counter8++;
        }
        displayLabSetStatus(i);
   }

}
/* This function 1st sets the primary slot-clash avoiding labSchedule before perturbation */
void setLabSchedule()
{
    outfile<<"into setlabschedule\n";
    for(int i=0;i<4;i++)
    {
        int counter1=0,counter2=0;
        for(int j=0;j<5;j++)
        {
            outfile<<"into j loop for CL "<<j<<endl;
            int day=CL[i].findNextMaxPrefDay(j+1);    //assigning labs according to preferences of LabTA's
            int slotPreference=CL[i].profSlotPreference(day);
            if(C_Lab.setLabs(day,slotPreference,CL[i].getID())==true)
            {
                outfile<<"CL["<<i<<"] got day and slot"<<day<<" "<<slotPreference<<endl;
                counter1++;
                outfile<<counter1<<"counter1"<<endl;
            }
            if(counter1==CL[i].getnumber_of_lectures())
            {
                outfile<<"exiting CL j loop\n";
                break;
            }

        }
        for(int j=0;j<5;j++)
        {
            outfile<<"into j loop "<<j<<endl;
            int day=PL[i].findNextMaxPrefDay(j+1);
            int slotPreference=PL[i].profSlotPreference(day);
            if(P_Lab.setLabs(day,slotPreference,PL[i].getID())==true)
            {
                outfile<<"PL["<<i<<"] got day and slot"<<day<<" "<<slotPreference<<endl;
                counter2++;
                outfile<<counter2<<"counter2\n";
            }
            if(counter2==PL[i].getnumber_of_lectures())
            {
                outfile<<"exiting j loop for PL\n";
                outfile<<"lecture number\n"<<PL[i].getnumber_of_lectures();
                break;
            }

        }
    }
}
/* This function makes primary timetable by considering preferences of professors sequentially. This timetable is to be optimised later*/
void makeTimeTable(Professor P[],LabTA PL[],LabTA CL[],TutorialTA T[],Timetable TT[])
{
        outfile<<"make timetable started\n";
                                //reserving lab forenoon lab slot to D1, D2 and afternoon lab slot to D3, D4 according to institute
                                //rules. To write a function to permute labslots in future, in class TimeTable
         setLabSchedule();
         integrateLabsAndTimeTable();
         TT[0].setLabSlot(1);
         TT[1].setLabSlot(1);
         TT[2].setLabSlot(2);
         TT[3].setLabSlot(2);
         outfile<<"labslots assigned\n";
        for(int i=0;i<10;i++)        //scanning prof inputs one by one and assigning values to cells in corresponding timetables.
        {
            for(int k=0;k<P[i].getnumber_of_lectures();k++)  //allots all number of lectures assigned to prof
            {
                for(int j=0;j<5;j++)                        //5 times iteration ensures that if lecture day corresponding
                                                             // to higher preference day of prof is not available, he
                                                               //atleast gets some day for his lecture
                {
                    int day=P[i].findNextMaxPrefDay(j+1);
                    int profSlotPreference=P[i].profSlotPreference(day);//Problem-time slots put in preference by prof is turning into hard constraint
                    outfile<<"day and profSlotPreference set loop variables"<<i<<" "<<k<<" "<<j<<endl;                                                                    //hope to alter slots in perturbations
                    if((TT[P[i].getDivision(1)-1].setElement(day,profSlotPreference,P[i].getID(),i,P[i].getDivision(1))==true))//sets division 1 lectures
                    {
                        outfile<<"Professor "<<i+1<<" alloted day"<<day<<"slot "<<profSlotPreference<<" division "<<P[i].getDivision(1)<<endl;
                        break;
                    }
                    else
                        continue;
                }
                for(int j=0;j<5;j++)
                {

                    int day=P[i].findNextMaxPrefDay(j+1);
                    int profSlotPreference=P[i].profSlotPreference(day);
                    if(TT[P[i].getDivision(2)-1].setElement(day,profSlotPreference,P[i].getID(),i,P[i].getDivision(2))==true)//sets division 2 lectures
                    {
                        outfile<<"Professor "<<i+1<<" alloted "<<day<<" "<<profSlotPreference<<" division "<<P[i].getDivision(2)<<endl;

                        break;
                    }
                    else
                        continue;
                }

            }
        }
  //same loop type to assign tuts
        for(int i=0;i<4;i++)
        {
            for(int k=0;k<T[i].getnumber_of_lectures();k++)  //allots all number of lectures assigned to prof
            {
                for(int j=0;j<5;j++)                        //5 times iteration ensures that if lecture day corresponding
                                                            // to higher preference day of prof is not available, he
                                                            //atleast gets some day for his lecture
                {
                    int day=T[i].findNextMaxPrefDay(j+1);
                    int profSlotPreference=T[i].profSlotPreference(day);
                    if((TT[T[i].getDivision(1)-1].setElement(day,profSlotPreference,T[i].getID(),i,T[i].getDivision(1))==true))//sets division 1 lectures
                    {
                        outfile<<"TutTA "<<i+1<<" assigned "<<day<<" "<<profSlotPreference<<" division "<<T[i].getDivision(1);
                        break;
                    }
                else
                    continue;
                }
                for(int j=0;j<5;j++)
                {

                int day=T[i].findNextMaxPrefDay(j+1);
                int profSlotPreference=T[i].profSlotPreference(day);
                if(TT[T[i].getDivision(2)-1].setElement(day,profSlotPreference,T[i].getID(),i,T[i].getDivision(2))==true)//sets lectures for 2nd division
                                                                                                                         //assigned to TA
                {
                    outfile<<"TutTA "<<i+1<<" assigned "<<day<<" "<<profSlotPreference<<" division "<<T[i].getDivision(2);
                    break;
                }
                else
                    continue;
                }

            }

        }
        outfile<<"TimeTables generated\n";

}

/* To display timetable in an output file */
void outfileTimetable(Timetable &T)
{
    for(int i=0;i<5;i++)
    {   outfile<<"\n"<<Day[i]<<"\n";
        for(int j=0;j<8;j++)
        {
            outfile<<"Slot "<<j+1<<"\t";
            T.elements[i][j].featureCell();
        }
    }
    outfile<<"displayed timetable\n";
}

/*
    This function is used to output the timetable in a tabular form on the console
*/

void console_output(int div)
{
        int j;
        cout<<"\n\n";
        cout<<setw(10)<<"Day"<<setw(15)<<"08:30-"<<setw(15)<<"09:30-"<<setw(15)<<"10:30-"<<setw(15)<<"11:30-"<<setw(15)<<"14:00-"<<setw(15)<<"15:00-"<<setw(15)<<"16:00-"<<setw(15)<<"17:00-"<<endl;
        cout<<setw(24)<<"09:30"<<setw(15)<<"10:30"<<setw(15)<<"11:30"<<setw(15)<<"14:00"<<setw(15)<<"15:00"<<setw(15)<<"16:00"<<setw(15)<<"17:00"<<setw(15)<<"18:00"<<endl;
        cout<<"\n\n";
        for(int i=0;i<5;i++)
        {   cout<<setw(10)<<Day[i];
            for(j=0;j<8;j++)
            {
                cout<<setw(15)<<TT[div-1].elements[i][j].getTeacher_ID();
            }
            //cout<<"\n"<<setw(24);
            //for(int k=0;k<24;k++)
            cout<<"\n"<<"          ";
            for(j=0;j<8;j++)
            {
                if(TT[div-1].elements[i][j].getTeacher_ID()>1000)
                cout<<setw(15)<<P[(TT[div-1].elements[i][j].getTeacher_ID())%1000-1].getSubject();

                else if(TT[div-1].elements[i][j].getTeacher_ID()>300)
                cout<<setw(15)<<"CH117L";

                else if(TT[div-1].elements[i][j].getTeacher_ID()>200)
                cout<<setw(15)<<"PH117L";

                else if(TT[div-1].elements[i][j].getTeacher_ID()>100)
                cout<<setw(15)<<T[(TT[div-1].elements[i][j].getTeacher_ID())%100-1].getSubject();

                else if(TT[div-1].elements[i][j].getTeacher_ID()==0)
                {
                cout<<setw(15)<<"-----";
                }
            }
            cout<<"\n\n";
        }


}
/*This function calls the tabular form of timetable for a particular division*/

void Timetable_per_division_Output()
{   system("cls");
    int n; bool repeat=true;
    cout<<"\n\n\tEnter the division whose timetable you want to see.";
    do
    {   cout<<"\n\t1. D1\n\t2. D2\n\t3. D3\n\t4. D4\n\t5. Exit\n\n\tYour choice:    ";
        cin>>n;
        switch(n)
        {
        case 1:
        case 2:
        case 3:
        case 4: {
                    console_output(n);
                    outfileTimetable(TT[n-1]);
                    cout<<"\n\n\tYou can view time-tables for other divisions as well or you may choose to exit.\n\n";
                    break;
                }
        case 5: {
                    cout<<"You have chosen to exit...\n\n\n\n\n\n\n";
                    exit(0);
                }
        default:{
                    cout<<"\nYou can only enter 1-5. Enter again.\n\n";

                }

        }



    } while(repeat);

}


/*INPUT: This function takes in data from user in the form of preference orders*/

void getData_from_user()
{
    int i;
    int pseudo_sub_ID, courseAllot[5]={0,0,0,0,0}, tuteAllot[2]={0,0}, labAllot[2]={0,0};

    //Input for the professors.
    for(i=0;i<10;i++)
    {   system("cls");
        cout<<"\nWelcome. This is the data input portal for Professors.\n\n";
        cout<<"\t\t\tProfessor  # "<<i+1;
        cout<<"\n\nEnter your choice of subject from the following:\n\t1. PH107\n\t2. CH107\n\t3. MA106\n\t4. IC102\n\t5. BB101";
        cout<<"\nEnter the serial number to choose a particular subject     ";

        cin>>pseudo_sub_ID;
        cout<<"\n\n";
        if((int)pseudo_sub_ID<1||(int)pseudo_sub_ID>5)
        {
        cout<<"Enter again. Rightly:";
        i--; continue;
        }
        if(courseAllot[pseudo_sub_ID-1]<2)
        {
            int prof_ID=1000+(i+1);
            P[i].getInput(pseudo_sub_ID, prof_ID, courseAllot[pseudo_sub_ID-1]);
            P[i].setID(prof_ID);
            courseAllot[pseudo_sub_ID-1]++;
            cout<<"\nEnter c to continue...";
            char c; cin>>c;
        }
        else
        {   char c;
            cout<<"That subject has been allotted. We are sorry, better luck next time!";
            cout<<"Enter c key to continue!";
            cin>>c;
            i--; continue;

        }
        if(i==11)
        {cout<<"Data has been successfully input for all the 10 profs.";}
    }

    //Input for Lab TAs
    for(i=0;i<8;i++)
    {   system("cls");
        cout<<"\n\nWelcome. This is the data input portal for Lab TAs.\n\n";
        cout<<"\n\t\t\t lab TA # "<<i+1;
        cout<<"\n\nEnter your choice of subject from the following:\n\t1. PH117\n\t2. CH117";
        cout<<"\nEnter the serial number to choose a particular lab subject     ";
        cin>>pseudo_sub_ID;
        cout<<"\n\n";
        if(pseudo_sub_ID<1||pseudo_sub_ID>2)
        {
        cout<<"Enter again. Rightly:";
        i--; continue;
        }

        if(labAllot[pseudo_sub_ID-1]<4)
        {
            int TA_ID;
            if(pseudo_sub_ID==1)
            {   TA_ID=200+(labAllot[pseudo_sub_ID-1]+1);
                PL[labAllot[1]].getInput(pseudo_sub_ID, TA_ID, labAllot[pseudo_sub_ID-1]);
                PL[labAllot[1]].setID(TA_ID);
            }
            else
            {   TA_ID=300+(labAllot[pseudo_sub_ID-1]+1);
                CL[labAllot[2]].getInput(pseudo_sub_ID, TA_ID, labAllot[pseudo_sub_ID-1]);
                CL[labAllot[2]].setID(TA_ID);
            }

            labAllot[pseudo_sub_ID-1]++;
            cout<<"\nEnter c key to continue...";
            char c; cin>>c;
        }
        else
        {   char c;
            cout<<"\nThat subject has been allotted. We are sorry, better luck next time!\n";
            cout<<"Enter c key to continue!";
            cin>>c;
            i--; continue;

        }
        if(i==7)
        {cout<<"Data has been successfully input for all the 8 lab TAs.";}
    }

    //Input for tute TAs
        for(i=0;i<4;i++)
    {   system("cls");
        cout<<"\n\nWelcome. This is the data input portal for tute TAs.\n\n";
        cout<<"\n\t\t\t Tute TA # "<<i+1;
        cout<<"\n\nEnter your choice of subject from the following:\n\t1. PH107\n\t2. MA106";
        cout<<"\nEnter the serial number to choose a particular tute subject    ";
        cin>>pseudo_sub_ID;
        cout<<"\n\n";
        if(pseudo_sub_ID<1||pseudo_sub_ID>2)
        {
        cout<<"Enter again. Rightly:";
        i--; continue;
        }

        if(tuteAllot[pseudo_sub_ID-1]<2)
        {   int TA_ID=100+(i+1);
            T[i].getInput(pseudo_sub_ID, TA_ID, tuteAllot[pseudo_sub_ID-1]);
            T[i].setID(TA_ID);
            tuteAllot[pseudo_sub_ID-1]++;
            cout<<"\nEnter c key to continue...";
            char c; cin>>c;
        }
        else
        {   char c;
            cout<<"\nThat subject has been allotted. We are sorry, better luck next time!\n";
            cout<<"Enter c key to continue!";
            cin>>c;
            i--; continue;
        }

        if(i==7)
        {cout<<"Data has been successfully input for all the 8 tute TAs.";}
    }

    cout<<"The input is complete.";
}
/*  INPUT: DEFAULT CASES.
    For ease of experimentation, verification and debugging, we have used default cases that adhere to the following guidelines.
    D1 D2 have morning labs, D3 and D4 afternoon labs. P[i] is alloted subject in the order Physics, Chemistry, Math, Bio, IC.
    This assumptions can be used without loss of generality.
*/



/*
    Code 1: All Profs have type 1 allotment.
    Code 2: Half the profs have Type1 allotment, and the other half have Type 2 allotment.
    Code 3: All profs have Type3 allotment.
    Code 4: Profs have a Type4 allotment with perm starting with something random.
*/

void create_default_set(int set_code)
{
    switch(set_code)
    {
    /*All profs/TAs have Type 1 preference, i.e. 54321 with morning slots.Division has been set as first 5 profs get*/
    case 1: {   for(int i=0;i<10;i++)//Profs
                {
                    P[i].setID(1001+i);
                    P[i].setSubject(i%5);
                    outfile<<P[i].getID()<<" ";
                    P[i].setPreferenceMatrixType1();
                }


                for(int i=0;i<4;i++)//Tutorial TAs
                {
                    T[i].setID(101+i);
                    T[i].setSubject(i%2);
                    outfile<<T[i].getID()<<" ";
                    T[i].setPreferenceMatrixType1();
                }

                //LabTAs
                for(int i=0;i<4;i++)
                {
                    PL[i].setID(201+i);
                    PL[i].setLabSubject(0);
                    outfile<<PL[i].getID()<<" ";
                    PL[i].setPreferenceMatrixType1();
                    PL[i].displayPreference();
                    PL[i].setnumber_of_lectures(2);
                    cout<<"Lab default values";
                }

                for(int i=0;i<4;i++)
                {
                    CL[i].setID(301+i);
                    CL[i].setLabSubject(1);
                    outfile<<CL[i].getID()<<" ";
                    CL[i].setPreferenceMatrixType1();
                    CL[i].setnumber_of_lectures(2);
                    outfile<<"lab default values";                }



                //Division Allotment

                for(int i=0;i<5;i++)
                {
                    P[i].setDivision(1,1);
                    P[i].setDivision(2,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                for(int i=5;i<10;i++)
                {
                    P[i].setDivision(3,1);
                    P[i].setDivision(4,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                 for(int i=0;i<4;i++)
                {
                    if(i<2)
                    {
                    T[i].setDivision(1,1);
                    T[i].setDivision(2,2);
                    }
                    if(i>1)
                    {
                    T[i].setDivision(3,1);
                    T[i].setDivision(4,2);
                    }
                    outfile<<T[i].getDivision(1)<<" "<<T[i].getDivision(2)<<endl;
                }

                break;

            }

    /*Half the profs have Type1 and other half have Type2.*/
    case 2: {   for(int i=0;i<5;i++)
                {
                    P[i].setID(1001+i);
                    P[i].setSubject(i%5);
                    outfile<<P[i].getID()<<" ";
                    P[i].setPreferenceMatrixType2();
                }

                for(int i=5;i<10;i++)
                {
                    P[i].setID(1001+i);
                    P[i].setSubject(i%5);
                    outfile<<P[i].getID()<<" ";
                    P[i].setPreferenceMatrixType1();
                }

                for(int i=0;i<2;i++)
                {
                    T[i].setID(101+i);
                    T[i].setSubject(i%2);
                    outfile<<T[i].getID()<<" ";
                    T[i].setPreferenceMatrixType2();
                }


                for(int i=2;i<4;i++)
                {
                    T[i].setID(101+i);
                    T[i].setSubject(i%2);
                    outfile<<T[i].getID()<<" ";
                    T[i].setPreferenceMatrixType1();
                }

                 for(int i=0;i<4;i++)
                {
                    PL[i].setID(201+i);
                    //cout<<PL[i].getID();
                    PL[i].setLabSubject(0);
                    outfile<<PL[i].getID()<<" ";
                    PL[i].setPreferenceMatrixType2();
                    PL[i].displayPreference();
                    PL[i].setnumber_of_lectures(2);
                    cout<<"Lab default values";
                }

                for(int i=0;i<4;i++)
                {
                    CL[i].setID(301+i);
                    CL[i].setLabSubject(1);
                    outfile<<CL[i].getID()<<" ";
                    CL[i].setPreferenceMatrixType2();
                    CL[i].setnumber_of_lectures(2);
                    outfile<<"lab default values";                }


                //LabTAs. to do.
                for(int i=0;i<5;i++)
                {
                    P[i].setDivision(1,1);
                    P[i].setDivision(2,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                for(int i=5;i<10;i++)
                {
                    P[i].setDivision(3,1);
                    P[i].setDivision(4,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                 for(int i=0;i<4;i++)
                {
                    if(i<2)
                    {
                    T[i].setDivision(1,1);
                    T[i].setDivision(2,2);
                    }
                    if(i>1)
                    {
                    T[i].setDivision(3,1);
                    T[i].setDivision(4,2);
                    }
                    outfile<<T[i].getDivision(1)<<" "<<T[i].getDivision(2)<<endl;
                }
                break;

            }

    /*All profs get Type3 pref allotted, i.e. */
    case 3: {   for(int i=0;i<10;i++)
                {
                    P[i].setID(1001+i);
                    P[i].setSubject(i%5);
                    outfile<<P[i].getID()<<" ";
                    P[i].setPreferenceMatrixType3();
                }

                for(int i=0;i<4;i++)
                {
                    T[i].setID(101+i);
                    T[i].setSubject(i%2);
                    outfile<<T[i].getID()<<" ";
                    T[i].setPreferenceMatrixType3();
                }

                //LabTAs. to do.
                for(int i=0;i<5;i++)
                {
                    P[i].setDivision(1,1);
                    P[i].setDivision(2,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                for(int i=5;i<10;i++)
                {
                    P[i].setDivision(3,1);
                    P[i].setDivision(4,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                                 for(int i=0;i<4;i++)
                {
                    PL[i].setID(201+i);
                    //cout<<PL[i].getID();
                    PL[i].setLabSubject(0);
                    outfile<<PL[i].getID()<<" ";
                    PL[i].setPreferenceMatrixType3();
                    PL[i].displayPreference();
                    PL[i].setnumber_of_lectures(2);
                    cout<<"Lab default values";
                }

                for(int i=0;i<4;i++)
                {
                    CL[i].setID(301+i);
                    CL[i].setLabSubject(1);
                    outfile<<CL[i].getID()<<" ";
                    CL[i].setPreferenceMatrixType3();
                    CL[i].setnumber_of_lectures(2);
                    outfile<<"lab default values";                }

                for(int i=0;i<4;i++)
                {
                    if(i<2)
                    {
                    T[i].setDivision(1,1);
                    T[i].setDivision(2,2);
                    }
                    if(i>1)
                    {
                    T[i].setDivision(3,1);
                    T[i].setDivision(4,2);
                    }
                    outfile<<T[i].getDivision(1)<<" "<<T[i].getDivision(2)<<endl;
                }
                break;

            }

    case 4: {   srand(time(NULL));

                for(int i=0;i<10;i++)
                {
                    P[i].setID(1001+i);
                    P[i].setSubject(i%5);
                    outfile<<P[i].getID()<<" ";
                    P[i].setPreferenceMatrixType4(rand()%100, (rand()%100)%2+1);
                }

                for(int i=0;i<4;i++)
                {
                    T[i].setID(101+i);
                    T[i].setSubject(i%2);
                    outfile<<T[i].getID()<<" ";
                    T[i].setPreferenceMatrixType4(rand()%100, (rand()%100)%2+1);
                }

                 for(int i=0;i<4;i++)
                {
                    PL[i].setID(201+i);
                    //cout<<PL[i].getID();
                    PL[i].setLabSubject(0);
                    outfile<<PL[i].getID()<<" ";
                    PL[i].setPreferenceMatrixType4(rand()%100, (rand()%100)%2+1);
                    PL[i].displayPreference();
                    PL[i].setnumber_of_lectures(2);
                    cout<<"Lab default values";
                }

                for(int i=0;i<4;i++)
                {
                    CL[i].setID(301+i);
                    CL[i].setLabSubject(1);
                    outfile<<CL[i].getID()<<" ";
                    CL[i].setPreferenceMatrixType4(rand()%100, (rand()%100)%2+1);
                    CL[i].setnumber_of_lectures(2);
                    outfile<<"lab default values";                }
                //LabTAs. to do.
                for(int i=0;i<5;i++)
                {
                    P[i].setDivision(1,1);
                    P[i].setDivision(2,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                for(int i=5;i<10;i++)
                {
                    P[i].setDivision(3,1);
                    P[i].setDivision(4,2);
                    outfile<<P[i].getDivision(1)<<" "<<P[i].getDivision(2)<<endl;
                }

                for(int i=0;i<4;i++)
                {
                    if(i<2)
                    {
                    T[i].setDivision(1,1);
                    T[i].setDivision(2,2);
                    }
                    if(i>1)
                    {
                    T[i].setDivision(3,1);
                    T[i].setDivision(4,2);
                    }
                    outfile<<T[i].getDivision(1)<<" "<<T[i].getDivision(2)<<endl;
                }
                break;

            }
    default: {cout<<"You have entered a wrong choice.";//this should be taken care of before entering the function
             }

    }
}

/*This gives the console window for choosing default sets*/

void default_cases()
{   system("cls");
    bool repeat=true;
    int Case;
    cout<<"\n\n\t\t\tDefault Sets (for easy review)\n\n"<<endl;
    cout<<"\n\tThe default cases are as follows:\n";
    cout<<"\n\t1.All the professors have a descending order of preference, with Monday being the most\n\t  preferred and Friday being the least, all of them preferring morning slots\n";
    cout<<"\n\t2.First Five professors will have their preference order as in case 1 and the \n\t  next five will have ascending order of preference without loss of generality,\n\t  that is Monday being the least preferred and Friday being the most, all of them preferring the afternoon slots\n";
    cout<<"\n\t3.All the professors have a descending order of preference, with Monday being the most preferred\n\t  and Friday being the least with them preferring alternate slots on consecutive days\n";
    cout<<"\n\t4.All the professors have a descending order of preference starting from a random day,\n\t  that is starting from any day from Monday to Friday, all preferring morning slots.\n";

    cout<<"\n\tYour choice:";
    cin>>Case;

    while(repeat)
    {
        switch(Case)
        {
        case 1:
        case 2:
        case 3:
        case 4: {   create_default_set(Case);
                    outfile<<"Input given.\n";
                    repeat=false;
                    break;
                }
        case 5: {   cout<<"\nYou have chosen to exit...\n\n\n\n\n\n\n";
                    exit(0);
                    break;
                }
        default:{   cout<<"\nYou can enter only 1-5. Enter again.";}
        }
    }
}

/*The input function that will be called in main()*/

void portal_input()
{   system("cls");
    bool repeat=true;
    int choose;
    cout<<"\n\n\t\t\tWelcome to the time-table optimisation portal!\n\n\n";
    do
    {

    cout<<"\n\t1.Default Cases(for checking without giving input)\n\t2.Enter professor's input manually.\n\t3.Exit\n\n\tYour choice: ";
    cin>>choose;
    switch(choose)
    {
        case 1: {   default_cases();
                    repeat=false;

                    break;
                }
        case 2: {   getData_from_user();
                    repeat=false;

                    break;
                }
        case 3: {   cout<<"\nYou have chosen to exit...\n\n\n\n\n\n\n";
                    exit(0);
                }
        default:{   cout<<"\nYou can enter only 1, 2 or 3. Please enter again.\n\n";

                }
    }
    }
    while(repeat);
}

/*Using perturbation to optimise the Timetable*/

void optimiseTimetable()
{
    cout<<"after make timetable";
    outfile<<TotalobjectiveFunction(P,T)<<"before perturbation"<<endl;
    for(int i=0;i<10;i++)
    {
        outfile<<"professor "<<i+1<<endl;
        P[i].displayPreference();
        P[i].displayMyTimeTable();
        outfile<<"Allot matrix of Prof"<<i+1<<endl;
        P[i].displayAllot();
        outfile<<"my obj function"<<P[i].getObjFunction()<<endl;
    }
    for(int i=0;i<4;i++)
    {
        outfile<<"TutorialTA "<<i+1<<endl;
        T[i].displayPreference();
        T[i].displayMyTimeTable();
        outfile<<"Allot matrix of Prof"<<i<<endl;
        T[i].displayAllot();
        outfile<<"my obj function"<<T[i].getObjFunction()<<endl;

    }


    for(int i=0;i<4;i++)
    {
        if(i<2)
        TT[i].perturbTimeTable(2,i+1);
        else
        TT[i].perturbTimeTable(1,i+1);
    }
    for(int i=0;i<10;i++)
    {
        P[i].displayAllot();
    }
        outfile<<TotalobjectiveFunction(P,T)<<"after perurbation"<<endl;
        for(int i=0;i<10;i++)
    {
        P[i].displayMyTimeTable();
    }




}



int main()
{

    portal_input();
    makeTimeTable(P,PL,CL,T,TT);
    optimiseTimetable();
    Timetable_per_division_Output();

}
