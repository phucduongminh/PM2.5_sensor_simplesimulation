#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>

struct SensorData {
    int id;
    std::string time;
    double value;
};

std::string getPollutionLevel(double concentration) {
    if (concentration >= 0.0 && concentration < 12.0) {
        return "Good";
    } else if (concentration >= 12.0 && concentration < 35.5) {
        return "Moderate";
    } else if (concentration >= 35.5 && concentration < 55.5) {
        return "Slightly unhealthy";
    } else if (concentration >= 55.5 && concentration < 150.5) {
        return "Unhealthy";
    } else if (concentration >= 150.5 && concentration < 250.5) {
        return "Very unhealthy";
    } else if (concentration >= 250.5 && concentration < 350.5) {
        return "Hazardous";
    } else {
        return "Extremely hazardous";
    }
}

void calculateAverageConcentration(const std::vector<SensorData>& sensorData) {
    std::map<int, std::map<std::string, std::vector<double>>> hourlyData;

    for (const SensorData& data : sensorData) {
        std::string hour = data.time.substr(0, 13); // Extract the hour portion from the timestamp

        hourlyData[data.id][hour].push_back(data.value);
    }

    std::ofstream aqiFile("dust_aqi.csv");
    if (!aqiFile) {
        std::cout << "Failed to create the AQI file.\n";
        return;
    }

    aqiFile << "id,time,value,aqi,pollution\n";

    for (const auto& entry : hourlyData) {
        int id = entry.first;

        for (const auto& hourEntry : entry.second) {
            std::string hour = hourEntry.first;

            double sum = 0.0;
            const std::vector<double>& values = hourEntry.second;

            for (double value : values) {
                sum += value;
            }

            double average = sum / values.size();
            double aqi = 0.0;

            if (average >= 0.0 && average < 12.0) {
                aqi = (average / 12.0) * 50.0;
            } else if (average >= 12.0 && average < 35.5) {
                aqi = (average-12)/(35.5-12)*(100-50)+50;
            } else if (average >= 35.5 && average < 55.5) {
                aqi = (average-35.5)/(55.5-35.5)*(50)+100;
            } else if (average >= 55.5 && average < 150.5) {
                aqi = (average-55.5)/(150.5-55.5)*(100-50)+150;
            } else if (average >= 150.5 && average < 250.5) {
                aqi = (average-150.5)/(250.5-150.5)*(100)+200;
            } else if (average >= 250.5 && average < 350.5) {
                aqi = (average-250.5)/(100)*(100)+300;
            } else {
                aqi = (average-350.5)/( 550.5-350.5)*(100)+400;
            }

            std::string pollution = getPollutionLevel(average);

            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hour.substr(11, 2) << ":00:00";
            std::string formattedTime = hour.substr(0, 11) + oss.str();

            aqiFile << id << "," << formattedTime << "," << average << "," << aqi << "," << pollution << "\n";
        }
    }

    aqiFile.close();
    std::cout << "Average dust concentrations per hour have been calculated and stored in dust_aqi.csv.\n";
}

int main() {
    std::ifstream dataFile("dust_sensor.csv");
    if (!dataFile) {
        std::cout << "Failed to open the data file.\n";
        return 1;
    }

    std::vector<SensorData> sensorData;
    std::string line;

    // Skip the header line
    std::getline(dataFile, line);

    while (std::getline(dataFile, line)) {
        std::istringstream iss(line);
        SensorData data;
        std::string value;

        std::getline(iss, value, ',');
        data.id = std::stoi(value);

        std::getline(iss, data.time, ',');

        std::getline(iss, value, ',');
        data.value = std::stod(value);

        sensorData.push_back(data);
    }

    dataFile.close();

    calculateAverageConcentration(sensorData);

    return 0;
}


/*This code is a program that calculates the average dust concentration per hour 
from a data file and stores the results in a CSV file. The program defines a 
struct called SensorData, which represents a single data entry with an ID, a timestamp, 
and a value. There is also a function called getPollutionLevel, which takes a 
concentration value as input and returns a string indicating the pollution level based 
on the concentration value. The main function of the program reads data from a file 
called "dust_sensor.csv" and stores it in a vector of SensorData objects. It then calls 
the calculateAverageConcentration function, passing the vector of SensorData objects as 
an argument. The calculateAverageConcentration function initializes a map called 
hourlyData to store the hourly data. It iterates over each SensorData object in the input 
vector and extracts the hour portion from the timestamp. It then adds the value to the 
corresponding hour entry in the hourlyData map. Next, the function opens a file called 
"dust_aqi.csv" for writing. It writes a header line to the file and then iterates over 
the hourlyData map. For each entry, it calculates the average value by summing up the values 
and dividing by the number of values. It then calculates the Air Quality Index (AQI) based 
on the average value using a set of if-else conditions. It also calls the getPollutionLevel 
function to determine the pollution level based on the average value. The function formats 
the hour portion of the timestamp and writes the ID, formatted time, average value, AQI, 
and pollution level to the CSV file. Finally, the function closes the file and prints 
a message indicating that the average dust concentrations per hour have been calculated 
and stored in the "dust_aqi.csv" file. The main function also checks if the data file was 
successfully opened. If not, it prints an error message and returns 1. After calling the 
calculateAverageConcentration function, the main function returns 0 to indicate 
successful execution*/