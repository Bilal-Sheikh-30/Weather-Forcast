#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256

int isCurrentDateOrLater(char *date) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    char currentDate[11];
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", tm_info);

    return strcmp(date, currentDate) >= 0;
}

int main() {
    FILE *file;
    FILE *reportFile;
    char line[MAX_LINE_LENGTH];
    
    // Open the file
    file = fopen("karachi_database.txt", "r");
    
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Variables to store cumulative values
    double sumTemperature = 0.0, sumMinTemperature = 0.0, sumMaxTemperature = 0.0;
    double sumPressure = 0.0, sumFeelsLike = 0.0, sumHumidity = 0.0;

    // Variable to count entries for averaging
    int entryCount = 0;

    // Previous date for comparison
    char prevDate[11] = "";

    // Open the report file
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    char currentDate[11];
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", tm_info);


    reportFile = fopen("weather_report.txt", "w");

    if (reportFile == NULL) {
        perror("Error creating report file");
        fclose(file);
        return 1;
    }

    // Read lines from the file
    fprintf(reportFile, "\n--- WEATHER FORCAST ---\n");
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strncmp(line, "Time:", 5) == 0) {
            // Extract the date part from the time line
            char date[11];
            strncpy(date, line + 6, 10);
            date[10] = '\0';

            // Check if the date is equal to or greater than the current date
            if (isCurrentDateOrLater(date)) {
                // If the date changes, write averages for the previous date to the report file
                if (strcmp(date, prevDate) != 0 && entryCount > 0) {
                    // Calculate averages
                    double avgTemperature = sumTemperature / entryCount;
                    double avgMinTemperature = sumMinTemperature / entryCount;
                    double avgMaxTemperature = sumMaxTemperature / entryCount;
                    double avgPressure = sumPressure / entryCount;
                    double avgFeelsLike = sumFeelsLike / entryCount;
                    double avgHumidity = sumHumidity / entryCount;

                    // Write averages for the previous date to the report file
                    fprintf(reportFile, "\nDate: %s\n", prevDate);
                    fprintf(reportFile, "Average Temperature: %.2lf\n", avgTemperature);
                    fprintf(reportFile, "Average Min Temperature: %.2lf\n", avgMinTemperature);
                    fprintf(reportFile, "Average Max Temperature: %.2lf\n", avgMaxTemperature);
                    fprintf(reportFile, "Average Pressure: %.2lf\n", avgPressure);
                    fprintf(reportFile, "Average Feels Like: %.2lf\n", avgFeelsLike);
                    fprintf(reportFile, "Average Humidity: %.2lf\n", avgHumidity);

                    // Reset cumulative values and entry count for the new date
                    sumTemperature = 0.0;
                    sumMinTemperature = 0.0;
                    sumMaxTemperature = 0.0;
                    sumPressure = 0.0;
                    sumFeelsLike = 0.0;
                    sumHumidity = 0.0;
                    entryCount = 0;
                }

                // Variables to store current entry values
                double temperature, minTemperature, maxTemperature, pressure, feelsLike, humidity;

                // Read the values from the following lines
                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Temperature: %lf", &temperature);

                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Feels Like: %lf", &feelsLike);

                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Min Temperature: %lf", &minTemperature);

                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Max Temperature: %lf", &maxTemperature);

                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Pressure: %lf", &pressure);

                fgets(line, MAX_LINE_LENGTH, file);
                sscanf(line, "Humidity: %lf", &humidity);

                // Accumulate values
                sumTemperature += temperature;
                sumMinTemperature += minTemperature;
                sumMaxTemperature += maxTemperature;
                sumPressure += pressure;
                sumFeelsLike += feelsLike;
                sumHumidity += humidity;

                // Increment entry count
                entryCount++;

                // Update the previous date
                strcpy(prevDate, date);
            }
        }
    }

    // Write averages for the last date to the report file
    if (entryCount > 0) {
        // Calculate averages
        double avgTemperature = sumTemperature / entryCount;
        double avgMinTemperature = sumMinTemperature / entryCount;
        double avgMaxTemperature = sumMaxTemperature / entryCount;
        double avgPressure = sumPressure / entryCount;
        double avgFeelsLike = sumFeelsLike / entryCount;
        double avgHumidity = sumHumidity / entryCount;

        // Write averages for the last date to the report file
        fprintf(reportFile, "\nDate: %s\n", prevDate);
        fprintf(reportFile, "Average Temperature: %.2lf\n", avgTemperature);
        fprintf(reportFile, "Average Min Temperature: %.2lf\n", avgMinTemperature);
        fprintf(reportFile, "Average Max Temperature: %.2lf\n", avgMaxTemperature);
        fprintf(reportFile, "Average Pressure: %.2lf\n", avgPressure);
        fprintf(reportFile, "Average Feels Like: %.2lf\n", avgFeelsLike);
        fprintf(reportFile, "Average Humidity: %.2lf\n", avgHumidity);
    }

    // Close the files
    fclose(file);
    fclose(reportFile);

    return 0;
}

