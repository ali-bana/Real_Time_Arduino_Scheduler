#include <avr/wdt.h>
#include <avr/sleep.h>

#include <avrcontext_arduino.h>

//// Global variables

extern "C" {
avr_context_t *volatile current_task_ctx; // current task context
}
static size_t current_task_num; // current task index
static avr_context_t dummy_ctx; // never going to be used
static avr_context_t tasks[2]; // contexts for tasks
static uint8_t disabler_stack[128]; // stack for the disabler_task


static void disabler_task1(void *)
{

  int a = 0;
    for (;;)
    {
      a += 1;
       Serial.println(a);
       a = a % 100;
    }
}

static void disabler_task2(void *)
{
  int b = 1000;
    for (;;)
    {
      b += 1;
       Serial.println(b);
       b = b % 100 + 1000;
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
    wdt_enable(WDTO_120MS); // configure period
    WDTCSR |= 1 << WDIE; // use WD timer in interrupt mode
    sei(); // enable interrupts
}

void setup(void)
{
    // Enable builtin LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.begin(19200);
    Serial.println("ghiiirrrr\n\n\n");

  
     

    
    current_task_num = 0;
    current_task_ctx = &tasks[0];
    
    start_system_timer();
    Serial.println("kiiiiirrrrrr");
    Serial.println(WDTCSR);
    avr_getcontext(&dummy_ctx);
    avr_getcontext(&tasks[0]);
    avr_getcontext(&tasks[1]);
    avr_makecontext(&tasks[0],
                    (void*)&disabler_stack[0], sizeof(disabler_stack),
                    &dummy_ctx,
                    disabler_task1, NULL);
                    
    avr_makecontext(&tasks[1],
                    (void*)&disabler_stack[0], sizeof(disabler_stack),
                    &dummy_ctx,
                    disabler_task2, NULL);
                    
    //start_system_timer();
    //////////////////////////////////////////////////////////////////
    //isabler_task1((void*)&disabler_stack[0]); // If I comment this line and uncomment the following line the WDT will reset the whole cpu after second task. But I think it should not.
   avr_setcontext(current_task_ctx); //this line must act like the above line
   //////////////////////////////////////////////////////////////////
    
   
}

void loop(void)
{
    Serial.println("......");
}

static void switch_task(void)
{
    current_task_num = current_task_num == 0 ? 1 : 0;
    current_task_ctx = &tasks[current_task_num];
}


ISR(WDT_vect, ISR_NAKED)
{
  Serial.println("\n we are in interrupt shit \n\n");
    // save the context of the current task
    AVR_SAVE_CONTEXT_GLOBAL_POINTER(
        "cli\n", // disable interrupts during task switching
        current_task_ctx);
    switch_task(); // switch to the other task.
    WDTCSR |= 1 << WDIE; // re-enable watchdog timer interrupts to avoid reset
    // restore the context of the task to which we have just switched.
    AVR_RESTORE_CONTEXT_GLOBAL_POINTER(current_task_ctx);
    asm volatile("reti\n"); // return from the interrupt and activate the restored context.
}
