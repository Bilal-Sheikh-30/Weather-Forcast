# cew-OEL
This project is capable to fetch data from api, stores it in a txt file and process it to generate report.It generates forcast report and stores it in a txt file. In addition to this, an alert system is also incorporated with it to send alert to the user via email if it finds any alarming reading it will send alert regarding it.Moreover, it also sends the weekly forcast report.

Description of files is as follow:<br>
fetchData.c: this file fetches data from api parse it using curl and saves it in database file "karachi_database.txt"<br>
generateReport.c: this file reads database file and process its data to make a report and save it in "weather_report.txt"<br>
sendForcastReport.c: this file sends the "weather_report.txt" via email.<br>
alert.c: this file reads the "weather_report.txt" and gets the data of current date.If the temperature meets the given condition it sends an alert via email.