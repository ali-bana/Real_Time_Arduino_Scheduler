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
size_t tasks_size_byte = 0;
avr_context_t *volatile current;
///////////////////////////

void addTask(avr_context_func_t funcp, int T, char* t_name) //add a new task to the array
{
  tasks_size_byte += sizeof(task);
  // Check the following line. It may be a source of error because it never didn't initialized to be casted to sth.
  tasks = realloc(tasks, tasks_size_byte * sizeof(task));
  
  int last_index = (int) tasks_size_byte / sizeof(task); // does it need +1?
  tasks[last_index].t_name = t_name;
  tasks[last_index].T = T;
  tasks[last_index].ttn = T;
  tasks[last_index].cntx = NULL;
  tasks[last_index].funcp = funcp;
  // TODO
}

int compare(const void *s1, const void *s2)
{
  struct task *e1 = (struct task *)s1;
  struct task *e2 = (struct task *)s2;
  return e1->T - e2->id; // or vice versa?
}


void sortTasks() //sort the tasks from shortest t to the longest
{
  //dooble check the arguments
  qsort(tasks, (int) tasks_size_byte / sizeof(task), sizeof(taske), compare);
  // TODO
}












void setup() {
  // put your setup code here, to run once

}

void loop() {
  // put your main code here, to run repeatedly:

}
