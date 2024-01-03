#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// Callback function to write data received from the API
size_t write_callback(void *contents, size_t size, size_t nmemb, char **output) {
    size_t realsize = size * nmemb;
    *output = realloc(*output, realsize + 1);
    if (*output == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }
    memcpy(*output, contents, realsize);
    (*output)[realsize] = 0;
    return realsize;
}

// Function to get latitude and longitude based on city name
void getLatLonFromCity(const char *city, double *lat, double *lon) {
    if (strcmp(city, "karachi") == 0) {
        *lat = 24.8;
        *lon = 67;
    } else if (strcmp(city, "lahore") == 0) {
        *lat = 31.5;
        *lon = 74.35;
    } else if (strcmp(city, "isl") == 0 || strcmp(city, "islamabad") == 0) {
        *lat = 33.68;
        *lon = 73.04;
    } else {
        fprintf(stderr, "Unsupported city: %s\n", city);
        exit(EXIT_FAILURE);
    }
}

int main() {
    CURL *curl;
    CURLcode res;

    // Redirect standard output to a file
    char city[20];
    printf("Enter city name (karachi, lahore, isl): ");
    scanf("%s", city);

    // Open the file with the specified city name
    char filename[50];
    snprintf(filename, sizeof(filename), "%s_database.txt", city);
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    // Redirect standard output to the file
    dup2(fileno(file), STDOUT_FILENO);
    fclose(file);

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Get latitude and longitude based on the city name
        double lat, lon;
        getLatLonFromCity(city, &lat, &lon);

        // Construct the API URL
        char url[200];
        snprintf(url, sizeof(url), "https://api.openweathermap.org/data/2.5/forecast?lat=%.5f&lon=%.5f&appid=960ab4072cce45cce5e399e8453ed270", lat, lon);

        // Perform the HTTP GET request
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Response data will be stored here
        char *response_data = NULL;

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse the JSON response using cJSON
            cJSON *root = cJSON_Parse(response_data);

            if (root) {
                // Extract information from the JSON response
                cJSON *list = cJSON_GetObjectItem(root, "list");

                if (list && cJSON_IsArray(list)) {
                    int size = cJSON_GetArraySize(list);
                    for (int i = 0; i < size; i++) {
                        cJSON *item = cJSON_GetArrayItem(list, i);

                        // Extract relevant information from the item
                        cJSON *main = cJSON_GetObjectItem(item, "main");
                        cJSON *weatherArray = cJSON_GetObjectItem(item, "weather");

                        if (main && weatherArray && cJSON_IsObject(main) && cJSON_IsArray(weatherArray)) {
                            // Access the first element of the "weather" array
                            cJSON *firstWeather = cJSON_GetArrayItem(weatherArray, 0);

                            if (firstWeather && cJSON_IsObject(firstWeather)) {
                                // Display relevant information
                                printf("Time: %s\n", cJSON_GetObjectItem(item, "dt_txt")->valuestring);
                                printf("Temperature: %.2f\n", cJSON_GetObjectItem(main, "temp")->valuedouble);
                                printf("Feels Like: %.2f\n", cJSON_GetObjectItem(main, "feels_like")->valuedouble);
                                printf("Min Temperature: %.2f\n", cJSON_GetObjectItem(main, "temp_min")->valuedouble);
                                printf("Max Temperature: %.2f\n", cJSON_GetObjectItem(main, "temp_max")->valuedouble);
                                printf("Pressure: %d\n", cJSON_GetObjectItem(main, "pressure")->valueint);
                                printf("Humidity: %d\n", cJSON_GetObjectItem(main, "humidity")->valueint);
                                printf("Weather description: %s\n", cJSON_GetObjectItem(firstWeather, "description")->valuestring);
                                printf("\n");
                            } else {
                                fprintf(stderr, "Failed to extract weather information in item %d\n", i);

                                // Print the content of the problematic item
                                char *item_str = cJSON_Print(item);
                                fprintf(stderr, "Problematic JSON item:\n%s\n", item_str);
                                free(item_str);
                            }
                        } else {
                            fprintf(stderr, "Failed to extract main or weather information in item %d\n", i);

                            // Print the content of the problematic item
                            char *item_str = cJSON_Print(item);
                            fprintf(stderr, "Problematic JSON item:\n%s\n", item_str);
                            free(item_str);
                        }
                    }
                } else {
                    fprintf(stderr, "Failed to extract list information\n");
                    // Print the content of the entire response
                    fprintf(stderr, "JSON response:\n%s\n", response_data);
                }

                // Free cJSON structure
                cJSON_Delete(root);
            } else {
                fprintf(stderr, "Failed to parse JSON data\n");
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
        if (response_data) {
            free(response_data);
        }
    } else {
        fprintf(stderr, "Failed to initialize curl\n");
    }

    // Cleanup curl global state
    curl_global_cleanup();

    return 0;
}
