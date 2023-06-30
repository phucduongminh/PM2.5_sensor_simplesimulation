#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SensorData {
    int id;
    char time[20];
    double value;
};

void processSensorData(struct SensorData* sensorData, int dataSize) {
    // Filtering outliers
    struct SensorData* outliers = NULL;
    int outliersCount = 0;

    for (int i = 0; i < dataSize; i++) {
        if (sensorData[i].value < 5.0 || sensorData[i].value > 550.5) {
            outliersCount++;
            outliers = (struct SensorData*)realloc(outliers, outliersCount * sizeof(struct SensorData));
            outliers[outliersCount - 1] = sensorData[i];
        }
    }

    // Writing outliers to file
    FILE* outlierFile = fopen("dust_outlier.csv", "w");
    if (!outlierFile) {
        printf("Failed to create the outlier file.\n");
        free(outliers);
        return;
    }

    fprintf(outlierFile, "number of outliers: %d\n", outliersCount);
    fprintf(outlierFile, "id,time,value\n");

    for (int i = 0; i < outliersCount; i++) {
        fprintf(outlierFile, "%d,%s,%.1f\n", outliers[i].id, outliers[i].time, outliers[i].value);
    }

    fclose(outlierFile);
    printf("Outliers have been filtered and stored in dust_outlier.csv.\n");

    free(outliers);
}

int main(int argc, char* argv[]) {
    const char* filename = "dust_sensor.csv";

    // Check if a filename is provided as a command-line argument
    if (argc > 1) {
        filename = argv[1];
    }

    if (argc > 2||argc<1) {
        printf("Error 03: invalid command\n");
        return 1;
    }

    // Read sensor data from file
    FILE* dataFile = fopen(filename, "r");
    if (!dataFile) {
        printf("Error 01: input file not found or not accessible\n");
        return 1;
    }

    struct SensorData* sensorData = NULL;
    int dataSize = 0;

    // Skip the header line
    char line[256];
    fgets(line, sizeof(line), dataFile);

    while (fgets(line, sizeof(line), dataFile)) {
        struct SensorData data;
        sscanf(line, "%d,%[^,],%lf", &data.id, data.time, &data.value);

        dataSize++;
        sensorData = (struct SensorData*)realloc(sensorData, dataSize * sizeof(struct SensorData));
        sensorData[dataSize - 1] = data;
    }

    fclose(dataFile);

    // Process the sensor data
    processSensorData(sensorData, dataSize);

    // Write the remaining data to a new file
    FILE* updatedDataFile = fopen("dust_sensor_updated.csv", "w");
    if (!updatedDataFile) {
        printf("Failed to create the updated data file.\n");
        free(sensorData);
        return 1;
    }

    fprintf(updatedDataFile, "id,time,value\n");

    for (int i = 0; i < dataSize; i++) {
        // Check if the value is within the acceptable range
        if (sensorData[i].value >= 5.0 && sensorData[i].value <= 550.5) {
            fprintf(updatedDataFile, "%d,%s,%.1f\n", sensorData[i].id, sensorData[i].time, sensorData[i].value);
        }
    }

    fclose(updatedDataFile);
    printf("Outlier lines have been removed from dust_sensor.csv.\n");

    // Replace the original file with the updated file
    remove(filename);
    rename("dust_sensor_updated.csv", filename);

    free(sensorData);

    return 0;
}
