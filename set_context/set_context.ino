#include <avr/wdt.h>
#include <avr/sleep.h>

#include <avrcontext_arduino.h>

//// Global variables

extern "C" {
avr_context_t *volatile current_task_ctx; // current task context
}
static avr_context_t ctx; // never going to be used
static uint8_t stack[128]; // stack for the disabler_task


static void disabler_task(void *)
{
    for (;;)
    {
      Serial.println("hi");
        delay(2000);
    }
}




void setup(void)
{
    // Enable builtin LED
    Serial.begin(19200);
    Serial.println("begining");
    
    // Initialise the second task.
    //
    // This task starts execution on the first system timer tick.
    avr_getcontext(&ctx);
    avr_makecontext(&ctx,
                    (void*)&stack[0], sizeof(stack),
                    &ctx,
                    disabler_task, NULL);
    // start scheduling
    avr_setcontext(&ctx);
    // after returning from this function
    // loop() gets executed (as usual).
}

// This code tries to enable built-in LED (forever).
void loop(void)
{
    digitalWrite(LED_BUILTIN, HIGH);
}
