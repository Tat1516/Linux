#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool isValidIP(const char *ip) {
    unsigned int octets[4];
    int numParsed = sscanf(ip, "%u.%u.%u.%u", &octets[0], &octets[1], &octets[2], &octets[3]);
    
    if (numParsed != 4) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (octets[i] > 255) {
            return false;
        }
    }

    // Check for special/reserved addresses
    if (octets[0] == 0 || octets[0] == 127 || octets[0] >= 224) {
        return false; // Not a standard unicast address
    }

    return true;
}

void findClassAndDetails(const char *ip) {
    unsigned int octets[4];
    sscanf(ip, "%u.%u.%u.%u", &octets[0], &octets[1], &octets[2], &octets[3]);

    char classType;
    char netID[16], defaultMask[16], broadcastID[16];

    if (octets[0] >= 1 && octets[0] <= 126) {
        classType = 'A';
        sprintf(netID, "%u.0.0.0", octets[0]);
        strcpy(defaultMask, "255.0.0.0");
        sprintf(broadcastID, "%u.255.255.255", octets[0]);
    } else if (octets[0] >= 128 && octets[0] <= 191) {
        classType = 'B';
        sprintf(netID, "%u.%u.0.0", octets[0], octets[1]);
        strcpy(defaultMask, "255.255.0.0");
        sprintf(broadcastID, "%u.%u.255.255", octets[0], octets[1]);
    } else if (octets[0] >= 192 && octets[0] <= 223) {
        classType = 'C';
        sprintf(netID, "%u.%u.%u.0", octets[0], octets[1], octets[2]);
        strcpy(defaultMask, "255.255.255.0");
        sprintf(broadcastID, "%u.%u.%u.255", octets[0], octets[1], octets[2]);
    } else if (octets[0] >= 224 && octets[0] <= 239) {
        classType = 'D'; // Multicast addresses
        strcpy(netID, "N/A");
        strcpy(defaultMask, "N/A");
        strcpy(broadcastID, "N/A");
    } else if (octets[0] >= 240 && octets[0] <= 255) {
        classType = 'E'; // Experimental addresses
        strcpy(netID, "N/A");
        strcpy(defaultMask, "N/A");
        strcpy(broadcastID, "N/A");
    } else {
        printf("Invalid IP address.\n");
        return;
    }

    printf("IP Address: %s\n", ip);
    printf("Class: %c\n", classType);
    printf("Net ID: %s\n", netID);
    printf("Default Mask: %s\n", defaultMask);
    printf("Broadcast ID: %s\n", broadcastID);
}

int main() {
    char ipAddress[17]; // Increased to 17 to safely hold 15 chars + null terminator
    
    printf("Enter an IPv4 address (e.g., 192.168.1.1): ");
    scanf("%16s", ipAddress);

    if (!isValidIP(ipAddress)) {
        printf("Invalid IP address format or reserved address.\n");
        return 1;
    }

    findClassAndDetails(ipAddress);

    return 0;
}
