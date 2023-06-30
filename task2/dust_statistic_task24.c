#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_POLLUTION_LEVELS 7

typedef struct {
    int id;
    char pollution[MAX_LINE_LENGTH];
} DataPoint;

typedef struct {
    int id;
    int duration[MAX_POLLUTION_LEVELS];
} SensorStatistics;

int main() {
    char input_file[] = "dust_aqi.csv";
    char output_file[] = "dust_statistics.csv";
    char line[MAX_LINE_LENGTH];

    SensorStatistics statistics[MAX_LINE_LENGTH];
    int numSensors = 0;
    
    FILE *input = fopen(input_file, "r");
    if (input == NULL) {
        printf("Failed to open the input file.\n");
        return 1;
    }

    fgets(line, sizeof(line), input); // Skip the header line

    while (fgets(line, sizeof(line), input)) {
        DataPoint data;
        sscanf(line, "%d,%*[^,],%*[^,],%*[^,],%[^,\n]", &data.id, data.pollution);
        
        int i;
        int sensorFound = 0;
        for (i = 0; i < numSensors; i++) {
            if (statistics[i].id == data.id) {
                sensorFound = 1;
                break;
            }
        }

        if (!sensorFound) {
            statistics[numSensors].id = data.id;
            memset(statistics[numSensors].duration, 0, sizeof(statistics[numSensors].duration));
            numSensors++;
        }

        if (strcmp(data.pollution, "Good") == 0) {
            statistics[i].duration[0]++;
        } else if (strcmp(data.pollution, "Moderate") == 0) {
            statistics[i].duration[1]++;
        } else if (strcmp(data.pollution, "Slightly unhealthy") == 0) {
            statistics[i].duration[2]++;
        } else if (strcmp(data.pollution, "Unhealthy") == 0) {
            statistics[i].duration[3]++;
        } else if (strcmp(data.pollution, "Very unhealthy") == 0) {
            statistics[i].duration[4]++;
        } else if (strcmp(data.pollution, "Hazardous") == 0) {
            statistics[i].duration[5]++;
        } else if (strcmp(data.pollution, "Extremely hazardous") == 0) {
            statistics[i].duration[6]++;
        }
    }
    
    fclose(input);

    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        printf("Failed to open the output file.\n");
        return 1;
    }

    fprintf(output, "id,pollution,duration\n");
    
    int i;
    for (i = 0; i < numSensors; i++) {
        fprintf(output, "%d,Good,%d\n", statistics[i].id, statistics[i].duration[0]);
        fprintf(output, "%d,Moderate,%d\n", statistics[i].id, statistics[i].duration[1]);
        fprintf(output, "%d,Slightly unhealthy,%d\n", statistics[i].id, statistics[i].duration[2]);
        fprintf(output, "%d,Unhealthy,%d\n", statistics[i].id, statistics[i].duration[3]);
        fprintf(output, "%d,Very unhealthy,%d\n", statistics[i].id, statistics[i].duration[4]);
        fprintf(output, "%d,Hazardous,%d\n", statistics[i].id, statistics[i].duration[5]);
        fprintf(output, "%d,Extremely hazardous,%d\n", statistics[i].id, statistics[i].duration[6]);
    }
    
    fclose(output);

    printf("Statistics have been calculated and saved to the file '%s'.\n", output_file);
    
    return 0;
}
