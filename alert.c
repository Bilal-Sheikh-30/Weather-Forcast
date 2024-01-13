#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "email_sender.h"

#define MAX_LINE_LENGTH 256

int isCurrentDate(char *date) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    char currentDate[11];
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", tm_info);

    return strcmp(date, currentDate) == 0;
}

void writeStatusFile(const char *filename, const char *content, const char *alert) {
    FILE *statusFile = fopen(filename, "w");
    if (statusFile == NULL) {
        perror("Error creating status file");
        return;
    }

    fprintf(statusFile, "\n%s", content);
    fprintf(statusFile, "\nALERT: %s. Stay at home.\n", alert);

    fclose(statusFile);
}

int main() {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char contentBuffer[MAX_LINE_LENGTH * 10];  // Buffer to store content for writing to the status file

    // Open the file
    file = fopen("weather_report.txt", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    time_t t;
    struct tm *tm_info;

    // Get current time
    time(&t);
    tm_info = localtime(&t);

    // Variable to check if content for the current date is found
    int currentDateFound = 0;

    // Read lines from the file
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strncmp(line, "Date:", 5) == 0) {
            // Extract the date part from the Date line
            char date[11];
            strncpy(date, line + 6, 10);
            date[10] = '\0';

            // Check if the date is the current date
            if (isCurrentDate(date)) {
                // Read and store the content for writing to the status file
                strcpy(contentBuffer, line);
                while (fgets(line, MAX_LINE_LENGTH, file) != NULL && strncmp(line, "\n", 1) != 0) {
                    strcat(contentBuffer, line);
                }

                // Check conditions for alerts and write to the status file
                char *linePtr = contentBuffer;
		double temperature = 0.0;

		while (sscanf(linePtr, "%[^\n]%*c", line) == 1) {
    			if (sscanf(line, "Average Temperature: %lf", &temperature) == 1) {
        		// Temperature found, break the loop
        		break;
    			}
    			// Move to the next line
    			linePtr += strlen(line) + 1;
		}
                const char *to = "imowais728@gmail.com";
                const char *file_path = "todayStatus.txt";
                if ((tm_info->tm_mon >= 10 || tm_info->tm_mon <= 1) && temperature < 10) {
                    writeStatusFile("todayStatus.txt", contentBuffer, "It is too cold today.\nPRECAUTIONS: Wear warm clothes");
                    int result = send_email_with_attachment(to, file_path);

                    if (result == 0) {
                        printf("Email sent successfully.\n");
                    } else {
                        printf("Failed to send email.\n");
                    }
                    
                } else if (tm_info->tm_mon >= 2 && tm_info->tm_mon <= 9 && temperature > 35) {
                    writeStatusFile("todayStatus.txt", contentBuffer, "It is too hot today.\nPRECAUTIONS: wear light clothes and drink more water");
                    int result = send_email_with_attachment(to, file_path);

                    if (result == 0) {
                        printf("Email sent successfully.\n");
                    } else {
                        printf("Failed to send email.\n");
                    }
                   
                }

                currentDateFound = 1;  // Set to 1 to indicate content for the current date is found

                break;  // Break out of the loop once content for the current date is processed
            }
        }
    }

    // If content for the current date is not found
    if (!currentDateFound) {
        printf("Data not found for the current date.\n");
    }

    // Close the file
    fclose(file);

    return 0;
}

