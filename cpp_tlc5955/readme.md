
Simple example:

```
#include <tlc5955.hpp>

void main()
{
  tlc5955::Driver leds;
  leds.set_function_data(false, false, true, true, true);
  leds.send_control_data();


  while(true)
  {
    leds.send_grayscale_data();
    HAL_Delay(1000);
  }
}
```
