#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <signal.h>
#include <stdbool.h>
#include <_mingw_unicode.h>
#include <apisetcconv.h>
#include <winapifamily.h>
#include <minwinbase.h>
#include <bemapiset.h>
#include <debugapi.h>
#include <errhandlingapi.h>
#include <fibersapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <heapapi.h>
#include <ioapiset.h>
#include <interlockedapi.h>
#include <jobapi.h>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <namedpipeapi.h>
#include <namespaceapi.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <processtopologyapi.h>
#include <profileapi.h>
#include <realtimeapiset.h>
#include <securityappcontainer.h>
#include <securitybaseapi.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <systemtopologyapi.h>
#include <threadpoolapiset.h>
#include <threadpoollegacyapiset.h>
#include <utilapiset.h>
#include <wow64apiset.h>
#include <winbase.h>


// Max buffer size for reading config file
#define MAX_BUFFER_SIZE 1024

// Structure to hold all configuration settings
typedef struct {
    char inputFilePath[256];
    char outputFilePath[256];
    char logFilePath[256];
    int algorithmType; // 1 for BFS, 2 for DFS
    int dataStructureType; // 1 for Tree, 2 for Graph
    int enableMultithreading; // 0 for No, 1 for Yes
    int memoryOptimizationLevel; // 1 to 3, 1 being low optimization, 3 high
    int verboseLogging; // 0 for No, 1 for Yes
} ConfigSettings;

// Function prototypes
void loadConfig(const char* configFilePath, ConfigSettings* config);
void displayConfig(const ConfigSettings* config);
void parseConfigLine(char* line, ConfigSettings* config);

int main() {
    ConfigSettings config;

    // Load configurations from file
    loadConfig("ads_config.cfg", &config);

    // Display the configurations
    displayConfig(&config);

    return 0;
}

// Function to load the configuration file
void loadConfig(const char* configFilePath, ConfigSettings* config) {
    FILE *file = fopen(configFilePath, "r");
    if (file == NULL) {
        printf("Error: Unable to open config file %s\n", configFilePath);
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];
    while (fgets(buffer, MAX_BUFFER_SIZE, file) != NULL) {
        // Skip empty lines and comments
        if (buffer[0] == '#' || buffer[0] == '\n') {
            continue;
        }
        // Parse the line and apply settings
        parseConfigLine(buffer, config);
    }

    fclose(file);
}

// Function to parse each line of the config file
void parseConfigLine(char* line, ConfigSettings* config) {
    char key[128], value[256];

    // Parse the line into key-value pairs
    sscanf(line, "%s = %s", key, value);

    // Set the appropriate configuration based on the key
    if (strcmp(key, "input_file") == 0) {
        strcpy(config->inputFilePath, value);
    } else if (strcmp(key, "output_file") == 0) {
        strcpy(config->outputFilePath, value);
    } else if (strcmp(key, "log_file") == 0) {
        strcpy(config->logFilePath, value);
    } else if (strcmp(key, "algorithm_type") == 0) {
        config->algorithmType = atoi(value); // 1 for BFS, 2 for DFS
    } else if (strcmp(key, "data_structure") == 0) {
        config->dataStructureType = atoi(value); // 1 for Tree, 2 for Graph
    } else if (strcmp(key, "enable_multithreading") == 0) {
        config->enableMultithreading = atoi(value); // 0 for No, 1 for Yes
    } else if (strcmp(key, "memory_optimization") == 0) {
        config->memoryOptimizationLevel = atoi(value); // 1 to 3
    } else if (strcmp(key, "verbose_logging") == 0) {
        config->verboseLogging = atoi(value); // 0 for No, 1 for Yes
    }
}

// Function to display the current configurations
void displayConfig(const ConfigSettings* config) {
    printf("Current Configuration Settings:\n");
    printf("--------------------------------\n");
    printf("Input File Path: %s\n", config->inputFilePath);
    printf("Output File Path: %s\n", config->outputFilePath);
    printf("Log File Path: %s\n", config->logFilePath);
    printf("Algorithm Type: %s\n", config->algorithmType == 1 ? "BFS" : "DFS");
    printf("Data Structure: %s\n", config->dataStructureType == 1 ? "Tree" : "Graph");
    printf("Multithreading Enabled: %s\n", config->enableMultithreading ? "Yes" : "No");
    printf("Memory Optimization Level: %d\n", config->memoryOptimizationLevel);
    printf("Verbose Logging: %s\n", config->verboseLogging ? "Enabled" : "Disabled");
}
