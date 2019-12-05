# fermentcontrol
Control (or at least log) fermentation of beer using a Raspberry Pi, 1Wire temperature sensors and Remote Controlled outlets

this initial commit does not offer any flexibility with regard to 
* which or how many temperature sensor are needed to be read
* which code is needed to control the RC outlet
* set temperature is hard-coded


so it probably will not be of any use for anybody until I get around to implementing a lot of flexibility for it...


## Compilation ##
Compilation is done by executing 
``gcc fermentcontrol.c -o fermentcontrol``

## Start ##
Place an entry as below in your crontab. Type 
``crontab -e ``
and place the following line at the bottom
``*/5 *  *   *   *    /home/pi/fermentcontrol/fermentcontrol``
