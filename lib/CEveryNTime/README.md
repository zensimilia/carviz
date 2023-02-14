# Some helpers to perform tasks periodically

This code comes from the [FastLED](https://github.com/FastLED/FastLED) library by Daniel Garcia and Mark Kriegsman.
Ported by Marc Miller, [May 2020](https://gist.github.com/marmilicious/43cda5561971f410753e3cd050e0e4c7).

By including this file all the `EVERY_N_` timer functions from the FastLED library can be used without including the entire library. Please see the original FastLED's [lib8tion.h](https://github.com/FastLED/FastLED/blob/master/src/lib8tion.h) file
for more details and comments.

This mechanism does not use interrupts, and has to be executed periodically to work, i.e. as shown in the “loop” function above. It uses 4 bytes of RAM for each independent ‘Every N Seconds’ timer; you can have as many different ones as you wish.

## Example

There are a few variations, including EVERY_N_MILLISECONDS.

```cpp
#include <CEveryNTime.h>

void loop() {
  ...
  EVERY_N_MILLISECONDS( <mseconds> ) {
    <your timed code here>;
  }

  EVERY_N_SECONDS( <seconds> ) {
    <your timed code here>;
  }
}
```

You can also use these periodic triggers like this, without the helper macros. You have to declare a static instance of the ‘Every N Seconds’ class, initialize it with the period you want, and then you can just test it with an “if” statement to see if the period has come around again.

```cpp
#include <CEveryNTime.h>

void loop() {
  static CEveryNSeconds newPatternTime( 10 );
  ...
  if ( newPatternTime ) {
    <your timed code here>;
  }
}
```

## Macros

| Macro                | Param          | Alias          |
| -------------------- | -------------- | -------------- |
| EVERY_N_MILLISECONDS | mseconds\<int> | EVERY_N_MILLIS |
| EVERY_N_SECONDS      | seconds\<int>  |                |
| EVERY_N_BSECONDS     | bseconds\<int> |                |
| EVERY_N_MINUTES      | minutes\<int>  |                |
| EVERY_N_HOURS        | hours\<int>    |                |

## Public Member Functions

|     Type | Functtion                    |
| -------: | ---------------------------- |
|    class | CEveryNTime(timeType period) |
|     void | setPeriod(timeType period)   |
| timeType | getTime()                    |
| timeType | getPeriod()                  |
| timeType | getElapsed()                 |
| timeType | getRemaining()               |
| timeType | getLastTriggerTime()         |
|     bool | ready()                      |
|     void | reset()                      |
|     void | trigger()                    |
| operator | bool()                       |

## Public Attributes

|     Type | Attribute    |
| -------: | ------------ |
| timeType | mPrevTrigger |
| timeType | mPeriod      |
