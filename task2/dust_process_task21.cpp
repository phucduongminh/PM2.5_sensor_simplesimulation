#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct SensorData {
    int id;
    char time[20];
    double value;
};

bool isValidCSV(const char* filename);

void processSensorData(struct SensorData* sensorData, int dataSize);

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

    if(isValidCSV(filename)==false){
        return 1;
    }

    // Read sensor data from file
    FILE* dataFile = fopen(filename, "r");
    if (!dataFile) {
        printf("Error 01: input file not found or not accessible.\n");
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
        printf("Error 01: input file not found or not accessible.\n");
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

bool isValidCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error 01: input file not found or not accessible.\n");
        return false;
    }

    char line[1000];
    int lineCount = 0;
    bool hasHeader = false;

    if (fgets(line, sizeof(line), file) != NULL) {
        //lineCount++;
        if (strcmp(line, "id,time,value\n") != 0) {
            printf("Error 02: Invalid csv file format.\n");
            fclose(file);
            return false;
        }
        hasHeader = true;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        lineCount++;

        // Check if all fields are blank
        char* token = strtok(line, ",");
        bool isBlankLine = true;
        while (token != NULL) {
            if (!isspace(*token)) {
                isBlankLine = false;
                break;
            }
            token = strtok(NULL, ",");
        }
        if (isBlankLine) {
            printf("Error 04: Data is missing at line %d.\n", lineCount);
            fclose(file);
            return false;
        }

        // Check for invalid number of fields
        int fieldCount = 1;
        token = strtok(NULL, ",");
        while (token != NULL) {
            fieldCount++;
            token = strtok(NULL, ",");
        }
        if (fieldCount != 3) {
            printf("Error 02: Invalid csv file format.\n");
            fclose(file);
            return false;
        }

        // Check for blank or invalid ID field
        char* id = strtok(line, ",");
        if (id == NULL || *id == '\n' || !isdigit(*id)) {
            printf("Error 04: Data is missing at line %d.\n", lineCount);
            fclose(file);
            return false;
        }

        // Check for blank or invalid time field
        id = strtok(NULL, ",");
        char* time = strtok(line, ",");
        if (time == NULL || *time == '\n') {
            printf("Error 04: Data is missing at line %d.\n", lineCount);
            fclose(file);
            return false;
        }

        // Check for blank concentration value
        time = strtok(NULL, ",");
        char* value = strtok(line, ",");
        if (value == NULL || *value == '\n' || *value == '\r') {
            printf("Error 04: Data is missing at line %d.\n", lineCount);
            fclose(file);
            return false;
        }
    }

    fclose(file);

    if (hasHeader) {
        
    } else {
        return false;
    }

    return true;
}

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
