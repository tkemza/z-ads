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

void sysclear() {
    system("clear");
}

int main(int argc, const char *argv[]) {
    sysclear();
    // Print message indicating the start of the update process
    printf("Entering subsystem and running update...\n");

    // Run the 'sudo apt update' command to update the package list on Ubuntu
    int ret = system("sudo apt-get update -y");

    // Check if the command ran successfully
    if (ret == 0) {
        printf("Update command executed successfully.\n");
    } else {
        printf("Failed to execute update command. Error code: %d\n", ret);
    }

    // Exit code set to 0
    return EXIT_SUCCESS;
}