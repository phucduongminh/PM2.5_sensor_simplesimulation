#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void floatToBytes(float value, uint8_t *bytes) {
    memcpy(bytes, &value, sizeof(float));
}

void int16ToBytes(int16_t value, uint8_t *bytes) {
    bytes[0] = (value >> 8) & 0xFF;
    bytes[1] = value & 0xFF;
}

void calculateChecksum(uint8_t *data, int length, uint8_t *checksum) {
    uint8_t sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    *checksum = 0xFF - sum;
}


void convertToDataPacket(char *line, uint8_t *packet) {
    int sensor_id;
    char timestamp[20];
    float concentration;
    int16_t aqi;
    sscanf(line, "%d, %[^,], %f,%hd", &sensor_id, timestamp, &concentration, &aqi);
    int packet_length = 14;
    packet[0] = 0x7A;
    packet[1] = 0x0F;
    packet[2] = sensor_id;

    int year, month, day, hour, minute, second;
    sscanf(timestamp, "%d:%d:%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    time_t unix_time = mktime(&tm);
    packet[3] = (unix_time >> 24) & 0xFF;
    packet[4] = (unix_time >> 16) & 0xFF;
    packet[5] = (unix_time >> 8) & 0xFF;
    packet[6] = unix_time & 0xFF;
    floatToBytes(concentration, &packet[7]);
    int16ToBytes(aqi, &packet[11]);
    calculateChecksum(packet, packet_length - 2, &packet[13]);
    packet[14] = 0x7F;
}




int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Invalid command-line statement. Usage: dust_convert [data_filename.csv] [hex_filename.dat]\n");
        return 1;
    }
    char *data_filename = argv[1];
    char *hex_filename = argv[2];
    FILE *data_file = fopen(data_filename, "r");
    if (data_file == NULL) {
        printf("Unable to open input file: %s\n", data_filename);
        return 1;
    }
    FILE *hex_file = fopen(hex_filename, "w");
    if (hex_file == NULL) {
        printf("Unable to create output file: %s\n", hex_filename);
        fclose(data_file);
        return 1;
    }
    char line[100];
    uint8_t packet[15];
    while (fgets(line, sizeof(line), data_file)) {
        line[strcspn(line, "\r\n")] = '\0';
        convertToDataPacket(line, packet);

        for (int i = 0; i < 15; i++) {
            fprintf(hex_file, "%02X ", packet[i]);
        }
        fprintf(hex_file, "\n");
    }
    fclose(data_file);
    fclose(hex_file);
    printf("Data conversion completed successfully. Output file: %s\n", hex_filename);
    return 0;
}
