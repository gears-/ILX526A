Timing Requirements
-----------------
The requirements are outlined in the ILX526A documentation. For the sake of completeness, the signals coming out of the Uno were measured. The pictures of the scope traces are in their respective folders. The color scheme is as follows
* Channel 1 (yellow): ROG
* Channel 2 (cyan): SHUT
* Channel 3 (magenta): CCD CLK
* Channel 4 (green): ADC CLK

For the tables below, a CCD clock period of 1 microsecond is assumed (1 MHz freq.)

| ROG Requirement | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| ROG - CLK | t5 | 125 | 250 | 375 | 249 | ns |
| ROG - CLK | t9 | 125 | 250 | 375 | 251 | ns |
| ROG Rise, Fall | t6, t8 | 0 | 10 | 100 | ~4 | ns |
| ROG Pulse width | t7 | 6 | 10 | 20 | 10 | uS |
---------------------------------
| SHUT Requirement | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| SHUT - CLK | t14 | 150 | 200 | 250 | 190| ns |
| SHUT - CLK | t15 | 150 | 200 | 250 | 190| ns |
| SHUT Rise, Fall | t11, t13 | 0 | 10 | 100 | 3-5 | ns |
| SHUT Pulse width | t12 | 4 | 5 | --- | 5 | uS |
---------------------------------

The requirement to have ROG to SHUT be more than 10 uS is automatically fulfilled by waiting for all the pixels to be grabbed from the CCD.
The ROG period is derived from the clock timing on pp.4-5 in the documentation
> 3100 or more clock pulses are required  

This simply means that the minimum period at 1 MHz is going to be 3.1 ms (or 3,100 uS). 

| Other Requirements | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| SHUT to ROG | t16 | 10 | --- | --- | 34 | uS |
| ROG period | --- | 3100 | --- | --- | 3112 | uS |


