#include <avr/wdt.h>
#include <avr/sleep.h>

enum state{
  finished=0,
  in_progress=1,
};
struct task{
  char* t_name; //points to string containing the task name. used for debugging purposes.
  int T; //period of task. if T==-1 task is aperiodic.
  int ttn; //time to next instance of the task. Also time to deadline
  avr_context_t cntx; //context of the task
  void (*funcp)(void *); //pointer to the process function. 
}task;

////////variables that we need

task* tasks;
avr_context_t *volatile current;
///////////////////////////

void addTask() //add a new task to the array
{
  // TODO
}

void sortTasks() //sort the tasks from shortest t to the longest
{
  // TODO
}












void setup() {
  // put your setup code here, to run once

}

void loop() {
  // put your main code here, to run repeatedly:

}
