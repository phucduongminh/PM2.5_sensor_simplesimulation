#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SensorData
{
    int id;
    char time[20];
    double value;
};

int isDuplicate(struct SensorData data, struct SensorData *sensorData, int dataSize);

void fixOutput(const char* filename);

const char *getPollutionLevel(double concentration);

void calculateAverageConcentration(struct SensorData *sensorData, int dataSize);

int main()
{
    FILE *dataFile = fopen("dust_sensor.csv", "r");
    if (!dataFile)
    {
        printf("Failed to open the data file.\n");
        return 1;
    }

    struct SensorData *sensorData = NULL;
    int dataSize = 0;
    char line[100];

    // Skip the header line
    fgets(line, sizeof(line), dataFile);

    while (fgets(line, sizeof(line), dataFile))
    {
        struct SensorData data;

        sscanf(line, "%d,%[^,],%lf", &data.id, data.time, &data.value);

        struct SensorData *newData = (struct SensorData *)realloc(sensorData, (dataSize + 1) * sizeof(struct SensorData));
        if (!newData)
        {
            printf("Memory allocation failed.\n");
            fclose(dataFile);
            free(sensorData);
            return 1;
        }

        sensorData = newData;
        sensorData[dataSize++] = data;
    }

    fclose(dataFile);

    calculateAverageConcentration(sensorData, dataSize);

    free(sensorData);
    fixOutput("dust_aqi.csv");

    return 0;
}

int isDuplicate(struct SensorData data, struct SensorData *sensorData, int dataSize) {
    int i;
    for (i = 0; i < dataSize; i++) {
        if (data.id == sensorData[i].id && strcmp(data.time, sensorData[i].time) == 0 && data.value == sensorData[i].value) {
            return 1;  // Duplicate line found
        }
    }
    return 0;  // Line is unique
}

void fixOutput(const char* filename){
    FILE* file = fopen(filename, "r+");
    
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }
    
    struct SensorData *sensorData = NULL;
    int dataSize = 0;
    char line[1000];
    
    // Skip the header line
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file) != NULL) {
        struct SensorData data;
        sscanf(line, "%d,%[^,],%lf", &data.id, data.time, &data.value);
        
        if (!isDuplicate(data, sensorData, dataSize)) {
            struct SensorData *newData = (struct SensorData *)realloc(sensorData, (dataSize + 1) * sizeof(struct SensorData));
            if (!newData) {
                printf("Memory allocation failed.\n");
                free(sensorData);
                fclose(file);
                return;
            }
            sensorData = newData;
            sensorData[dataSize++] = data;
        }
    }
    
    fclose(file);
    
    file = fopen(filename, "w");
    
    if (file == NULL) {
        printf("Failed to open the file.\n");
        free(sensorData);
        return;
    }
    
    fprintf(file, "id,time,value,aqi,pollution\n");
    
    int i, j;
    for (i = 0; i < dataSize; i++) {
        double aqi = 0.0;
        double average = sensorData[i].value;
        
        if (average >= 0.0 && average < 12.0) {
            aqi = (average / 12.0) * 50.0;
        } else if (average >= 12.0 && average < 35.5) {
            aqi = (average - 12) / (35.5 - 12) * (100 - 50) + 50;
        } else if (average >= 35.5 && average < 55.5) {
            aqi = (average - 35.5) / (55.5 - 35.5) * (100) + 100;
        } else if (average >= 55.5 && average < 150.5) {
            aqi = (average - 55.5) / (150.5 - 55.5) * (200 - 150) + 150;
        } else if (average >= 150.5 && average < 250.5) {
            aqi = (average - 150.5) / (250.5 - 150.5) * (300 - 200) + 200;
        } else if (average >= 250.5 && average < 350.5) {
            aqi = (average - 250.5) / (350.5 - 250.5) * (400 - 300) + 300;
        } else {
            aqi = (average - 350.5) / (500.0 - 350.5) * (500 - 400) + 400;
        }
        
        const char *pollution = getPollutionLevel(average);
        fprintf(file, "%d,%s,%.1f,%.0f,%s\n", sensorData[i].id, sensorData[i].time, average, aqi, pollution);
    }
    
    fclose(file);
    free(sensorData);
    
    printf("Duplicate lines removed successfully.\n");
}

const char *getPollutionLevel(double concentration)
{
    if (concentration >= 0.0 && concentration < 12.0)
    {
        return "Good";
    }
    else if (concentration >= 12.0 && concentration < 35.5)
    {
        return "Moderate";
    }
    else if (concentration >= 35.5 && concentration < 55.5)
    {
        return "Slightly unhealthy";
    }
    else if (concentration >= 55.5 && concentration < 150.5)
    {
        return "Unhealthy";
    }
    else if (concentration >= 150.5 && concentration < 250.5)
    {
        return "Very unhealthy";
    }
    else if (concentration >= 250.5 && concentration < 350.5)
    {
        return "Hazardous";
    }
    else
    {
        return "Extremely hazardous";
    }
}

void calculateAverageConcentration(struct SensorData *sensorData, int dataSize)
{
    FILE *aqiFile = fopen("dust_aqi.csv", "w");
    if (!aqiFile)
    {
        printf("Failed to create the AQI file.\n");
        return;
    }

    fprintf(aqiFile, "id,time,value\n");

    int i, j;
    for (i = 0; i < dataSize; i++)
    {
        int id = sensorData[i].id;
        char hour[14];
        strncpy(hour, sensorData[i].time, 13);
        hour[13] = '\0';

        double sum = 0.0;
        int count = 0;

        for (j = 0; j < dataSize; j++)
        {
            if (id == sensorData[j].id && strncmp(hour, sensorData[j].time, 13) == 0)
            {
                sum += sensorData[j].value;
                count++;
            }
        }

        double average = sum / count;
        const char *pollution = getPollutionLevel(average);

        char formattedTime[20];
        strncpy(formattedTime, hour, 11);
        snprintf(formattedTime + 11, sizeof(formattedTime) - 11, "%02d:00:00", atoi(hour + 11));
        fprintf(aqiFile, "%d,%s,%.1f\n", id, formattedTime, average);
    }
    fclose(aqiFile);
    printf("Average dust concentrations per hour have been calculated and stored in dust_aqi.csv.\n");
}
