#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SensorData {
    int id;
    char time[20];
    double value;
};

int maxId(){
    FILE *dataFile = fopen("dust_aqi.csv", "r");

    char line[100];
    int id[100],idmax=1;

    // Skip the header line
    fgets(line, sizeof(line), dataFile);
    fgets(line, sizeof(line), dataFile);
    while (fgets(line, sizeof(line), dataFile))
    {
        struct SensorData data;

        sscanf(line, "%d", &id[idmax]);
        if(id[idmax]!=1){
            idmax++;
        } else {
            break;
        }
    }

    fclose(dataFile);
    return idmax;
}

void calculateSummary(const struct SensorData* sensorData, int size) {
    int i, id, idmax=maxId();
    FILE* summaryFile = fopen("dust_summary.csv", "w");
    if (!summaryFile) {
        printf("Failed to create the summary file.\n");
        return;
    }

    int hourend;
    int hourstart;
    int hour;
    sscanf(sensorData[0].time, "%*[^ ] %d:", &hourstart);
    sscanf(sensorData[size-2].time, "%*[^ ] %d:", &hourend);

    if(hourend=0){
        hourend=24;
    }
    if(hourend==hourstart){
        hour=24;
    } else if(hourend>hourstart){
        hour=hourend-hourstart;
    } else{
        hour=24+ (hourend-hourstart);
    }

    double maxValues[idmax+1];
    memset(maxValues, 0, sizeof(maxValues));

    double minValues[idmax+1];
    memset(minValues, 0, sizeof(minValues));

    double sumValues[idmax];
    memset(sumValues, 0, sizeof(sumValues));

    int countValues[idmax];
    memset(countValues, 0, sizeof(countValues));
    //printf("%d\n",idmax);

    for (i = 0; i < size; i++) {
        int id = sensorData[i].id;
        double value = sensorData[i].value;

        if (value > maxValues[id]) {
            maxValues[id] = value;
        }

        if(minValues[id]==0){
            minValues[id] = value;
        }

        if (value < minValues[id]) {
            minValues[id] = value;
        }

        sumValues[id] += value;
        countValues[id]=countValues[id]+1;
    }

    char minTime[20] = "N/A";
    char maxTime[20] = "N/A";
    fprintf(summaryFile, "id,parameter,time,value\n");

    for (id = 1; id <= idmax; id++) {
        for (i = 0; i < size; i++) {
            if (sensorData[i].id == id && sensorData[i].value == minValues[id]) {
                strncpy(minTime, sensorData[i].time, sizeof(minTime));
                //break;
            }

            if (sensorData[i].id == id && sensorData[i].value == maxValues[id]) {
                strncpy(maxTime, sensorData[i].time, sizeof(maxTime));
                //break;
            }
        }

        fprintf(summaryFile, "%d,max,%s,%.1f\n", id, maxTime, maxValues[id]);
        fprintf(summaryFile, "%d,min,%s,%.1f\n", id, minTime, minValues[id]);
        fprintf(summaryFile, "%d,mean,%02d:00:00,%.2f\n", id,hour, sumValues[id]/countValues[id]);
    }

    fclose(summaryFile);
    printf("Summary of dust concentration values has been calculated and stored in dust_summary.csv.\n");
}

int main() {
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

    calculateSummary(sensorData, dataSize);

    return 0;
}
