Timing Requirements
-----------------
The requirements are outlined in the ILX526A documentation. For the sake of completeness, the signals coming out of the Teensy were measured. The pictures of the scope traces are in their respective folders. The color scheme is as follows
* Channel 1 (yellow): ROG
* Channel 2 (cyan): SHUT
* Channel 3 (magenta): CCD CLK
* Channel 4 (green): ADC CLK

For the tables below, a CCD clock period of 1 microsecond is assumed (1 MHz freq.)

| CLK Requirement | Symbol | Min. | Typ. | Max. | Measured | Units | 
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| CLK Rise | t1 | 0 | 10 | 100 | 2.3 - 2.7 | ns |
| CLK Fall | t2 | 0 | 10 | 100 | 1.9 - 2.2 | ns |

---------------------------------


| ROG Requirement | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| ROG - CLK | t5 | 125 | 250 | 375 | 314 | ns |
| ROG - CLK | t9 | 125 | 250 | 375 | 207 | ns |
| ROG Rise, Fall | t6, t8 | 0 | 10 | 100 | ~3.9-4.7 | ns |
| ROG Pulse width | t7 | 6 | 10 | 20 | 10 | uS |

---------------------------------

| SHUT Requirement | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| SHUT - CLK | t14 | 150 | 200 | 250 | 210 | ns |
| SHUT - CLK | t15 | 150 | 200 | 250 | 210 | ns |
| SHUT Rise, Fall | t11, t13 | 0 | 10 | 100 | 3-5 | ns |
| SHUT Pulse width | t12 | 4 | 5 | --- | 41 | uS |
---------------------------------

The requirement to have ROG to SHUT be more than 10 uS is automatically fulfilled by waiting for all the pixels to be grabbed from the CCD.
The ROG period is derived from the clock timing on pp.4-5 in the documentation
> 3100 or more clock pulses are required  
The last requirement can be obtained by measuring the positive pulse train width of the CCD clock. 3100 pulses at 1 microsecond each corresponds to a total burst width of 3.1 milliseconds.

| Other Requirements | Symbol | Min. | Typ. | Max. | Measured | Units |
|:-----------:|:------:|:----:|:----:|:----:|:--------:|:-----:|
| SHUT to ROG | t16 | 10 | --- | --- | 5 | mS |
| ROG to SHUT | t17 | 10 | --- | --- | 3.06 | mS |
| CCD Width | --- | --- | --- | --- | 3.11 | mS |


