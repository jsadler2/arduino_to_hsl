This package was designed to automatically insert sensor data using HTTP requests from a GPRS Sim900 Shield attached to an Arduino code. 
The Arduino code is called 'sensorToHSL'

To use the package, do the following:

	1. put inputSensorData.php into the 'client' folder of HydroServerLite
	
	2. in 'sensorToHSL.ino' modify these variables found at the top of the sketch:
		- the 'SourceID' variable to the ID of the desired source (created in HSL)
		- 'SiteID' to the ID of the desired site (created in HSL)
		- 'VarID1' to the ID of the desired variable (created in HSL)
		- 'VarID2' to the ID of the desired variable (created in HSL)
		-  and the 'url' of the 'inputSensorData.php' file
	
	3. modify the 'sensors()' function in 'sensorToHSL.ino' to read the data from YOUR sensor(s)
	
	4. modify 'recTime' and 'recIntv' to change when the first reading occurs (what minute) and at how often respectively

	5. upload sketch to Arduino


written by Jeff Sadler