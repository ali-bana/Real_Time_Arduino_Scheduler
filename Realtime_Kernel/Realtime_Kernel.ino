#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avrcontext_arduino.h>


enum state{
  FINISHED=0,
  PROGRESS=1
};

typedef  struct{
  int instance_num;
  char* t_name; //points to string containing the task name. used for debugging purposes.
  int T; //period of task. if T==-1 task is aperiodic.
  int ttn; //time to next instance of the task. Also time to deadline
  state stat;
  void (*funcp)(void *); //pointer to the process function.
  void* stack;
  avr_context_t volatile *cntx; //context of the task

}task;

////////variables that we need

task* tasks = malloc(1);
size_t tasks_size_byte = 0;
avr_context_t *volatile current_cntx;
static int taskNum = 0; //total number of tasks
static int current_task = -1; // index of current task
static avr_context_t scheduler_cntx;
static uint8_t scheduler_stack[128];// scheduler stack
static avr_context_t waiting_cntx;
static uint8_t waiting_stack[128];
///////////////////////////

void addTask(void (*funcp)(void *), int T, char* t_name) //add a new task to the array
{
  tasks_size_byte += sizeof(task);
  // Check the following line. It may be a source of error because it never didn't initialized to be casted to sth.
  tasks = realloc(tasks, tasks_size_byte * sizeof(task));
  
  tasks[taskNum].t_name = t_name;
  tasks[taskNum].T = T;
  tasks[taskNum].ttn = T;
  tasks[taskNum].cntx = malloc(sizeof(avr_context_t));
  tasks[taskNum].funcp = funcp;
  tasks[taskNum].stat = FINISHED;
  tasks[taskNum].instance_num = 0;
  tasks[taskNum].stack = malloc(20*sizeof(int));
  taskNum++;
  // TODO
}

int compare(const void *s1, const void *s2)
{
   task *e1 = (task *)s1;
   task *e2 = (task *)s2;
  return e1->T - e2->T; // or vice versa?
}


void sortTasks() //sort the tasks from shortest t to the longest
{
  //dooble check the arguments
  qsort(tasks, (int) tasks_size_byte / sizeof(task), sizeof(task), compare);
  // TODO
}


void func1()
{
  int a = 1;
  Serial.println(9);
  Serial.println(9);
  Serial.println(9);
  Serial.println(9);
  Serial.println(9);
  return;
}

void func2()
{
  int a = 1;
  Serial.println(8);
  Serial.println(8);
  Serial.println(8);
  Serial.println(8);
  Serial.println(8);
  return;
}

void func3()
{
  int a = 1;
  Serial.println("task1");
  return;
}

//// scheduler function


void scheduler_function(void) __attribute__((naked));
void scheduler_function(void){
    wdt_disable();
    if (current_task != -1)
      tasks[current_task].stat = FINISHED;
    scheduler();
//    WDTCSR |= 1 << WDIE; 
    start_system_timer();
    AVR_RESTORE_CONTEXT_GLOBAL_POINTER(current_cntx);
    asm volatile("ret\n");
}

void scheduler_function2(void)
{
  wdt_disable();
  if (current_task != -1)
    tasks[current_task].stat = FINISHED;
  scheduler();
  start_system_timer();
  avr_setcontext(current_cntx);
}



static void scheduler()
{
  delay(10);
  Serial.println(6);
  // update task status
  for(int i=0; i<taskNum; i++)
    {
      
      if (millis() >= tasks[i].T * tasks[i].instance_num)
        {
          //new task arrives
            avr_getcontext(tasks[i].cntx);
            avr_makecontext(tasks[i].cntx,
                    (void*)tasks[i].stack, sizeof(tasks[i].stack),
                    &scheduler_cntx,
                    tasks[i].funcp, NULL);
            tasks[i].instance_num = tasks[i].instance_num + 1;
            tasks[i].stat = PROGRESS;
        }
    }
  //select an unfinished task to continue
  for(int i=0; i<taskNum; i++)
  {
    if(tasks[i].stat == PROGRESS)
    {
      current_task = i;
      current_cntx = tasks[i].cntx;
      return;
    }
  }
  // no context found
  current_task = -1;
  current_cntx = &waiting_cntx;
}

void busy_waiting()
{

  while(1)
  {
    Serial.println(1);
    delay(1000);
   }
}



void start_system_timer(void)
{
    cli(); // disable interrupts
    MCUSR &= ~(1<<WDRF);
    wdt_reset(); // reset watchdog timer
    // configure WD timer
    WDTCSR |= 1 << WDCE | 1 << WDE; // enable WD timer configuration mode
    WDTCSR = 0; // reset WD timer
    wdt_enable(WDTO_1S); // configure period
    WDTCSR |= 1 << WDIE; // use WD timer in interrupt mode
    sei(); // enable interrupts
}



void setup() {
  Serial.begin(19200);
  delay(100);
  Serial.println(3);
  delay(100);
  //making scheduler context so we will return to the scheduler after finishing
  avr_getcontext(&scheduler_cntx);
  avr_makecontext(&scheduler_cntx,
                    (void*)&scheduler_stack[0], sizeof(scheduler_stack),
                    &scheduler_cntx,
                    scheduler_function2, NULL);
  // adding waiting context
  avr_getcontext(&waiting_cntx);
  avr_makecontext(&waiting_cntx,
                    (void*)&waiting_stack[0], sizeof(waiting_stack),
                    &waiting_cntx,
                    busy_waiting, NULL);
  // add yer tasks here
  addTask(func1, 4000, "task1");
  addTask(func2, 3000, "task2");
//  addTask(func1, 12, "task3");
//  addTask(func1, 20, "task4");
//  addTask(func1, 2, "task5");
//  addTask(func1, 1, "task6");
//  addTask(func1, 30, "task7");
//  addTask(func1, 15, "task8");
  // sorting the tasks by their periods.
  
  sortTasks();
  start_system_timer();
  current_cntx = &waiting_cntx;
  avr_setcontext(&scheduler_cntx);
//  busy_waiting();
}

void loop() {
  delay(1000);
}


ISR(WDT_vect, ISR_NAKED)
{
  //saving the current state
  if (current_task != -1)
    AVR_SAVE_CONTEXT_GLOBAL_POINTER(
        "cli\n",
        current_cntx);
    wdt_disable();
    scheduler();
//    WDTCSR |= 1 << WDIE; 
    start_system_timer();
    AVR_RESTORE_CONTEXT_GLOBAL_POINTER(current_cntx);
    asm volatile("reti\n");
    
    
}
