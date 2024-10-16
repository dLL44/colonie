#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include "cJSON.h"
#include "color.h"

// colonies
//  made by dLL44
//  gnu license

#define MAX_NAME_LEN 50
#define MAX_EVENT_LEN 150
#define SAVE_FILE "data/player.json"

// Cooldowns
#define COOLDOWN_BUILD_HOUSE 5
#define COOLDOWN_BUILD_TOWNHOUSE 10
#define COOLDOWN_GATHER 8
#define COOLDOWN_RANDEVENT 15

time_t lastBuiltHouse = 0;
time_t lastBuiltTownhouse = 0;
time_t lastGather = 0;
time_t lastRandevnt = 0;

struct UnsavedData
{
    char colonyName[MAX_NAME_LEN];
    int colonists;
    int wood;
    int food;
    int houses;
    int townhouses;
};

int maxColonists(struct UnsavedData *data)
{
    return (data->houses * 6) + (data->townhouses * 16);
}

void cls()
{
    system("cls");
}

void statsDisplay(struct UnsavedData *data)
{
    setColor(CCOLOR_YELLOW);
    printf("\n--- The colony of %s ---\n", data->colonyName);
    printf("\tColonists: %d out of %d max\n", data->colonists, maxColonists(data));
    printf("\tWood: %d\n", data->wood);
    printf("\tFood: %d\n", data->food);
    printf("\tHousing Total: %d\n", data->houses + data->townhouses);
    printf("\tRegular Houses: %d\n", data->houses);
    printf("\tTownhouses: %d\n", data->townhouses);
    setColor(CCOLOR_GREEN);
}

void randEvnt(struct UnsavedData *data)
{
    int eventType = rand() % 8;   // Update to allow for the new event type
    char eventMsg[MAX_EVENT_LEN]; // Ensure the buffer is large enough to hold the message

    switch (eventType)
    {
    case 0:
    {
        int destroyedHouses = rand() % (data->houses / 2 + 1); // Ensure it's valid
        data->houses -= destroyedHouses;
        setColor(CCOLOR_RED);
        snprintf(eventMsg, sizeof(eventMsg), "A storm came in, destroying %d of your houses.", destroyedHouses);
        break;
    }
    case 1:
    {
        data->food += 50;
        setColor(CCOLOR_GREEN);
        snprintf(eventMsg, sizeof(eventMsg), "One of your colonists pointed out a cache of food, possibly from an old colony.");
        break;
    }
    case 2:
    {
        int newColonists = rand()%9+3;   // Randomly add 0 to 8 colonists
        data->colonists += newColonists; // Increase colonist count
        int familyType = rand() % 8;     // Random family type
        char familyRace[15];             // Buffer for family race

        switch (familyType)
        {
        case 0:
            strcpy(familyRace, "English");
            break;
        case 1:
            strcpy(familyRace, "African");
            break;
        case 2:
            strcpy(familyRace, "German");
            break;
        case 3:
            strcpy(familyRace, "Irish");
            break;
        case 4:
            strcpy(familyRace, "Dutch");
            break;
        case 5:
            strcpy(familyRace, "Swedish");
            break;
        case 6:
            strcpy(familyRace, "Finnish");
            break;
        default:
            strcpy(familyRace, "Nordic");
            break; // Fallback to "Nordic" if no match
        }
        setColor(CCOLOR_GREEN);
        snprintf(eventMsg, sizeof(eventMsg), "A(n) %s family came to your colony, saying (translated by a %s colonist) that the colony of %s was heard to be great. +%d colonists.", familyRace, familyRace, data->colonyName, newColonists);
        break;
    }
    case 3:
    {
        setColor(CCOLOR_YELLOW);
        snprintf(eventMsg, sizeof(eventMsg), "Nothing unusual happened. You've been spared by nature.");
        break;
    }
    case 4:
    {
        int foodDistributed = rand() % (data->colonists / 2 + 1); // Randomly distribute up to half the colonists' count
        if (data->food >= foodDistributed)
        {
            setColor(CCOLOR_GREEN);
            data->food -= foodDistributed; // Deduct the food distributed
            snprintf(eventMsg, sizeof(eventMsg), "Your colonists are hungry, you distribute %d food.", foodDistributed);
        }
        else
        {
            int killedOff = rand() % 23;
            data->colonists -= killedOff;
            setColor(CCOLOR_RED);
            snprintf(eventMsg, sizeof(eventMsg), "Your colonists are hungry, but you don't have enough food to distribute, %d die.", killedOff);
        }
        break;
    }
    case 5:
    {
        int newHouses = rand() % 6 + 1;
        int foodGift = newHouses>3 ? 24 : 18;
        setColor(CCOLOR_GREEN);
        snprintf(eventMsg, sizeof(eventMsg), "A good samaritan colonist decides to build %d houses for you, you thank him with %d food.", newHouses, foodGift);
        break;
    }
    case 6:
        int gatheredWood = rand() % 31;
        int gatheredFood = rand() % 21;
        data->wood += gatheredWood;
        data->food += gatheredFood;
        setColor(CCOLOR_GREEN);
        snprintf(eventMsg, sizeof(eventMsg), "A handful of your colonists decided to gather for the colony, bringing back %d wood and %d food", gatheredWood, gatheredFood);
        break;
    case 7:
        int coinToss = rand()% 2 + 1;
        if (coinToss == 1)
        {
            int warLoss = rand()%data->colonists/4;
            setColor(CCOLOR_YELLOW);
            snprintf(eventMsg, sizeof(eventMsg), "A group of Indians come to raid, you resist and win, losing %d colonist lives", warLoss);
            data->colonists -= warLoss;
        } else if (coinToss == 2)
        {
            int warLoss = rand()%data->colonists/2;
            setColor(CCOLOR_RED);
            snprintf(eventMsg, sizeof(eventMsg), "A group of Indians come to raid, you resist but lose, losing %d colonist lives and half of your resources", warLoss);
            data->colonists -= warLoss;
            data->food -= data->food/2;
            data->wood -= data->wood/2;
        }
        break;
    default:
        setColor(CCOLOR_MAGENTA);
        snprintf(eventMsg, sizeof(eventMsg), "Something happened, but nothing was affected... weird.");
        break;
    }

    printf("\n** RANDOM EVENT: %s **\n", eventMsg);
    setColor(CCOLOR_RESET);
}

void autoRandEvent(struct UnsavedData *data)
{
    printf("Press 'q' to stop nature.\n");

    while (1)
    {
        randEvnt(data);
        Sleep(2000); // Wait for 2 seconds before the next event

        // Check for keyboard input
        if (_kbhit())
        {
            char ch = _getch(); // Get the pressed key
            if (ch == 'q')
            {
                break; // Exit the loop if 'q' is pressed
            }
        }
    }
}

void autoLumber(struct UnsavedData *data)
{
    printf("Press 'q' to stop lumberjacking.\n");

    while (1)
    {
        int gatheredWood = rand() % 100 + 25; // Randomly gather between 25 and 124 wood
        data->wood += gatheredWood;
        printf("You chopped %d wood.\n", gatheredWood);
        Sleep(20000); // Wait for 20 seconds before gathering again

        // Check for keyboard input
        if (_kbhit())
        {
            char ch = _getch(); // Get the pressed key
            if (ch == 'q')
            {
                printf("Stopped lumberjacking.\n");
                break; // Exit the loop if 'q' is pressed
            }
        }
    }
}
void buildHouse(struct UnsavedData *data)
{
    if (data->wood >= 10)
    {
        data->houses++;
        data->wood -= 10;
        setColor(CCOLOR_YELLOW);
        printf("You built one house, you now have %d.\n", data->houses);
        setColor(CCOLOR_GREEN);
    }
    else
    {
        setColor(CCOLOR_RED);
        printf("You don't have enough wood.\n");
        setColor(CCOLOR_GREEN);
    }
}

void buildTownhouse(struct UnsavedData *data)
{
    if (data->wood >= 20)
    {
        data->townhouses++;
        data->wood -= 20;
        setColor(CCOLOR_YELLOW);
        printf("You built one townhouse, you now have %d.\n", data->townhouses);
        setColor(CCOLOR_GREEN);
    }
    else
    {
        setColor(CCOLOR_RED);
        printf("You don't have enough wood.\n");
        setColor(CCOLOR_GREEN);
    }
}

void builder(int buildType, struct UnsavedData *data)
{
    printf("Press 'q' to stop the builder.\n");

    while (1)
    {
        if (buildType == 1)
        {
            buildHouse(data);
            Sleep(5000);
        } else if (buildType == 2)
        {
            buildTownhouse(data);
            Sleep(10000);
        } else
        {
            setColor(CCOLOR_RED);
            perror("Internal Game Error: buildType is neither 1 or 2.");
            setColor(CCOLOR_GREEN);
            break;
        }
        // Check for keyboard input
        if (_kbhit())
        {
            char ch = _getch(); // Get the pressed key
            if (ch == 'q')
            {
                break; // Exit the loop if 'q' is pressed
            }
        }
    }
}

// Save game state to JSON
void saveGame(struct UnsavedData *data)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "colonyName", data->colonyName);
    cJSON_AddNumberToObject(json, "colonists", data->colonists);
    cJSON_AddNumberToObject(json, "wood", data->wood);
    cJSON_AddNumberToObject(json, "food", data->food);
    cJSON_AddNumberToObject(json, "houses", data->houses);
    cJSON_AddNumberToObject(json, "townhouses", data->townhouses);

    FILE *file = fopen(SAVE_FILE, "w");
    if (file)
    {
        char *jsonString = cJSON_Print(json);
        fprintf(file, "%s", jsonString);
        free(jsonString);
        fclose(file);
        printf("Game saved successfully.\n");
    }
    else
    {
        printf("Error saving game.\n");
    }

    cJSON_Delete(json);
}

// Load game state from JSON
void loadGame(struct UnsavedData *data)
{
    FILE *file = fopen(SAVE_FILE, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *jsonString = malloc(length + 1);
        fread(jsonString, 1, length, file);
        jsonString[length] = '\0'; // Null-terminate the string

        cJSON *json = cJSON_Parse(jsonString);
        if (json)
        {
            strcpy(data->colonyName, cJSON_GetObjectItem(json, "colonyName")->valuestring);
            data->colonists = cJSON_GetObjectItem(json, "colonists")->valueint;
            data->wood = cJSON_GetObjectItem(json, "wood")->valueint;
            data->food = cJSON_GetObjectItem(json, "food")->valueint;
            data->houses = cJSON_GetObjectItem(json, "houses")->valueint;
            data->townhouses = cJSON_GetObjectItem(json, "townhouses")->valueint;

            // Ensure colonists don't exceed max allowed
            if (data->colonists > maxColonists(data))
            {
                data->colonists = maxColonists(data);
            }

            printf("Game loaded successfully.\n");
        }
        else
        {
            printf("Error loading game data.\n");
        }

        free(jsonString);
        fclose(file);
    }
    else
    {
        printf("No saved game found. Starting a new game.\n");
    }
}

int main()
{
    srand(time(NULL));             // Seed for random events
    struct UnsavedData data = {0}; // Initialize the structure

    // Load game data if available
    loadGame(&data);

    setColor(CCOLOR_GREEN);
    // If it's a new game, initialize game data
    if (data.colonists == 0)
    {
        printf("Set your colony name\n> ");
        fgets(data.colonyName, sizeof(data.colonyName), stdin);
        data.colonyName[strcspn(data.colonyName, "\n")] = 0;
        data.colonists = 200;
        data.wood = 50;  // Starting materials
        data.food = 100; // Starting food
        data.houses = 0; // Starting buildings
        data.townhouses = 0;

        Sleep(500);
        printf("Welcome to the 1800s, leader of %s.\n", data.colonyName);
        Sleep(1000);
        printf("Rules are simple, ");
        Sleep(1000);
        printf("200 colonists, ");
        Sleep(1000);
        printf("make houses if your colonists goes over max or they die, ");
        Sleep(1000);
        printf("keep your colony alive.\n");
        Sleep(1000);
        printf("Good luck.\n");
    }

    char command[20];

    while (1)
    {
        time_t currentTime = time(NULL); // Update current time at the start of the loop
        statsDisplay(&data);
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline
        
        if (strcmp(command, "build house") == 0)
        {
            cls();
            if (difftime(currentTime, lastBuiltHouse) >= COOLDOWN_BUILD_HOUSE)
            {
                buildHouse(&data);
                lastBuiltHouse = currentTime; // Update last executed time
            }
            else
            {
                int remainingTime = (int)(COOLDOWN_BUILD_HOUSE - difftime(currentTime, lastBuiltHouse));
                if (remainingTime > 0)
                {
                    setColor(CCOLOR_YELLOW);
                    printf("You must wait %d seconds before building another house.\n", remainingTime);
                    setColor(CCOLOR_GREEN);
                }
            }
        }
        else if (strcmp(command, "build townhouse") == 0)
        {
            cls();

            if (difftime(currentTime, lastBuiltTownhouse) >= COOLDOWN_BUILD_TOWNHOUSE)
            {
                buildTownhouse(&data);
                lastBuiltTownhouse = currentTime; // Update last executed time
            }
            else
            {
                int remainingTime = (int)(COOLDOWN_BUILD_HOUSE - difftime(currentTime, lastBuiltTownhouse));
                if (remainingTime > 0)
                {
                    setColor(CCOLOR_YELLOW);
                    printf("You must wait %d seconds before building another townhouse.\n", remainingTime);
                    setColor(CCOLOR_GREEN);
                }
            }
        }
        else if (strcmp(command, "builder house") == 0)
        {
            cls();
            builder(1, &data);
        }
        else if (strcmp(command, "builder townhouse") == 0)
        {
            cls();
            builder(2, &data);
        }
        else if (strcmp(command, "gather") == 0)
        {
            cls();
            if (difftime(currentTime, lastGather) >= COOLDOWN_GATHER)
            {
                int gatheredWood = rand() % 31;
                int gatheredFood = rand()%21;
                data.wood += gatheredWood;
                data.food += gatheredFood;
                printf("You gathered %d wood and %d food.\n", gatheredWood, gatheredFood);
                lastGather = currentTime; // Update last executed time
            }
            else
            {
                int remainingTime = (int)(COOLDOWN_BUILD_HOUSE - difftime(currentTime, lastGather));
                if (remainingTime > 0)
                {
                    setColor(CCOLOR_YELLOW);
                    printf("You must wait %d seconds before gathering.\n", remainingTime);
                    setColor(CCOLOR_GREEN);
                }
            }
        }
        else if (strcmp(command, "lumber") == 0)
        {
            cls();
            autoLumber(&data);
        }
        else if (strcmp(command, "randevent") == 0)
        {
            cls();
            if (difftime(currentTime, lastRandevnt) >= COOLDOWN_RANDEVENT)
            {
                randEvnt(&data);
                lastRandevnt = currentTime; // Update last executed time
            }
            else
            {
                int remainingTime = (int)(COOLDOWN_BUILD_HOUSE - difftime(currentTime, lastRandevnt));
                if (remainingTime > 0)
                {
                    setColor(CCOLOR_YELLOW);
                    printf("You must wait %d seconds before having nature take its course.\n", remainingTime);
                    setColor(CCOLOR_GREEN);
                }
            }
        }
        else if (strcmp(command, "let nature") == 0)
        {
            cls();
            autoRandEvent(&data);
        }
        else if (strcmp(command, "save") == 0)
        {
            cls();
            saveGame(&data);
        }
        else if (strcmp(command, "quit") == 0)
        {
            cls();
            saveGame(&data);
            setColor(CCOLOR_YELLOW);
            printf("Goodbye!\n");
            setColor(CCOLOR_RESET);
            break;
        }
        else if (strcmp(command, "help") == 0 || strcmp(command, "?") == 0)
        {
            cls();
            setColor(CCOLOR_YELLOW);
            printf("You ask your assistant, Ferdiand, how does one \"play this game\"\n");
            printf("He replies:\n\tbuild house\t\tbuild one house\n\tbuild townhouse\t\tbuild one townhouse\n\tbuilder house\t\tauto build houses\n\tbuilder townhouse\tauto build townhouses\n\tgather\t\t\tgather wood and food\n\tlumber\t\t\tstart lumberjacking (high wood output, more time)\n\trandevent\t\tlet nature do one event\n\tlet nature\t\tlet nature run its course until you press \"q\"\n\tsave\t\t\tsave game\n\tquit\t\t\tquit\n");
            setColor(CCOLOR_GREEN);
        }
        else
        {
            cls();
            setColor(CCOLOR_RED);
            printf("Unknown command.\n");
            setColor(CCOLOR_GREEN);
        }
        
        if (data.colonists <= 0)
        {
            setColor(CCOLOR_RED);
            printf("Your colony has perished. You are left by yourself, and a pack of angry wolfes gather around you.");
            Sleep(2000);
            setColor(CCOLOR_CYAN);
            printf(" ______     ______     __    __     ______        ______     __   __   ______     ______    \n");
            printf("/\\  ___\\   /\\  __ \\   /\\ \"-./  \\   /\\  ___\\      /\\  __ \\   /\\ \\ / /  /\\  ___\\   /\\  == \\   \n");
            printf("\\ \\ \\__ \\  \\ \\  __ \\  \\ \\ \\-./\\ \\  \\ \\  __\\      \\ \\ \\/\\ \\  \\ \\ \\'/   \\ \\  __\\   \\ \\  __<   \n");
            printf(" \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_\\ \\ \\_\\  \\ \\_____\\     \\ \\_____\\  \\ \\__|    \\ \\_____\\  \\ \\_\\ \\_\\ \n");
            printf("  \\/_____/   \\/_/\\/_/   \\/_/  \\/_/   \\/_____/      \\/_____/   \\/_/      \\/_____/   \\/_/ /_/ \n");
            printf("\n");
            setColor(CCOLOR_RESET);
        }
    }

    return 0;
}
// 569 lines of bullshit
