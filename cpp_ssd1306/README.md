
### Example

Very basic test: flashing text!

```
#ifdef __cplusplus
  extern "C"
  {
#endif

#include "ssd1306.hpp"
#include <sstream>

  void test()
  {
    ssd1306::Font3x5 xs_font;
    ssd1306::Display oled;
    oled.init();

    while(true)
    {
      std::stringstream msg("TEST");
      oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
      HAL_Delay(1000);

      std::stringstream msg("");
      oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
      HAL_Delay(1000);
    }
  }

#ifdef __cplusplus
  }
#endif


```
