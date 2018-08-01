// wdtimer.h:
// Hardware abstraction module: Watchdog timer

#ifndef WDTIMER_H_
#define WDTIMER_H_

// Watchdog timer controls:
void Watchdog_init(void);
void Watchdog_hit(void);
void Watchdog_disable(void);

#endif /* WDTIMER_H_ */
