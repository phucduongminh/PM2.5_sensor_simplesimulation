#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct
{
    int id;
    char time[20];
    double value;
} SensorData;

void generate_sensor_data(int num_sensors, int sampling_time, int measurement_duration);

int isNumber(const char *str);

int main(int argc, char *argv[])
{
    int num_sensors = 1;
    int sampling_time = 30;
    int measurement_duration = 24;

    if (argc >= 3)
    {
        for (int i = 1; i < argc; i += 2)
        {
            if (strcmp(argv[i], "-n") == 0)
            {
                if (i + 1 >= argc)
                {
                    printf("Error 01: invalid command.\n");
                    return 1;
                }
                num_sensors = atoi(argv[i + 1]);
                if (num_sensors <= 0)
                {
                    printf("Error 02: invalid argument.\n");
                    return 1;
                }
            }
            else if (strcmp(argv[i], "-st") == 0)
            {
                if (i + 1 >= argc)
                {
                    printf("Error 01: invalid command.\n");
                    return 1;
                }
                sampling_time = atoi(argv[i + 1]);
                if (sampling_time <= 0)
                {
                    printf("Error 02: invalid argument.\n");
                    return 1;
                }
            }
            else if (strcmp(argv[i], "-si") == 0)
            {
                if (i + 1 >= argc)
                {
                    printf("Error 01: invalid command.\n");
                    return 1;
                }
                measurement_duration = atoi(argv[i + 1]);
                if (measurement_duration <= 0)
                {
                    printf("Error 02: invalid argument.\n");
                    return 1;
                }
            }
            else
            {
                printf("Error 02: invalid argument.\n");
                return 1;
            }
        }
        for (int i = 2; i < argc; i += 2)
        {
            if (isNumber(argv[i]) == 0)
            {
                printf("Error 02: invalid argument.\n");
                return 1;
            }
        }
    }
    else
    {
        // lack
        printf("Error 01: invalid command.\n");
        return 1;
    }

    if (argc > 7)
    {
        printf("Error 02: invalid argument.\n");
        return 1;
    }

    generate_sensor_data(num_sensors, sampling_time, measurement_duration);

    return 0;
}

void generate_sensor_data(int num_sensors, int sampling_time, int measurement_duration)
{
    srand(time(NULL));
    time_t current_time;
    time(&current_time);
    struct tm *start_time = localtime(&current_time);
    start_time->tm_sec -= measurement_duration * 3600;
    mktime(start_time);

    FILE *file = fopen("dust_sensor.csv", "w");
    if (file == NULL)
    {
        printf("Error 03: dust_sensor.csv access denied.\n");
        return;
    }

    fprintf(file, "id,time,value\n");

    // loop
    for (int j = measurement_duration * 3600; j > 0; j -= sampling_time)
    {
        for (int i = 1; i <= num_sensors; i++)
        {
            struct tm measurement_time = *start_time;
            measurement_time.tm_sec -= j;
            mktime(&measurement_time);
            double measurement_value = (double)rand() / RAND_MAX * 1000;

            fprintf(file, "%d,%04d:%02d:%02d %02d:%02d:%02d,%.1f\n", i,
                    measurement_time.tm_year + 1900, measurement_time.tm_mon + 1, measurement_time.tm_mday,
                    measurement_time.tm_hour, measurement_time.tm_min, measurement_time.tm_sec,
                    measurement_value);
        }
    }
    // init run
    for (int i = 1; i <= num_sensors; i++)
    {
        struct tm measurement_time = *start_time;
        mktime(&measurement_time);
        double measurement_value = (double)rand() / RAND_MAX * 1000;

        fprintf(file, "%d,%04d:%02d:%02d %02d:%02d:%02d,%.1f\n", i,
                measurement_time.tm_year + 1900, measurement_time.tm_mon + 1, measurement_time.tm_mday,
                measurement_time.tm_hour, measurement_time.tm_min, measurement_time.tm_sec,
                measurement_value);
    }

    fclose(file);
    printf("Simulation data generated successfully.\n");
}

int isNumber(const char *str)
{
    char *endptr;
    strtol(str, &endptr, 10); // Base 10 conversion

    return *endptr == '\0';
}