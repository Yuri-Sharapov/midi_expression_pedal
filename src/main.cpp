#include <Arduino.h>

#define DEBUG_MODE  false

// Assign each button to a port. 
const int expPin    = A0;
const int CCNum     = 1;

int currentPedalVal = 0;
int currentPedalValN = 0;

void sendNote(int cmd, int pitch, int velocity)
{
    Serial1.write(cmd);      // Defines message as "Note on" over specified channel
    Serial1.write(pitch);    // Defines pitch
    Serial1.write(velocity); // Defines velocity
}

void sendControlChange(int val)
{
    Serial1.write(176);      // Defines message as Control Change
    Serial1.write(CCNum);    // Defines CC number
    Serial1.write(val);      // Defines value
}


void updateExpressionPedal(void)
{
    static int newExpVal    = 0;
    static int lastExpVal   = 0;

    newExpVal = analogRead(expPin);
    newExpVal = map(newExpVal, 93, 1023, 0, 1023);
    currentPedalVal = newExpVal;
    
    newExpVal = map(newExpVal, 0, 1023, 0, 127);
    currentPedalValN = newExpVal;

    newExpVal = constrain(newExpVal, 0, 127);
    if (newExpVal != lastExpVal)
    {
#if (DEBUG_MODE == false)        
        sendControlChange(127 - newExpVal);
#endif        
    }
    lastExpVal = newExpVal;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    // Set MIDI baud rate:
#if (DEBUG_MODE == false)
    Serial1.begin(31250);
#else
    Serial.begin(115200);
#endif
}

void loop()
{
    static unsigned long timeToUpdateExpressionMs   = 0;
    static unsigned long timeToUpdateMonitorMs      = 0;
    static unsigned long timeToUpdateLed            = 0;

    unsigned long currentTimeMs = millis();

    if (currentTimeMs - timeToUpdateExpressionMs > 10)
    {
        timeToUpdateExpressionMs = currentTimeMs;
        // PROCESS EXPRESSION PEDAL...  
        updateExpressionPedal();
    }

#if (DEBUG_MODE == true)    
    if (currentTimeMs - timeToUpdateMonitorMs > 200)
    {
        timeToUpdateMonitorMs = currentTimeMs;
        Serial.print("val: ");
        Serial.print(currentPedalVal, DEC);
        Serial.print(" ");
        Serial.print(currentPedalValN, DEC);
        Serial.println();
    }
#endif

    if (currentTimeMs - timeToUpdateLed > 500)
    {
        timeToUpdateLed = currentTimeMs;

        if (digitalRead(LED_BUILTIN))
            digitalWrite(LED_BUILTIN, LOW);
        else
            digitalWrite(LED_BUILTIN, HIGH);
    }
}

