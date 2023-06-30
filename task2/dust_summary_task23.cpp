#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SensorData {
    int id;
    char time[9];
    double value;
};

void calculateSummary(const struct SensorData* sensorData, int size) {
    FILE* summaryFile = fopen("dust_summary.csv", "w");
    if (!summaryFile) {
        printf("Failed to create the summary file.\n");
        return;
    }

    fprintf(summaryFile, "id,parameter,time,value\n");

    double maxValues[100] = { 0 };
    double minValues[100] = { 0 };
    double sumValues[100] = { 0 };
    int countValues[100] = { 0 };

    for (int i = 0; i < size; i++) {
        int id = sensorData[i].id;
        double value = sensorData[i].value;

        if (value > maxValues[id] || maxValues[id] == 0) {
            maxValues[id] = value;
        }

        if (value < minValues[id] || minValues[id] == 0) {
            minValues[id] = value;
        }

        sumValues[id] += value;
        countValues[id]++;
    }

    for (int id = 1; id <= 100; id++) {
        double minValue = minValues[id];
        double maxValue = maxValues[id];
        double sumValue = sumValues[id];
        int countValue = countValues[id];
        double meanValue = sumValue / countValue;

        char minTime[9] = "N/A";
        char maxTime[9] = "N/A";

        for (int i = 0; i < size; i++) {
            if (sensorData[i].id == id && sensorData[i].value == minValue) {
                strncpy(minTime, sensorData[i].time, sizeof(minTime));
                break;
            }

            if (sensorData[i].id == id && sensorData[i].value == maxValue) {
                strncpy(maxTime, sensorData[i].time, sizeof(maxTime));
                break;
            }
        }

        fprintf(summaryFile, "%d,max,%s,%f\n", id, maxTime, maxValue);
        fprintf(summaryFile, "%d,min,%s,%f\n", id, minTime, minValue);
        fprintf(summaryFile, "%d,mean,10:00:00,%f\n", id, meanValue);
    }

    fclose(summaryFile);
    printf("Summary of dust concentration values has been calculated and stored in dust_summary.csv.\n");
}

int main() {
    FILE* dataFile = fopen("dust_sensor.csv", "r");
    if (!dataFile) {
        printf("Failed to open the data file.\n");
        return 1;
    }

    struct SensorData sensorData[100];
    char line[100];

    // Skip the header line
    fgets(line, sizeof(line), dataFile);

    int size = 0;
    while (fgets(line, sizeof(line), dataFile)) {
        char* value = strtok(line, ",");
        sensorData[size].id = atoi(value);

        value = strtok(NULL, ",");
        strncpy(sensorData[size].time, value, sizeof(sensorData[size].time));

        value = strtok(NULL, ",");
        sensorData[size].value = atof(value);

        size++;
    }

    fclose(dataFile);

    calculateSummary(sensorData, size);

    return 0;
}
