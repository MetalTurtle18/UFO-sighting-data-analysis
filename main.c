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

typedef int (*compare)(sightingNode *, sightingNode *, int);

void readStringToSpacer(FILE *csv, char string[]);

void freeData(sightingNode *head);

void printList(sightingNode *head, int maxNodes);

void printArray(sightingNode *arr[], int size);

void printNode(sightingNode *node);

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2);

int loadData(char fileName[], sightingNode *head);

int contains(char c, char arr[], int len);

int containsNull(sightingNode *arr[MAX_SEARCH_RESULTS]);

void lookAhead(sightingNode **node, int steps);

void sortBy(sightingNode **head, int size, int dir, compare function);

int dateTimeCompare(sightingNode *n1, sightingNode *n2, int dir);

int dateReportedCompare(sightingNode *n1, sightingNode *n2, int dir);

int cityCompare(sightingNode *n1, sightingNode *n2, int dir);

int stateCompare(sightingNode *n1, sightingNode *n2, int dir);

int countryCompare(sightingNode *n1, sightingNode *n2, int dir);

int shapeCompare(sightingNode *n1, sightingNode *n2, int dir);

int durationCompare(sightingNode *n1, sightingNode *n2, int dir);

void searchByString(sightingNode **results, sightingNode *head, stringPredicate predicate, char string[]);

int shapePredicate(sightingNode *node, char *shape);

int cityPredicate(sightingNode *node, char *city);

int statePredicate(sightingNode *node, char *state);

int countryPredicate(sightingNode *node, char *country);

void searchByDate(sightingNode **results, sightingNode *head, datePredicate predicate, date d);

int dateOccurredPredicate(sightingNode *node, date d);

int dateReportedPredicate(sightingNode *node, date d);

void addEntry(sightingNode **head);

int removeEntry(sightingNode **node);

void getDateInput(date *output, date defaultDate);

void getStringInput(char output[], char defaultString[]);

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
    char sortMenuOptions[] = {'d', 't', 's', 'c', 'h', 'u', 'p', 'r'};
    char filterMenu[][MAX_MENU_OPTION] = {"Date", "City", "State", "Country", "Shape", "Date Reported", "Reset"};
    char filterMenuOptions[] = {'d', 't', 's', 'c', 'h', 'p', 'r'};
    char openMenu[][MAX_MENU_OPTION] = {"Continue to file name entry?"};
    char openMenuOptions[] = {'e', 'd'};

    // DECLARE OTHER VARIABLES
    int size;
    int viewingLocation = 0;
    int prevSearchType; // 0 = date; 1 = string
    int sortDir = 1;
    int state = 3; // 0 = exiting; 1 = normal viewing; 2 = filtered viewing, 3 = opening data
    char fileName[50] = "../sample.csv";
    compare prevSort = dateTimeCompare;
    datePredicate prevDateSearch;
    stringPredicate prevStringSearch;
    char prevStringSearchString[50] = "hanover";
    date prevDateSearchDate = {2004, 12, 18};
    sightingNode *headNode = malloc(sizeof(sightingNode));
    sightingNode *viewingNode = headNode;
    sightingNode *searchResults[MAX_SEARCH_RESULTS];

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

    while (state) { // TODO: implement all menu actions
        menuInput = menu("Main Menu", mainMenu, mainMenuOptions, sizeof(mainMenu) / sizeof(mainMenu[0]), 0);
        switch (menuInput) {
            case 'v':
                if (state == 2) {
                    if (!containsNull(searchResults) && searchResults[9]->next != NULL) {
                        if (prevSearchType) {
                            lookAhead(&searchResults[9], MAX_SEARCH_RESULTS);
                            searchByString(searchResults, searchResults[9], prevStringSearch, prevStringSearchString);
                        }
                    } else {
                        printf("You are already viewing the end of the data. Try 'c' to return to the top\n");
                        break;
                    }
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n");
                } else {
                    if (viewingLocation + 10 >= size) {
                        printf("You are already viewing the end of the data. Try 'c' to return to the top\n");
                        break;
                    }
                    lookAhead(&viewingNode, MAX_SEARCH_RESULTS);
                    viewingLocation += 10;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size)
                        printf("End of data\n");
                }
                break;
            case 'o':
                menuInput = menu("Sorting Menu", sortMenu, sortMenuOptions,
                                 sizeof(sortMenu) / sizeof(sortMenu[0]), 0);
                switch (menuInput) {
                    case 'd':
                        prevSort = dateTimeCompare;
                        break;
                    case 't':
                        prevSort = cityCompare;
                        break;
                    case 's':
                        prevSort = stateCompare;
                        break;
                    case 'c':
                        prevSort = countryCompare;
                        break;
                    case 'h':
                        prevSort = shapeCompare;
                        break;
                    case 'u':
                        prevSort = durationCompare;
                        break;
                    case 'p':
                        prevSort = dateReportedCompare;
                        break;
                    default: // r
                        sortDir *= -1;
                }
                sortBy(&headNode, size, sortDir, prevSort);
                viewingNode = headNode;
                viewingLocation = 0;
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size)
                    printf("End of data\n");
                state = 1;
                break;
            case 'f':
                menuInput = menu("Filter (search) menu", filterMenu, filterMenuOptions,
                                 sizeof(filterMenu) / sizeof(filterMenu[0]), 6);
                switch (menuInput) {
                    case 'd':
                        getDateInput(&prevDateSearchDate, prevDateSearchDate);
                        searchByDate(searchResults, headNode, dateOccurredPredicate, prevDateSearchDate);
                        prevSearchType = 0;
                        prevDateSearch = dateOccurredPredicate;
                        state = 2;
                        break;
                    case 't':
                        getStringInput(prevStringSearchString, "hanover");
                        searchByString(searchResults, headNode, cityPredicate, prevStringSearchString);
                        prevSearchType = 1;
                        prevStringSearch = cityPredicate;
                        state = 2;
                        break;
                    case 's':
                        getStringInput(prevStringSearchString, "nh");
                        searchByString(searchResults, headNode, statePredicate, prevStringSearchString);
                        prevSearchType = 1;
                        prevStringSearch = statePredicate;
                        state = 2;
                        break;
                    case 'c':
                        getStringInput(prevStringSearchString, "us");
                        searchByString(searchResults, headNode, countryPredicate, prevStringSearchString);
                        prevSearchType = 1;
                        prevStringSearch = countryPredicate;
                        state = 2;
                        break;
                    case 'h':
                        getStringInput(prevStringSearchString, "circle");
                        searchByString(searchResults, headNode, shapePredicate, prevStringSearchString);
                        prevSearchType = 1;
                        prevStringSearch = shapePredicate;
                        state = 2;
                        break;
                    case 'p':
                        getDateInput(&prevDateSearchDate, prevDateSearchDate);
                        searchByDate(searchResults, headNode, dateReportedPredicate, prevDateSearchDate);
                        prevSearchType = 0;
                        prevDateSearch = dateReportedPredicate;
                        state = 2;
                        break;
                    case 'r':
                        state = 1;
                        break;
                }
                if (state == 2 && searchResults[0] != NULL) {
                    viewingNode = headNode;
                    viewingLocation = 0;
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n");
                } else {
                    if (state == 2)
                        printf("No results\n");
                    state = 1;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size)
                        printf("End of data\n");
                }
                break;
            case 'c':
                if (state == 2) {
                    if (prevSearchType)
                        searchByString(searchResults, headNode, prevStringSearch, prevStringSearchString);
                    else
                        searchByDate(searchResults, headNode, prevDateSearch, prevDateSearchDate);
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n");
                } else {
                    viewingNode = headNode;
                    viewingLocation = 0;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size)
                        printf("End of data\n");
                }
                break;
            case 'a':
                state = 1;
                addEntry(&viewingNode);
                size++;
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size)
                    printf("End of data\n");
                break;
            case 'r':
                state = 1;
                size -= removeEntry(&viewingNode);
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size)
                    printf("End of data\n");
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

void sortBy(sightingNode **head, int size, int dir, compare function) {
    sightingNode **cur, *ptr1, *ptr2;
    int sorted, i, j;
    for (i = 0; i < size; i++) {
        cur = head;
        sorted = 1;
        for (j = 0; j < size - i - 1; j++) {
            ptr1 = *cur;
            ptr2 = ptr1->next;
            if (function(ptr1, ptr2, dir) > 0) {
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
        printf("(%d) ", i);
        printNode(node); // TODO function pointer here for varied printing (like a lambda)
        printf("\n");
        i++;
        node = node->next;
    }
}

void printArray(sightingNode *arr[], int size) {
    int i = 0;
    while (arr[i] != NULL && i < size) {
        printf("(%d) ", i);
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
    char junk;

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
        if (fscanf(csv, "%c", &junk) == EOF) break;
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

int containsNull(sightingNode *arr[MAX_SEARCH_RESULTS]) {
    int i;
    for (i = 0; i < MAX_SEARCH_RESULTS; i++)
        if (arr[i] == NULL)
            return 1;
    return 0;
}

int dateTimeCompare(sightingNode *n1, sightingNode *n2, int dir) {
    // -1 == d1 before
    // 1 == d1 after
    // 0 == same date
    if (n1->dateTime.date.year < n2->dateTime.date.year)
        return -1 * dir;
    else if (n1->dateTime.date.year > n2->dateTime.date.year)
        return 1 * dir;
    if (n1->dateTime.date.month < n2->dateTime.date.month)
        return -1 * dir;
    else if (n1->dateTime.date.month > n2->dateTime.date.month)
        return 1 * dir;
    if (n1->dateTime.date.day < n2->dateTime.date.day)
        return -1 * dir;
    else if (n1->dateTime.date.day > n2->dateTime.date.day)
        return 1 * dir;
    if (n1->dateTime.hour < n2->dateTime.hour)
        return -1 * dir;
    else if (n1->dateTime.hour > n2->dateTime.hour)
        return 1 * dir;
    if (n1->dateTime.minute < n2->dateTime.minute)
        return -1 * dir;
    else if (n1->dateTime.minute > n2->dateTime.minute)
        return 1 * dir;
    return 0;
}

int dateReportedCompare(sightingNode *n1, sightingNode *n2, int dir) {
    // -1 == d1 before
    // 1 == d1 after
    // 0 == same date
    if (n1->dateReported.year < n2->dateReported.year)
        return -1 * dir;
    else if (n1->dateReported.year > n2->dateReported.year)
        return 1 * dir;
    if (n1->dateReported.month < n2->dateReported.month)
        return -1 * dir;
    else if (n1->dateReported.month > n2->dateReported.month)
        return 1 * dir;
    if (n1->dateReported.day < n2->dateReported.day)
        return -1 * dir;
    else if (n1->dateReported.day > n2->dateReported.day)
        return 1 * dir;
    return 0;
}

int cityCompare(sightingNode *n1, sightingNode *n2, int dir) {
    return strcmp(n1->city, n2->city) * dir;
}

int stateCompare(sightingNode *n1, sightingNode *n2, int dir) {
    return strcmp(n1->state, n2->state) * dir;
}

int countryCompare(sightingNode *n1, sightingNode *n2, int dir) {
    return strcmp(n1->country, n2->country) * dir;
}

int shapeCompare(sightingNode *n1, sightingNode *n2, int dir) {
    return strcmp(n1->shape, n2->shape) * dir;
}

int durationCompare(sightingNode *n1, sightingNode *n2, int dir) {
    return n1->duration > n2->duration ? dir : -dir;
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
    return node->dateTime.date.year == d.year &&
           node->dateTime.date.month == d.month &&
           node->dateTime.date.day == d.day;
}

int dateReportedPredicate(sightingNode *node, date d) {
    return node->dateReported.year == d.year &&
           node->dateReported.month == d.month &&
           node->dateReported.day == d.day;
}

void addEntry(sightingNode **head) {
    sightingNode *node = malloc(sizeof(sightingNode));
    char out[50];
    char c;
    int i;

    *out = '\0';
    do {
        c = ' ';
        i = 1;
        if (*out != '\0')
            printf("Invalid date\n");
        printf("Enter a new record in the CSV form \"12/18/2004 14:30,Hanover,NH,US,circle,120,I TOTALLY SAW A CRAZY CIRCLE ORB,5/7/2024,-72.294490,43.703514\"\n> ");
        scanf("%c", out);
        c = out[0];
        while (c != '\n') {
            scanf("%c", &c);
            if (c != '\n')
                out[i] = c;
            else
                out[i] = '\0';
            i++;
        }
        printf("\nSTRING: %s\n", out);
    } while (sscanf(out, "%d/%d/%d %02d:%02d,%[^,]69s,%[^,]3s,%[^,]3s,%[^,]9s,%d,%[^,]235s,%d/%d/%d,%lf,%lf",
                    &node->dateTime.date.month,
                    &node->dateTime.date.day,
                    &node->dateTime.date.year,
                    &node->dateTime.hour,
                    &node->dateTime.minute,
                    &node->city,
                    &node->state,
                    &node->country,
                    &node->shape,
                    &node->duration,
                    &node->comment,
                    &node->dateReported.month,
                    &node->dateReported.day,
                    &node->dateReported.year,
                    &node->longitude,
                    &node->latitude
    ) != 16);

    node->next = *head;
    *head = node;
}

int removeEntry(sightingNode **node) {
    char out[] = " \0\0";
    int index, i;
    sightingNode **rNode = node;

    printf("Enter the on-screen index of the node you want to remove (from the last displayed nodes 0-9) [default 0]\n> ");
    do {
        if (out[1] != '\0')
            printf("Invalid index. Try again\n> ");
        strcpy(out, "  ");
        scanf("%c", out);
        while (out[0] != '\n' && out[1] != '\n') { scanf("%c", out + 1); }
    } while (sscanf(out, "%d", &index) != 1 && out[0] != '\n');
    if (out[0] == '\n') index = 0;

    for (i = 0; i < index; i++) {
        if ((*rNode)->next != NULL || i == 10) {
            rNode = &(*rNode)->next;
        } else {
            printf("That node does not exist. Returning...\n");
            return 0;
        }
    }

    sightingNode *temp = *rNode;
    *rNode = (*rNode)->next;
    free(temp);
    printf("Removed node %d\n", index);
    return 1;
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

void getDateInput(date *output, date defaultDate) {
    char out[50];
    char c;
    int i = 1;
    *out = '\0';
    do {
        c = ' ';
        i = 1;
        if (*out != '\0')
            printf("Invalid date\n");
        printf("Enter search date in the form YYYY-MM-DD\n> ");
        scanf("%c", out);
        if (out[0] == '\n') {
            printf("Using default date");
            *output = defaultDate;
            return;
        }
        while (c != '\n') {
            scanf("%c", &c);
            if (c != '\n')
                out[i] = c;
            else
                out[i] = '\0';
            i++;
        }
        printf("\nSTRING: %s\n", out);
    } while (sscanf(out, "%d-%d-%d", &output->year, &output->month, &output->day) != 3);
}

void getStringInput(char output[], char defaultString[]) {
    char out[50];
    char c = ' ';
    int i = 1;
    printf("Enter search term\n> ");
    scanf("%c", out);
    if (out[0] == '\n') {
        printf("Using default term %s\n", defaultString);
        strcpy(output, defaultString);
        return;
    }
    while (c != '\n') {
        scanf("%c", &c);
        if (c != '\n')
            out[i] = c;
        else
            out[i] = '\0';
        i++;
    }
    strcpy(output, out);
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