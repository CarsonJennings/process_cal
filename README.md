# process_cal
An application that processes .ics calendar files and outputs the events within two given dates.

**How to run the application** \
To run the Python implementation of process_cal4.py simply run the command: \
&nbsp;&nbsp;&nbsp;&nbsp;python tester4.py --file=<ics filename> --start=yyyy/mm/dd --end=yyyy/mm/dd \
\
To run the C implementation you will first need to compile the files. This can be done using the "make" command. \
After the file has been compiled process_cal3.c can be run using the command: \
&nbsp;&nbsp;&nbsp;&nbsp;./process_cal --file=<ics filename> --start=yyyy/mm/dd --end=yyyy/mm/dd \
\
\
**A note on .ics files** \
This application only processes simple .ics files, meaning that the file can only contain the following information formats: \
\
  BEGIN \
  DTSART \
  DTEND \
  RRULE \
  LOCATION \
  SUMMARY \
  END
