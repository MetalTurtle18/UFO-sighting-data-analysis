#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACER "--------------------------------------------\n"

#define MAX_CITY 70 // 69 characters is the longest city name
#define MAX_SHAPE 10 // 9 characters is the longest shape name
#define MAX_COMMENT 236 // 235 characters is the longest comment
#define MAX_MENU_OPTION 50
#define MAX_SEARCH_RESULTS 10

typedef struct date { // MM/DD/YYYY format in csv
    int year;
    int month;
    int day;
} date;

typedef struct dateTime {
    date date;
    int hour;
    int minute;
} dateTime;

typedef struct sightingNode {
    dateTime dateTime;
    char city[MAX_CITY];
    char state[3];
    char country[3];
    char shape[MAX_SHAPE];
    int duration;
    char comment[MAX_COMMENT];
    date dateReported;
    double latitude;
    double longitude;
    struct sightingNode *next;
} sightingNode;

typedef int (*stringPredicate)(sightingNode *, char s[]);

typedef int (*datePredicate)(sightingNode *, date);

void readStringToSpacer(FILE *csv, char string[]);

void freeData(sightingNode *head);

void sortByDate(sightingNode **head, int size);

void printList(sightingNode *head, int maxNodes);

void printArray(sightingNode *arr[], int size);

void printNode(sightingNode *node);

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2);

int loadData(char fileName[], sightingNode *head);

int contains(char c, char arr[], int len);

int datecmp(date d1, date d2);

void lookAhead(sightingNode **node, int steps);

void searchByString(sightingNode **results, sightingNode *head, stringPredicate predicate, char string[]);

void searchByDate(sightingNode **results, sightingNode *head, datePredicate predicate, date d);

int shapePredicate(sightingNode *node, char *shape);

int cityPredicate(sightingNode *node, char *shape);

int statePredicate(sightingNode *node, char *shape);

int countryPredicate(sightingNode *node, char *shape);

int dateOccurredPredicate(sightingNode *node, date d);

int dateReportedPredicate(sightingNode *node, date d);

void addEntry(sightingNode **head, sightingNode *node);

void removeEntry(sightingNode **node);

void getFileName(char fileName[]);

int saveData(sightingNode *head);

void saveNode(FILE *file, sightingNode *node);

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption);

int main(void) {
    // DECLARE MENUS
    char menuInput;
    char mainMenu[][MAX_MENU_OPTION] = {"View more", "Sort", "Filter", "Return to top", "Add", "Delete", "Save",
                                        "Quit"};
    char mainMenuOptions[] = {'v', 'o', 'f', 'c', 'a', 'r', 's', 'q'};
    char sortMenu[][MAX_MENU_OPTION] = {"Date", "City", "State", "Country", "Shape", "Duration", "Date reported",
                                        "Reverse sorting"};
    char sortMenuOptions[] = {'d', 't', 's', 'c', 'h', 'u', 'r', 'f'};
    char filterMenu[][MAX_MENU_OPTION] = {"Date", "City", "State", "Country", "Shape", "Reported", "Clear"};
    char filterMenuOptions[] = {'d', 't', 's', 'c', 'h', 'r', 'l'};
    char openMenu[][MAX_MENU_OPTION] = {"Continue to file name entry?"};
    char openMenuOptions[] = {'e', 'd'};

    // DECLARE OTHER VARIABLES
    int size;
    int viewingLocation = 0;
    int state = 4; // 0 = exiting; 1 = main menu; 2 = sort menu; 3 = filter menu, 4 = opening data
    char fileName[50] = "../sample.csv";
    sightingNode *headNode = malloc(sizeof(sightingNode));
    sightingNode *viewingNode = headNode;


    while (state) { // TODO: implement all menu actions
        switch (state) {
            case 1:
                menuInput = menu("Main Menu", mainMenu, mainMenuOptions, sizeof(mainMenu) / sizeof(mainMenu[0]), 0);
                switch (menuInput) {
                    case 'v':
                        if (viewingLocation + 10 >= size) {
                            printf("You are already viewing the end of the data. Try 'c' to return to the top\n");
                            break;
                        }
                        lookAhead(&viewingNode, MAX_SEARCH_RESULTS);
                        viewingLocation += 10;
                        printList(viewingNode, MAX_SEARCH_RESULTS);
                        if (viewingLocation + 10 >= size)
                            printf("End of data\n");
                        break;
                    case 'c':
                        viewingNode = headNode;
                        viewingLocation = 0;
                        printList(viewingNode, MAX_SEARCH_RESULTS);
                        break;
                    case 's':
                        if (saveData(headNode))
                            state = 0;
                        break;
                    case 'q':
                        printf("Exiting program...");
                        state = 0;
                        break;
                }
                break;
            case 2:
                menuInput = menu("Sort Menu", sortMenu, sortMenuOptions, sizeof(sortMenu) / sizeof(sortMenu[0]), 0);
                break;
            case 3:
                menuInput = menu("Filter menu", filterMenu, filterMenuOptions,
                                 sizeof(filterMenu) / sizeof(filterMenu[0]), 6);
                break;
            case 4:
                menuInput = menu("Load data set (return to use default)", openMenu, openMenuOptions,
                                 sizeof(openMenu) / sizeof(openMenu[0]), 1);
                if (menuInput == 'e')
                    getFileName(fileName);
                else
                    printf("Using default file name %s\n", fileName);
                size = loadData(fileName, headNode);
                viewingNode = headNode;
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size)
                    printf("End of data\n");
                state = 1;
                break;
            default:
                break;
        }
    }

    freeData(headNode);
    return 0;
}

void readStringToSpacer(FILE *csv, char string[]) {
    char c = ' ';
    int i = 0;
    while (c != ',') {
        fscanf(csv, "%c", &c);
        if (c == ',')
            string[i] = '\0';
        else
            string[i] = c;
        i++;
    }
}

void freeData(sightingNode *head) {
    if (head->next == NULL)
        free(head);
    else
        freeData(head->next);
}

void sortByDate(sightingNode **head, int size) { // TODO Test
    sightingNode **cur, *ptr1, *ptr2;
    int sorted, i, j;
    for (i = 0; i < size; i++) {
        cur = head;
        sorted = 1;
        for (j = 0; j < size - i - 1; j++) {
            ptr1 = *cur;
            ptr2 = ptr1->next;
            if (datecmp(ptr1->dateReported, ptr2->dateReported) > 0) {
                *cur = swapNodes(ptr1, ptr2);
                sorted = 0;
            }
            cur = &(*cur)->next;
        }
        if (sorted)
            break;
    }
}

void printList(sightingNode *head, int maxNodes) {
    sightingNode *node = head;
    int i = 0;
    while (node != NULL && i < maxNodes) {
        printNode(node); // TODO function pointer here for varied printing (like a lambda)
        printf("\n");
        i++;
        node = node->next;
    }
}

void printArray(sightingNode *arr[], int size) {
    int i = 0;
    while (arr[i] != NULL && i < size) {
        printNode(arr[i]); // TODO function pointer here for varied printing (like a lambda)
        printf("\n");
        i++;
    }
}

void printNode(sightingNode *node) {
    printf("%d/%d/%d at %02d:%02d in %s (%s, %s): %s for %d seconds; \"%s...\"",
           node->dateTime.date.month,
           node->dateTime.date.day,
           node->dateTime.date.year,
           node->dateTime.hour,
           node->dateTime.minute,
           node->city,
           node->state,
           node->country,
           node->shape,
           node->duration,
           node->comment
    );
}

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2) {
    sightingNode *tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

int loadData(char fileName[], sightingNode *head) {
    FILE *csv = fopen(fileName, "r");
    sightingNode *node = head;
    int i = 0;

    while (1) {
        i++;
        fscanf(csv, "%d/%d/%4d %2d:%2d,",
               &node->dateTime.date.month,
               &node->dateTime.date.day,
               &node->dateTime.date.year,
               &node->dateTime.hour,
               &node->dateTime.minute
        );
        readStringToSpacer(csv, node->city);
        readStringToSpacer(csv, node->state);
        readStringToSpacer(csv, node->country);
        readStringToSpacer(csv, node->shape);
        fscanf(csv, "%d,", &node->duration);
        readStringToSpacer(csv, node->comment);
        fscanf(csv, "%d/%d/%4d,%lf,%lf\n",
               &node->dateReported.month,
               &node->dateReported.day,
               &node->dateReported.year,
               &node->longitude,
               &node->latitude
        );
        if (fscanf(csv, "%c") == EOF) break;
        fseek(csv, -1, SEEK_CUR);
        node->next = malloc(sizeof(sightingNode));
        node = node->next;
    }

    fclose(csv);
    return i;
}

int contains(char c, char arr[], int len) {
    int i;
    for (i = 0; i < len; i++)
        if (arr[i] == c)
            return 1;
    return 0;
}

int datecmp(date d1, date d2) {
    // -1 == d1 before
    // 1 == d1 after
    // 0 == same date
    if (d1.year < d2.year)
        return -1;
    else if (d1.year > d2.year)
        return 1;
    if (d1.month < d2.month)
        return -1;
    else if (d1.month > d2.month)
        return 1;
    if (d1.day < d2.day)
        return -1;
    else if (d1.day > d2.day)
        return 1;
    return 0;
}

void lookAhead(sightingNode **node, int steps) {
    int i;
    sightingNode *out = *node;
    for (i = 0; i < steps; i++) {
        if (out->next)
            out = out->next;
        else
            return;
    }
    *node = out;
}

void searchByString(sightingNode **results, sightingNode *head, stringPredicate predicate, char string[]) {
    sightingNode *node = head;
    int i = 0;
    int j;
    while (i < MAX_SEARCH_RESULTS && node != NULL) {
        if (predicate(node, string)) {
            results[i] = node;
            i++;
        }
        node = node->next;
    }
    for (j = i; j < MAX_SEARCH_RESULTS; j++)
        results[j] = NULL;
}

void searchByDate(sightingNode **results, sightingNode *head, datePredicate predicate, date d) {
    sightingNode *node = head;
    int i = 0;
    int j;
    while (i < MAX_SEARCH_RESULTS && node != NULL) {
        if (predicate(node, d)) {
            results[i] = node;
            i++;
        }
        node = node->next;
    }
    for (j = i; j < MAX_SEARCH_RESULTS; j++)
        results[j] = NULL;
}

int shapePredicate(sightingNode *node, char *shape) {
    return strcmp(node->shape, shape) == 0;
}

int cityPredicate(sightingNode *node, char *city) {
    return strncmp(city, node->city, strlen(city)) == 0;
}

int statePredicate(sightingNode *node, char *state) {
    return strcmp(node->state, state) == 0;
}

int countryPredicate(sightingNode *node, char *country) {
    return strcmp(node->country, country) == 0;
}

int dateOccurredPredicate(sightingNode *node, date d) {
    return datecmp(node->dateTime.date, d) == 0;
}

int dateReportedPredicate(sightingNode *node, date d) {
    return datecmp(node->dateReported, d) == 0;
}

void addEntry(sightingNode **head, sightingNode *node) {
    node->next = *head;
    *head = node;
}

void removeEntry(sightingNode **node) {
    sightingNode *temp = *node;
    *node = (*node)->next;
    free(temp);
}

int saveData(sightingNode *head) {
    FILE *file;
    char fileName[41]; // String to store the user-entered file name
    char createNewFile; // To get input from the user later
    sightingNode *node = head;

    printf("Saving\n");
    printf("Enter the name of the file you would like to save to (this will overwrite existing files): ");
    scanf("%40s", fileName);
    file = fopen(fileName, "r"); // FIRST try to open the file in read mode to see if it exists
    if (file == NULL) { // If it does not exist, give the user the option to create it
        printf("There is no file called %s. Do you want to create it? (Y/n) ", fileName);
        scanf(" %c", &createNewFile);
        if (createNewFile != 'y' && createNewFile != 'Y') { // If the user DOES NOT enter yes, return to the game
            printf("Save cancelled\n");
            return 0; // Return 0 because the file was not saved
        }
    } else { // Otherwise, make sure to close the file before the next step
        fclose(file);
    }
    file = fopen(fileName, "w"); // NOW open it in write mode. If it did not exist before, it will be created

    while (node != NULL) {
        saveNode(file, node);
        node = node->next;
        if (node != NULL)
            fprintf(file, "\n");
    }

    fclose(file);
    printf("Data saved to %s.\n", fileName);
    return 1;
}

void getFileName(char fileName[]) {
    int incomplete = 1;
    FILE *file;
    char _;
    while (incomplete) {
        printf("Enter file name\n> ");
        scanf("%s", fileName);
        file = fopen(fileName, "r");
        if (file != NULL) {
            incomplete = 0;
            fclose(file);
            scanf("%c", &_);
        } else {
            printf("That file does not exist\n");
        }
    }
}

void saveNode(FILE *file, sightingNode *node) {
    fprintf(file, "%d/%d/%d %02d:%02d,%s,%s,%s,%s,%d,%s,%d/%d/%d,%lf,%lf",
            node->dateTime.date.month,
            node->dateTime.date.day,
            node->dateTime.date.year,
            node->dateTime.hour,
            node->dateTime.minute,
            node->city,
            node->state,
            node->country,
            node->shape,
            node->duration,
            node->comment,
            node->dateReported.month,
            node->dateReported.day,
            node->dateReported.year,
            node->longitude,
            node->latitude
    );
}

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption) {
    int i;
    char out[] = " \0";
    printf(SPACER"%s\n", message);
    for (i = 0; i < numOptions; i++)
        printf("%s (%c)\n", optionsText[i], options[i]);
    printf("> ");

    do {
        if (out[1] != '\0')
            printf("Invalid option. Try again\n> ");
        strcpy(out, "  ");
        scanf("%c", out);
        while (out[0] != '\n' && out[1] != '\n') { scanf("%c", out + 1); }
    } while (!contains(out[0], options, numOptions) && out[0] != '\n');

    if (out[0] == '\n')
        return options[defaultOption];
    return out[0];
}