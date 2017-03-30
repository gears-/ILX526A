Timing Requirements
-----------------
The requirements are outlined in the ILX526A documentation. For the sake of completeness, I measured the signals coming out of the Uno.
A CCD clock period of 1 microsecond is assumed.

| Requirement | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| ROG - CLK | t5 | 125 | 250 | 375 | 249 | ns |
| ROG - CLK | t9 | 125 | 250 | 375 | 251 | ns |
| ROG Rise, Fall | t6, t8 | 0 | 10 | 100 | ~4 | ns |
| ROG Pulse width | t7 | 6 | 10 | 20 | 10 | uS |
---------------------------------
| ROG - CLK | t5 | 125 | 250 | 375 | 249 | ns |
| ROG - CLK | t9 | 125 | 250 | 375 | 251 | ns |
| ROG Rise, Fall | t6, t8 | 0 | 10 | 100 | ~4 | ns |
| ROG Pulse width | t7 | 6 | 10 | 20 | 10 | uS |
---------------------------------
