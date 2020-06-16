# fermentcontrol
Log fermentation of beer using a Raspberry Pi, 1Wire temperature sensors and Remote Controlled outlets

A small degree of flexibility is introduced to auto-discovery of connected 1Wire temperature sensors. In theory an unlimited number of sensors is supported. 

To achieve human-readable temperature sensor labels, place a file called "tempsensorlabels.txt" besides the fermentcontrol binary and add all temp sensors in the format as shown in this commit.


Control functionality has been removed for now.



## Compilation ##
Compilation is done using the supplied Makefile

## Start ##
Place an entry as below in your crontab. Type  
``crontab -e ``  
and place the following line at the bottom  
``*/5 *  *   *   *    /home/pi/fermentcontrol/fermentcontrol``


## Outlook ##
* Add function to auto-generate gnuplot_command.sh
* Pass desired output log file name as parameter to binary
