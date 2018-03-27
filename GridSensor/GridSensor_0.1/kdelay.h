/*
alternative delay function for KONNEKTING Library
this delay function doesn't block Knx.task(), so use can still receive KNX telegramms
use it as a normal delay: kdelay(x), where x are milliseconds
*/

void kdelay(unsigned long ms)
{
    uint32_t start = micros();
    
    while (ms > 0) {
        Knx.task();
        while ( ms > 0 && (micros() - start) >= 1000) {
            ms--;
            start += 1000;
        }
    }
}
