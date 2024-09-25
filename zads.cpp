/*Including libraries*/

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <wincrypt.h>
#include <time.h>
#include <shlwapi.h>
#include <conio.h> // For _getch()
#include <process.h> // For Multithreading
#include <zlib.h> // For data compression and decompression
#include <queue> // For task queue
#include <pthread.h> // For thread pool
#include <synchapi.h> // For secure memory headling

/*Pragma comments*/

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "pthreadVC2.lib") // Ensure pthread library is linked

/*Define attributes*/

#define ADS_SIZE_LIMIT 1024 * 1024 // 1 MB limit for ADS
#define LOG_FILE "ads_log.txt"
#define DEFAULT_ENCRYPTION_KEY "SuperSecretKey123" // Default AES encryption key (must be 16 bytes for AES-128)
#define AES_BLOCK_SIZE 16 // AES block size for 128-bit encryption
#define CONFIG_FILE "ads_config.ini" // Configuration file
#define BUFFER_SIZE 8192 // Buffer size for file operations
#define MAX_RETRIES 3 // Maximum number of retries for failed operations
#define THREAD_POOL_SIZE 4 // Number of threads in the pool 

/*HANDLE and Queue tasks*/

HANDLE logMutex;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queueCond = PTHREAD_COND_INITIALIZER;
std::priority_queue<std::pair<int, void (*)()>> taskQueue; // Priority queue for task management

int currentThreadCount = 0; // Current number of active threads

// Configuration options
char encryptionKey[256] = DEFAULT_ENCRYPTION_KEY; // AES encryption key from config
int logLevel = 1; // Default log level (1: errors only)

// Function to log messages with timestamp
void logMessage(const char* message, int level) {
    if (level <= logLevel) {
        WaitForSingleObject(logMutex, INFINITE);
        FILE* logFile = fopen(LOG_FILE, "a");
        if (logFile != NULL) {
            time_t currentTime = time(NULL);
            fprintf(logFile, "[%s]%s\n", ctime(&currentTime), message);
            fclose(logFile);
        }
        ReleaseMutex(logMutex);
    }
}

// Function to log events to the Windows Event Log
void logEvent(const char* message, WORD eventType) {
    HANDLE hEventLog = RegisterEventSource(NULL, "ADS_Tool");
    if (hEventLog != NULL) {
        ReportEvent(hEventLog, eventType, 0, 0, NULL, 1, 0, &message, NULL);

        DeregisterEventSource(hEventLog);
    }
}

// Function to load configuration from a file
void loadConfiguration() {
    FILE* configFile = fopen(CONFIG_FILE, "r");
    if (configFIle == NULL) {
        logMessage("Configuration file not found. Using default settings.", 2);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), configFile) != NULL) {
        if (strncmp(line, "ENCRYPTION_KEY=", 15) == 0) {
            strncpy(encryptionKey, line + 15, sizeof(encryptionKey) - 1);

            encryptionKey[strcspn(encryptionKey, "\r\n")] = '\0';
        } else if (strncmp(line, "LOG_LEVEL=", 10) == 0) {
            logLevel = atoi(line + 10);
        }
    }

    fclose(configFile);
    logMessage("Configuration loaded successfully.", 2);
}

// Function to compress data using zlib
int compressData(const BYTE* data, DWORD dataSize, BYTE* compressedData, DWORD* compressedDataSize) {
    uLongf destLen = *compressedDataSize;
    int result = compress(compressedData, *destLen, data, dataSize);
    *compressedDataSize = destLen;
    return result == Z_OK;
}

// Function to decompress data using zlib
int decompressData(const BYTE* compressedData, DWORD compressedDataSize, BYTE* data, DWORD* dataSize) {
    uLongf destLen = *dataSize;
    int result = uncompressed(data, *destLen, compressData, compressedDataSize);
    *dataSize = destLen;
    return result == Z_OK;
}

// Function to performe AES encryptior with secure memory handling
int aesEncrypt(const BYTE* data, DWORD dataSize, BYTE* encryptedData, DWORD* encryptedDataSize) {
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    HCRYPTHASH hHash = 0;
    BYTE* securedKey = (BYTE*)VirtualAlloc(NULL, strlen(encryptionKey), MEM_COMMIT, PAGE_READWRITE);

    if (secureKey == NULL) {
        logMessage("Memory allocation for secure key failed.", 1);
        return 0;
    }

    memcpy(secureKey, encryptionKey, strlen(encryptionKey)); // Lock key in memory

    if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0)) {
        logMessage("CryptAcquireContext failed.", 1);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        logMessage("CryptCreateHash failed.", 1);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptHashData(hHash, secureKey, strlen(encryptionKey), 0)) {
        logMessage("CryptHashData failed.", 1);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, CRYPT_EXPORTABLE, &hKey)) {
        logMessage("CryptDeriveKey failed.", 1);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    memcpy(encryptedData, data, dataSize);
    *encryptedDataSize = dataSize;

    if (!CryptEncrypt(hKey, 0, TRUE, 0, encryptedData, encryptedDataSize, dataSize + AES_BLOCK_SIZE)) {
        logMessage("CryptEncrypt failed.", 1);
        CryptDestroyKey(hKey);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    VirtualUnlock(secureKey, strlen(encryptionKey));
    VirtualFree(secureKey, 0, MEM_RELEASE);
    return 1;
}

// Function to perform AES decryptior with secure memory handling
int aesDecrypt(const BYTE* encryptedData, DWORD encryptedDataSize, BYTE* decryptedData, DWORD decryptedDataSize) {
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    HCRYPTHASH hHash = 0;
    BYTE* securedKey = (BYTE*)VirtualAlloc(NULL, strlen(encryptionKey), MEM_COMMIT, PAGE_READWRITE);

    if (secureKey == NULL) {
        logMessage("Memory allocation for secure key failed.", 1);
        return 0;
    }

    memcpy(secureKey, encryptionKey, strlen(encryptionKey)); // Lock key in memory
    VirtualLock(secureKey, strlen(encryptionKey)); // Lock key in memory 

    if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, 0)) {
        logMessage("CryptAcquireContext failed.", 1);
        VirtualLock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        logMessage("CryptCreateHash failed.", 1);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptHashData(hHash, secureKey, strlen(encryptionKey), 0)) {
        logMessage("CryptHashData failed.", 1);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, CRYPT_EXPORTABLE, &hKey)) {
        logMessage("CryptDeriveKey failed.", 1);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    memcpy(decryptedData, encryptedData, encryptedDataSize);
    *decryptedDataSize = encryptedDataSize;

    if (!CryptDecrypt(hKey, 0, TRUE. 0, decryptedData, decryptedDataSize)) {
        logMessage("CryptDecrypt failed.", 1);
        CryptDestroyKey(hKey);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        VirtualUnlock(secureKey, strlen(encryptionKey));
        VirtualFree(secureKey, 0, MEM_RELEASE);
        return 0;
    }

    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    VirtualUnlock(secureKey, strlen(encryptionKey));
    VirtualFree(secureKey, 0, MEM_RELEASE);
    return 1;
}

// Function to check if the filesystem is NFTS
int isNTFSFilesystem(const char* path) {
    char rootPath[MAX_PATH];
    strncpy(rootPath, path, 3);
    rootPath[3] = '\0';
    char fileSystemName[MAX_PATH];
    
    if (!GetVolumeInformationA(rootPath, NULL, 0, NULL, NULL, NULL, fileSystemName, sizeof(fileSystemName))) {
        logMessage("Error getting filesystem information.", 1);
        return 0;
    }
    return strcmp(fileSystemName, "NTFS") == 0;
}

// Function to check if ADS exists
int doesADSExist(const char* filename, const char* streamname) {
    char fullpath[MAX_PATH];
    snprintf(fullpath, sizeof(fullpath), "%s:%s", filename, streamname);
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(fullpath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return 1;
    }
    return 0;
}

// Function to read and decrypt data from an ADS with retries
int readFromADS(const char* filename, const char* streamname) {
    int retryCount = 0;

    while (retryCount < MAX_RETRIES) {
        char fullpath[MAX_PATH];
        snprintf(fullpath, sizeof(fullpath), "%s:%s", filename, streamname);

        HANDLE hFile = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (hFile == INVALID_HANDLE_VALUE) {
        logMessage("Error opening ADS for reading.", 1);
        retryCount++;
        Sleep(1000 * retryCount); // Exponental backoff
        continue;
    }

    BYTE encryptedData[ADS_SIZE_LIMIT + AES_BLOCK_SIZE];
    DWORD bytesRead;

    if (!ReadFile(hFile, encryptedData, sizeof(encryptedData), &bytesRead, NULL)) {
        logMessage("Error reading from ADS.", 1);
        CloseHandle(hFile);
        retryCount++;
        Sleep(1000 * retryCount); // Exponental backoff
        continue;
    }

    CloseHandle(hFile);

    BYTE decompressedData[ADS_SIZE_LIMIT];
    DWORD decompressedDataSize = sizeof(decompressedData);

    if (!decompressData(encryptedData, bytesRead, decompressedData, &decompressedDataSize)) {
        logMessage("Error decrypting data from ADS.", 1);
        retryCount++;
        Sleep(1000 * retryCount); // Exponental backoff
        continue;
    }

    decryptedData[decryptedDataSize] = '\0';
    printf("Decrypted data from ADS: %s\n", decryptedData);
    return 1;
    logMessage("Failed to read data from ADS after multiple retries.", EVENTLOG_ERROR_TYPE);
    return 0;
}

// Function to list all ADS for a file
int listADS(const char* filename) {
    char searchPatter[MAX_PATH];
    snprintf(searchPatter, sizeof(searchPatter), "%s:*", filename);

    WIN32_FIND_STREAM_DATA findStreamData;
    HANDLE hFind = FindFirstStreamW((LPCWSTR)searchPattern, FindStreamInfoStandard, &findStreamData, 0);

    if (hFind == INVALID_HANDLE_VALUE) {
        logMessage("Error listing ADS.", 1);
        return 0;
    }

    do {
        printf("Found ADS: %ws\n", findStreamData.cStreamName);
    } while (FindNextStreamW(hFind, &findStreamData));

    FindClose(hFind);
    return 1;
}

// Function to store encrypted and compressed data in ADS and verify integrity
int storeStringADS(const char* filename, const char* streamname, const char* data) {
    if (GetFileAttributesA(filename) == INVALID_FILE_ATTRIBUTES) {
        logMessage("Base file does not exist.", 1);
        return 1;
    }

    if (!isNTFSFilesystem(filename)) {
        logMessage("The file is not on an ETFS filesystem.", 1);
        return 2;
    }

    size_t dataSize = strlen(data);
    if (dataSize > ADS_SIZE_LIMIT) {
        logMessage("Data exceeds the size limit for ADS.", 1);
        return 3;
    }

    if(doesADSExist(filename, streamname)) {
        printf("ADS already exists. Overwrite? (y/n): ");
        char response = _getch();
        if (tolower(response) != 'y') {
            printf("\nOperation aborted.\n");
            return 0;
        }
        printf("\n"); 
    }

    BYTE compressedData[ADS_SIZE_LIMIT];
    DWORD compressedDataSize = sizeof(compressedData);
    
    if (!compressedData((BYTE*) data, dataSize, compressedData, compressedDataSize)) {
        logMessage("Data compression failed.", 1);
        return 4;
    }

    BYTE encryptedData[ADS_SIZE_LIMIT + AES_BLOCK_SIZE];
    DWORD encryptedDataSize = sizeof(encryptedData);

    if (!aesEncrypt(compressedData, compressedDataSize, encryptedData, encryptedDataSize)) {
        logMessage("Encryption failed.", 1);
        return 5;
    }

    char fullpath[MAX_PATH];
    snprintf(fullpath, sizeof(fullpath), "%s:%s", filename, streamname);

    HANDLE hFile = CreateFileA(fullpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        logMessage("Error creating file with ADS.", 1);
        return 6;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile, encryptedData, encryptedDataSize, &bytesWritten, NULL) || bytesWritten != encryptedDataSize) {
        logMessage("Error writing to file with ADS.", 1);
        CloseHandle(hFile);
        return 7;
    }

    CloseHandle(hFile);

    // Read back and verify data integrity
    BYTE readData[ADS_SIZE_LIMIT + AES_BLOCK_SIZE];
    DWORD readSize = encryptedDataSize;

    if  (!aesDecrypt(encryptedData, encryptedDataSize, readData, &readSize) || readSize != compressedDataSize || memcmp(compressedData, readData, compressedDataSize) != 0) {
        logMessage("Data integrity verification failed.", 1);
        return 8;
    }

    logMessage("Data successfully written to the ADS and verified", 2);
    return 0;

}

// Function to parse command-line arguments
int parseArguments(int argc, char* argv[], char* filename, char* streamname, char* data) {
    if (argc < 4) {
        printf("Usage: %s <filename> <streamname> <data>\n", argv[0]);
        return 0;
    }

    strncpy(filename, argv[1], MAX_PATH);
    strncpy(streamname, argv[2], MAX_PATH);
    strncpy(data, argv[3], ADS_SIZE_LIMIT);

    return 1;
}

// Function to print help information
void printHelp() {
    printf("Usage: ads_tool <command> <filename> <streamname> [data]\n");
    printf("Commands:\n");
    printf("    write <filename> <streamname> <data>    Write data to ADS.\n");
    printf("    read <filename> <streamname>            Read data from ADS.\n");
    printf("    list <filename>                         List all ADS for a file.\n");
    printf("    batch <command> <filelist> <streamname> <data>  Batch process files.\n");
    printf("    --help                                  Show this help message.\n");
}

// Function to handle batch processing of multiple files
void handleBatchOperation(const char* command, const char* fileList, const char* streamname, const char* data) {
    FILE* file = fopen(fileList, "r");
    if (file == NULL) {
        logMessage("Failed to open file list for batch processing.", 1);
        printf("Failed to open file list: %s\n", fileList);
        return;

        char filename[MAX_PATH];
        int totalFiles = 0;
        int processedFiles = 0;
        while (fgets(filename, sizeof(filename), file)) {
            filename[strcspn(filename, "\r\n")] = '\0'; // Remove new line character

            totalFiles++;
        }
        rewind(file); // Reset file pointer to the beginning 

        while (fgets(filename, sizeof(filename), file)) {
            filename[strcspn(filename, "\r\n")] = '\0'; // Remove new line character

            processedFiles++;

            if (strcmp(command, "write") == 0) {
                int result = storeStringADS(filename, streamname, data);
                
                if (result == 0) {
                    printf("Data successfully written to the ADS for file: %s\n", filename);
                } else {
                    printf("Failed to write data to the ADS for file: %s. Error code: %d\n", filename, result);
                }
            } else if (strcmp(command, "read") == 0) {
                if (!readFromADS(filename, streamname)) {
                    printf("Failed to read data from ADS for file: %s\n", filename);
                }
            } else if (strcmp(command, "list") == 0) {
                if (!listADS(filename)) {
                    printf("Failed to list ADS for file: %s\n", filename);
                }
            }
            printf("Progress: %d/%d files processed\n", processedFiles, totalFiles);
        }
        fclose(file);
    }
}

// Function for multithreading batch operations
void batchThread(void* args) {
    char** batchArgs = (char**)args;

    handleBatchOperation(batchArgs[0], batchArgs[1], batchArgs[2], batchArgs[3]);
    _endthread();
}

// Thread pool worker function
void* threadPoolWorker(void* arg) {
    while (1) {
        std::pair<int, void (*)()> task;

        pthread_mutex_lock(&queueMutex);
        while (taskQueue.empty()) {
            pthread_cond_wait(&queueCond, &queueMutex);
        }
        task = taskQueue.top();
        taskQueue.pop();

        pthread_mutex_unlock(&queueMutex);

        if (task.second != NULL) {
            task.second();
        }
        return NULL;
    }
}

// Function to initialize thread pool
void initThreadPool() {
    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, threadPoolWorker, NULL);
    }
}

// Function to add tasj for thread pool with priority
void addTaskToThreadPool(void (*task)(), int priority) {
    pthread_mutex_lock(&queueMutex);
    taskQueue.push(std::make_pair(priority, task));
    pthread_cond_signal(&queueCond);
    pthread_mutex_unlock(&queueMutex);
}

// Main function
int main(int argc, char* argv[]) {
    logMutex = CreateMutex(NULL, FALSE, NULL);
    loadConfiguration();
    initThreadPool();

    if (argc < 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        printHelp();
        return 0;
    }

    char filename[MAX_PATH];
    char streamname[MAX_PATH];
    char data[ADS_SIZE_LIMIT];

    if (strcmp(argv[1], "write") == 0) {
        if (!parseArguments(argc - 1, argc + 1, filename, streamname, data)) {
            logMessage("Invalid arguments.", 1);
            return 1;
        }
        int result = storeStringADS(filename, streamname, data);
        if (result == 0) {
            printf("Data successfully written to the ADS and verified!\n");
        } else {
            printf("Failed to write data to ADS. Error code: %d\n", result);
        }
    } else if (strcmp(argv[1], "read") == 0) {
        if (argc != 4) {
            printHelp();
            return 1;
        }
        strncpy(filename, argv[2], MAX_PATH);
        strncpy(streamname, argv[3], MAX_PATH);
        if (!readFromADS(filename, streamname)) {
            printf("Failed to read data from the ADS.\n");
        }
    } else if (strcmp(argv[1], "list") == 0) {
        if (argc != 3) {
            printHelp();
            return 1;
        }
        strncpy(filename, argv[2], MAX_PATH);
        if (!listADS(filename)) {
            printf("Failed to list ADS for the file.\n");
        }
    } else if (strcmp(argv[1], "batch") == 0) {
        if (argc != 5) {
            printHelp();
            return 1;
        }
        char* batchArgs[4] = {argv[2], argv[3], argv[4], data};
        
        addTaskToThreadPool(batchThread, 1); // Add batch task to priority
    } else {
        printHelp();
        return 1;
    }

    CloseHandle(logMutex);
    return 0;
}