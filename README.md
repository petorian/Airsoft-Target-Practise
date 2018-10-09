# Airsoft-Target-Practise
Arduino code for airsoft targets

This project will contain several ino files to use as Airsoft targets

The first file has been uploaded and does the following:

  Starts with blue leds on which indicates stand-by.
  First hit detected activates the target; the leds turn off and wait a random time after which they turn yellow for 2 seconds (or other timeout value)
  If hit too early, it will count as a miss
  if not hit at all, it will count as a miss
  if hit within the 2 second timeout, it will count as a hit.

  After the number of cycles have passed, the leds will flash white one time and then it will show the score:
  it will flash green for every hit and after that flash red for every miss.
  Then it will return to stand-by waiting for another initial hit to activate the target again.

  this sketch is based on the sketches in this topic on the arduino forum: http://forum.arduino.cc/index.php?topic=373655.0
 
