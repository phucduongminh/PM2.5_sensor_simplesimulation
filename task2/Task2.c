#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

struct SensorData {
    int id;
    char time[20];
    double value;
};

//2.1
bool isValidCSV(const char* filename);

void processSensorData(struct SensorData* sensorData, int dataSize);

//2.2
int isDuplicate(struct SensorData data, struct SensorData *sensorData, int dataSize);

void fixOutput(const char* filename);

const char *getPollutionLevel(double concentration);

void calculateAverageConcentration(struct SensorData *sensorData, int dataSize);

//2.3
int maxId();

void calculateSummary(const struct SensorData* sensorData, int size);

//2.4
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

void calculateStatistic();

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
    struct SensorData* sensorData1 = NULL;
    int dataSize = 0,i;

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

    for (i = 0; i < dataSize; i++) {
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
    //delay(2000);
     sleep(5);
    //2.2
    FILE* dataFile1 = fopen(filename, "r");
    if (!dataFile1) {
        printf("Error 01: input file not found or not accessible.\n");
        return 1;
    }
    fgets(line, sizeof(line), dataFile1);
    dataSize = 0;

    while (fgets(line, sizeof(line), dataFile1))
    {
        struct SensorData data;

        sscanf(line, "%d,%[^,],%lf", &data.id, data.time, &data.value);

        struct SensorData *newData = (struct SensorData *)realloc(sensorData, (dataSize + 1) * sizeof(struct SensorData));
        if (!newData)
        {
            printf("Error 01: input file not found or not accessible.\n");
            fclose(dataFile1);
            free(sensorData1);
            return 1;
        }

        sensorData1 = newData;
        sensorData1[dataSize++] = data;
    }

    fclose(dataFile1);

    calculateAverageConcentration(sensorData1, dataSize);
    fixOutput("dust_aqi.csv");

    //2.3
    calculateSummary(sensorData1, dataSize);

    free(sensorData1);

    //2.4
    calculateStatistic();
    return 0;
}

//2.1
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
    int outliersCount = 0,i;

    for (i = 0; i < dataSize; i++) {
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

    for (i = 0; i < outliersCount; i++) {
        fprintf(outlierFile, "%d,%s,%.1f\n", outliers[i].id, outliers[i].time, outliers[i].value);
    }

    fclose(outlierFile);
    printf("Outliers have been filtered and stored in dust_outlier.csv.\n");

    free(outliers);
}

//2.2
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

//2.3
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

//2.4
void calculateStatistic(){
    char input_file[] = "dust_aqi.csv";
    char output_file[] = "dust_statistics.csv";
    char line[MAX_LINE_LENGTH];

    SensorStatistics statistics[MAX_LINE_LENGTH];
    int numSensors = 0;
    
    FILE *input = fopen(input_file, "r");
    if (input == NULL) {
        printf("Error 01: input file not found or not accessible.\n");
        exit(1);
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
        printf("Error 01: input file not found or not accessible.\n");
        exit(1);
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
}
