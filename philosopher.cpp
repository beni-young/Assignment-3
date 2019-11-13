/* Philosophers is going to 4
   Forks is going to be 4 on table
   Philosophers is going to be P0, P1, P2, P3
   Forks is going to be F0, F1, F2, F3
   4 different threads for the 4 Philosophers
   Each Philosopher is going to be Thinking and Eating alternatively
   Think state lasts constant 10 ms
   Eating state lasts for random time 10 ms - 40 ms

   Each fork protected by a mutex
   Whenever a philosopher wants to eat, they try picking up a fork to their left (P3 picks up F3 first)
   If available, the philosopher will pick it up.

   If left fork is not available, the philosopher will sleep (random time 50 ms - 100 ms)
   if available the philosopher will pick up the left fork. If not, go to sleep.

   If right fork is available, the philosopher will pick it up. 
   If not, the philosopher will drop the left fork, sleep (random time 50 ms - 100 ms) then tries to pick up the fork to their left

   After pickup up both forks, the philosopher should sleep (randdom time 10 ms - 40 ms) to simulate eating

   Whenever a philospher wants to think, they can think right away.
   Simulate this by the philosopher thread should sleep for 10 ms.

   "waiting to eat" state as "hungry" state.

   Each philosopher tracks how long they are in hungry / eating / thinking states.

   Simulate scenario for 60 seconds
*/

#include <iostream>
#include <string>

using namespace std;

const int MAX = 4;

class Fork
{
    public:
    Fork(int);
    int position[MAX];
    int left, right;
    

    private:
    bool pickup;
};

Fork::Fork(int f_id)
{
    left = f_id;

    if(f_id = MAX)
        right = f_id - MAX;
    else
        right = f_id + 1;      
    
}

class Philosopher 
{
public:
Philosopher(int number = 0) { p_id = number;};
void Thinking();
void Hungry();
int Test();
void State();

private:
int p_id;
Fork forks(int p_id); // setup to have left and right fork ID by Philosopher eg. P3 picks up F3 (left fork) and tries to pickup F0 (right fork)
void sleep_fork(int left, int right, int both);

};


int Philosopher::Test()
{
    return forks.left();
}



int main()
{
    Philosopher P0(3);

    cout << P0.Test() << endl;
    //cout << "didnt work" << endl;

    return 0;
}