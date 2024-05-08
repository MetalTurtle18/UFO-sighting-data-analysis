#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACER "--------------------------------------------\n"
#define WELCOME "Welcome to UFO Sighting Viewer.\nThis program lets you view, sort, filter, and modify a large dataset of UFO sightings.\nData include location, shape, duration, and more.\nOpen the file to contiune.\n"

#define MAX_CITY 70 // 69 characters is the longest city name
#define MAX_SHAPE 10 // 9 characters is the longest shape name
#define MAX_COMMENT 236 // 235 characters is the longest comment
#define MAX_MENU_OPTION 50
#define MAX_SEARCH_RESULTS 10

/**
 * struct to store day, month, and year
 */
typedef struct date { // MM/DD/YYYY format in csv
    int year;
    int month;
    int day;
} date;

/**
 * struct to store a date and the time: hour and minute
 */
typedef struct dateTime {
    date date;
    int hour;
    int minute;
} dateTime;

/**
 * struct to store a node of the linked list
 */
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

// Aliases for function pointers for use in function prototypes
typedef int (*stringPredicate)(sightingNode *, char s[]);

typedef int (*datePredicate)(sightingNode *, date);

typedef int (*compare)(sightingNode *, sightingNode *, int);

/**
 * Prompt the user for input and add an item to the linked list
 * @param head the node to treat as the head and insert a new element before, replacing the head
 */
void addEntry(sightingNode **head);

/**
 * Recursively free memory for all nodes in the list
 * @param head
 */
void freeData(sightingNode *head);

/**
 * Prompt the user for a date input
 * @param output where to save the input
 * @param defaultDate
 */
void getDateInput(date *output, date defaultDate);

/**
 * Prompt the user for a file name
 * @param fileName
 */
void getFileName(char fileName[]);

/**
 * Prompt the user for string input
 * @param output where to save the input
 * @param defaultString
 */
void getStringInput(char output[], char defaultString[]);

/**
 * Take in a double pointer to head node and change it to point steps spaces ahead of that
 * @param node
 * @param steps
 */
void lookAhead(sightingNode **node, int steps);

/**
 * Print an array of pointers to nodes
 * @param arr
 * @param size
 */
void printArray(sightingNode *arr[], int size);

/**
 * Print a linked list
 * @param head
 * @param maxNodes how many nodes to print
 */
void printList(sightingNode *head, int maxNodes);

/**
 * Print a single node
 * @param node
 */
void printNode(sightingNode *node);

/**
 * Read a csv file character by character until encountering a spacer
 * @param csv file to read
 * @param string where to put the read sting
 */
void readStringToSpacer(FILE *csv, char string[]);

/**
 * Save a single node to a file
 * @param file
 * @param node
 */
void saveNode(FILE *file, sightingNode *node);

/**
 * Search a linked list by a given predicate and put the results in the given array
 * @param results output array
 * @param head node to start search
 * @param predicate function to use
 * @param d date to compare to
 */
void searchByDate(sightingNode **results, sightingNode *head, datePredicate predicate, date d);

/**
 * Search a linked list by a given predicate and put the results in the given array
 * @param results output array
 * @param head node to start search
 * @param predicate function to use
 * @param string string to compare to
 */
void searchByString(sightingNode **results, sightingNode *head, stringPredicate predicate, char string[]);

/**
 * Sort a linked list by the given comparison function
 * @param head
 * @param size
 * @param dir 1 for increasing, -1 for decreasing
 * @param function
 */
void sortBy(sightingNode **head, int size, int dir, compare function);

/**
 * Return true if the character is contained within the array
 * @param c character to search for
 * @param arr array to search
 * @param len
 * @return 1 if c in arr, 0 otherwise
 */
int contains(char c, char arr[], int len);

/**
 * Return true if the given array contains NULL elements
 * @param arr
 * @return 1 if arr contains NULL, 0 otherwise
 */
int containsNull(sightingNode *arr[MAX_SEARCH_RESULTS]);

/**
 * Load all data from file
 * @param fileName
 * @param head
 * @return integer size of the linked list after loading
 */
int loadData(char fileName[], sightingNode *head);

/**
 * Prompt the user and remove an entry from the list
 * @param node
 * @return 1 if an entry was removed, 0 otherwise
 */
int removeEntry(sightingNode **node);

/**
 * Save the list to a file
 * @param head
 * @return 1 if the data was saved, 0 otherwise
 */
int saveData(sightingNode *head);

// Comparison and predicate functions
int dateTimeCompare(sightingNode *n1, sightingNode *n2, int dir);

int cityCompare(sightingNode *n1, sightingNode *n2, int dir);

int stateCompare(sightingNode *n1, sightingNode *n2, int dir);

int countryCompare(sightingNode *n1, sightingNode *n2, int dir);

int shapeCompare(sightingNode *n1, sightingNode *n2, int dir);

int durationCompare(sightingNode *n1, sightingNode *n2, int dir);

int dateReportedCompare(sightingNode *n1, sightingNode *n2, int dir);

int cityPredicate(sightingNode *node, char *city);

int statePredicate(sightingNode *node, char *state);

int countryPredicate(sightingNode *node, char *country);

int shapePredicate(sightingNode *node, char *shape);

int dateOccurredPredicate(sightingNode *node, date d);

int dateReportedPredicate(sightingNode *node, date d);

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption);

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2);

int main(void) {
    // DECLARE MENUS
    char menuInput;
    char mainMenu[][MAX_MENU_OPTION] = {"View more (default)", "Sort", "Filter", "Return to top", "Add", "Delete",
                                        "Save",
                                        "Quit"};
    char mainMenuOptions[] = {'v', 'o', 'f', 'c', 'a', 'r', 's', 'q'};
    char sortMenu[][MAX_MENU_OPTION] = {"Date (default)", "City", "State", "Country", "Shape", "Duration",
                                        "Date reported",
                                        "Reverse sorting"};
    char sortMenuOptions[] = {'d', 't', 's', 'c', 'h', 'u', 'p', 'r'};
    char filterMenu[][MAX_MENU_OPTION] = {"Date", "City", "State", "Country", "Shape", "Date Reported",
                                          "Reset (default)"};
    char filterMenuOptions[] = {'d', 't', 's', 'c', 'h', 'p', 'r'};
    char openMenu[][MAX_MENU_OPTION] = {"Continue to file name entry?"};
    char openMenuOptions[] = {'e', 'd'};

    // DECLARE OTHER VARIABLES
    int size;
    int viewingLocation = 0; // What index of the list is the user looking at
    int prevSearchType; // 0 = date; 1 = string
    int sortDir = 1;
    int state = 3; // 0 = exiting; 1 = normal viewing; 2 = filtered viewing, 3 = opening data
    char fileName[50] = "../sample.csv"; // Starts with default directory
    compare prevSort = dateTimeCompare; // Last used sort function
    datePredicate prevDateSearch; // Last used date filter
    stringPredicate prevStringSearch; // Last used string filter
    char prevStringSearchString[50] = "hanover"; // Last used string filter text
    date prevDateSearchDate = {2004, 12, 18}; // Last used date filter date

    sightingNode *headNode = malloc(sizeof(sightingNode));
    sightingNode *viewingNode = headNode;
    sightingNode *searchResults[MAX_SEARCH_RESULTS]; // Search results for filter views

    printf(WELCOME);

    // Opening file
    menuInput = menu("Load data set (press return to use default)", openMenu, openMenuOptions,
                     sizeof(openMenu) / sizeof(openMenu[0]), 1);
    if (menuInput == 'e')
        getFileName(fileName); // Prompt the user for a file name
    else
        printf("Using default file name %s\n", fileName);
    size = loadData(fileName, headNode);
    viewingNode = headNode;
    printList(viewingNode, MAX_SEARCH_RESULTS);
    if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
        printf("End of data\n");
    state = 1;

    while (state) {
        menuInput = menu("Main Menu", mainMenu, mainMenuOptions, sizeof(mainMenu) / sizeof(mainMenu[0]), 0);
        switch (menuInput) {
            case 'v': // View (i.e. show the next ten elements)
                if (state == 2) { // If in filtered view, proceed with array
                    if (!containsNull(searchResults) &&
                        searchResults[9]->next != NULL) { // If there is more data to come
                        if (prevSearchType) { // Depending on the search type, difference predicate signatures are required
                            lookAhead(&searchResults[9], MAX_SEARCH_RESULTS);
                            searchByString(searchResults, searchResults[9], prevStringSearch, prevStringSearchString);
                        } else {
                            lookAhead(&searchResults[9], MAX_SEARCH_RESULTS);
                            searchByDate(searchResults, searchResults[9], prevDateSearch, prevDateSearchDate);
                        }
                    } else { // Inform the user if they are at the end
                        printf("You are already viewing the end of the data. Try 'c' to return to the top\n");
                        break;
                    }
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n"); // If they are viewing the last 10 (or fewer) items, indicate that
                } else { // If in regular view, simply move up 10 spaces in the linked list
                    if (viewingLocation + 10 >=
                        size) { // If they are viewing the last 10 (or fewer) items, indicate that
                        printf("You are already viewing the end of the data. Try 'c' to return to the top\n");
                        break;
                    }
                    lookAhead(&viewingNode, MAX_SEARCH_RESULTS);
                    viewingLocation += 10;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                        printf("End of data\n");
                }
                break;
            case 'o': // Sort option
                menuInput = menu("Sorting Menu", sortMenu, sortMenuOptions,
                                 sizeof(sortMenu) / sizeof(sortMenu[0]), 0);
                switch (menuInput) { // Set the proper comparison function depending on user input
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
                    default: // r = reverse
                        sortDir *= -1;
                }
                // Sort and print
                sortBy(&headNode, size, sortDir, prevSort);
                viewingNode = headNode;
                viewingLocation = 0;
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                    printf("End of data\n");
                state = 1;
                break;
            case 'f': // Filter option
                menuInput = menu("Filter (search) menu", filterMenu, filterMenuOptions,
                                 sizeof(filterMenu) / sizeof(filterMenu[0]), 6);
                switch (menuInput) { // Get proper user input and set predicate functions for each type of filter
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
                    case 'r': // Clear filters--set state back to normal view
                        state = 1;
                        break;
                }
                if (state == 2 && searchResults[0] != NULL) { // If there are results, show them
                    viewingNode = headNode;
                    viewingLocation = 0;
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n");
                } else {
                    if (state == 2) // This shouldn't be printed if they just cleared the filter; they know
                        printf("No results\n");
                    state = 1;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                        printf("End of data\n");
                }
                break;
            case 'c': // Back to top option
                if (state == 2) { // Check for filtered view; then move array by searching from head node
                    if (prevSearchType)
                        searchByString(searchResults, headNode, prevStringSearch, prevStringSearchString);
                    else
                        searchByDate(searchResults, headNode, prevDateSearch, prevDateSearchDate);
                    printArray(searchResults, MAX_SEARCH_RESULTS);
                    if (containsNull(searchResults) || searchResults[9]->next == NULL)
                        printf("End of data\n");
                } else { // In normal view, set viewing tracker to 0 and print again
                    viewingNode = headNode;
                    viewingLocation = 0;
                    printList(viewingNode, MAX_SEARCH_RESULTS);
                    if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                        printf("End of data\n");
                }
                break;
            case 'a': // Add entry option
                state = 1;
                addEntry(&headNode);
                viewingNode = headNode;
                viewingLocation = 0;
                size++;
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                    printf("End of data\n");
                break;
            case 'r': // Remove entry option
                state = 1;
                size -= removeEntry(&viewingNode); // Only decrease size if the item was removed successfully
                printList(viewingNode, MAX_SEARCH_RESULTS);
                if (viewingLocation + 10 >= size) // If they are viewing the last 10 (or fewer) items, indicate that
                    printf("End of data\n");
                break;
            case 's': // Save option
                if (saveData(headNode))
                    state = 0;
                break;
            case 'q': // Quit option
                printf("Exiting program...");
                state = 0;
                break;
        }
    }

    freeData(headNode);
    return 0;
}

void addEntry(sightingNode **head) {
    sightingNode *node = malloc(sizeof(sightingNode));
    char out[500];
    char c;
    int i;

    *out = '\0';
    do {
        c = ' ';
        i = 1;
        if (*out != '\0')
            printf("Invalid entry\n");
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
    } while (sscanf(out, "%d/%d/%d %02d:%02d,%69[^,],%2[^,],%2[^,],%9[^,],%d,%235[^,],%d/%d/%d,%lf,%lf",
                    &node->dateTime.date.month,
                    &node->dateTime.date.day,
                    &node->dateTime.date.year,
                    &node->dateTime.hour,
                    &node->dateTime.minute,
                    node->city,
                    node->state,
                    node->country,
                    node->shape,
                    &node->duration,
                    node->comment,
                    &node->dateReported.month,
                    &node->dateReported.day,
                    &node->dateReported.year,
                    &node->longitude,
                    &node->latitude
    ) != 16);

    node->next = *head;
    *head = node;
}

void freeData(sightingNode *head) {
    if (head->next == NULL)
        free(head); // If this is the last node, free it
    else
        freeData(head->next); // Recursively free the subsequent nodes
}

void getDateInput(date *output, date defaultDate) {
    char out[50];
    char c;
    int i;
    *out = '\0';
    do {
        c = ' ';
        i = 1;
        if (*out != '\0') // Only on the first iteration
            printf("Invalid date\n");
        printf("Enter search date in the form YYYY-MM-DD\n> ");
        scanf("%c", out);
        if (out[0] == '\n') { // If the user returns with nothing
            printf("Using default date");
            *output = defaultDate;
            return;
        }
        while (c != '\n') { // Continue reading character by character up to the newline
            scanf("%c", &c);
            if (c != '\n')
                out[i] = c;
            else
                out[i] = '\0';
            i++;
        }
        printf("\nSTRING: %s\n", out);
        // Try to parse the string. If it cannot be parsed, the data was invalid, so repeat and tell the user that.
    } while (sscanf(out, "%d-%d-%d", &output->year, &output->month, &output->day) != 3);
}

void getFileName(char fileName[]) {
    int incomplete = 1;
    FILE *file;
    char _;
    while (incomplete) {
        printf("Enter file name\n> ");
        scanf("%s", fileName);
        file = fopen(fileName, "r");
        if (file != NULL) { // If the file exists, nothing else to do in this function
            incomplete = 0;
            fclose(file);
            scanf("%c", &_); // Clear the buffer
        } else { // If not, tell the user that
            printf("That file does not exist\n");
        }
    }
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
    while (c != '\n') { // Read character by character until the whole string is read
        scanf("%c", &c);
        if (c != '\n')
            out[i] = c;
        else
            out[i] = '\0';
        i++;
    }
    strcpy(output, out);
}

void lookAhead(sightingNode **node, int steps) {
    int i;
    sightingNode *out = *node;
    for (i = 0; i < steps; i++) {
        if (out->next) // If the next node is not NULL
            out = out->next;
        else // If it is NULL, don't go ahead anymore
            return;
    }
    *node = out; // Replace the node pointer with the new one
}

void printArray(sightingNode *arr[], int size) {
    int i = 0;
    while (arr[i] != NULL && i < size) {
        printf("(%d) ", i);
        printNode(arr[i]);
        printf("\n");
        i++;
    }
}

void printList(sightingNode *head, int maxNodes) {
    sightingNode *node = head;
    int i = 0;
    while (node != NULL && i < maxNodes) {
        printf("(%d) ", i);
        printNode(node);
        printf("\n");
        i++;
        node = node->next;
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

void readStringToSpacer(FILE *csv, char string[]) {
    char c = ' ';
    int i = 0;
    while (c != ',') { // Keep reading character by character until the comma
        fscanf(csv, "%c", &c);
        if (c == ',')
            string[i] = '\0';
        else
            string[i] = c;
        i++;
    }
}

void searchByDate(sightingNode **results, sightingNode *head, datePredicate predicate, date d) {
    sightingNode *node = head;
    int i = 0;
    int j;
    while (i < MAX_SEARCH_RESULTS && node != NULL) { // Until we fill up the array
        if (predicate(node, d)) { // Use the input function to check the current node
            results[i] = node;
            i++; // If it matches, up the count
        }
        node = node->next; // Move onto the next node
    }
    for (j = i; j < MAX_SEARCH_RESULTS; j++)
        results[j] = NULL; // Fill the rest of the array with NULL for consistency
}

void searchByString(sightingNode **results, sightingNode *head, stringPredicate predicate, char string[]) {
    sightingNode *node = head;
    int i = 0;
    int j;
    while (i < MAX_SEARCH_RESULTS && node != NULL) {
        if (predicate(node, string)) { // Same code as searchByDate except predicate takes in a string
            results[i] = node;
            i++;
        }
        node = node->next;
    }
    for (j = i; j < MAX_SEARCH_RESULTS; j++)
        results[j] = NULL;
}

void sortBy(sightingNode **head, int size, int dir, compare function) {
    // Carry out bubble sort
    sightingNode **cur, *ptr1, *ptr2;
    int sorted, i, j;
    for (i = 0; i < size; i++) {
        cur = head;
        sorted = 1;
        for (j = 0; j < size - i - 1; j++) {
            ptr1 = *cur;
            ptr2 = ptr1->next;
            if (function(ptr1, ptr2, dir) > 0) { // Use given function to check ordering
                *cur = swapNodes(ptr1, ptr2); // Swap nodes if needed
                sorted = 0;
            }
            cur = &(*cur)->next; // Set the current double pointer to the address of the next node of the new current
        }
        if (sorted) // This indicates no swaps were made, so the list is sorted
            break;
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

int loadData(char fileName[], sightingNode *head) {
    FILE *csv = fopen(fileName, "r");
    sightingNode *node = head;
    int i = 0;
    char junk;

    while (1) { // Continue reading lines until the loop breaks
        i++;
        // Various methods of reading in data
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
        // Check if we are at the end
        if (fscanf(csv, "%c", &junk) == EOF) break;
        fseek(csv, -1, SEEK_CUR); // If not, move back so we don't lose anything
        node->next = malloc(sizeof(sightingNode)); // Allocate memory for the next node that was just read
        node = node->next; // Move on
    }

    fclose(csv);
    return i;
}

int removeEntry(sightingNode **node) {
    char out[] = " \0\0";
    int index, i;
    sightingNode **rNode = node;
    sightingNode *temp;

    if (*node == NULL) {
        printf("No displayed nodes to remove. Returning...\n");
        return 0;
    }

    // Getting number input from the user
    printf("Enter the on-screen index of the node you want to remove (from the last displayed nodes 0-9) [default 0]\n> ");
    do {
        if (out[1] != '\0')
            printf("Invalid index. Try again\n> ");
        strcpy(out, "  ");
        scanf("%c", out);
        while (out[0] != '\n' && out[1] != '\n') { scanf("%c", out + 1); }
        // Try to parse the number. If it can't tell the user and try again
    } while (sscanf(out, "%d", &index) != 1 && out[0] != '\n');
    if (out[0] == '\n') index = 0;

    // Find the node the user chose so it can be removed
    for (i = 0; i < index; i++) {
        if ((*rNode)->next != NULL || i == 10) {
            rNode = &(*rNode)->next;
        } else { // Or determine that it is not able to be removed
            printf("That node does not exist. Returning...\n");
            return 0;
        }
    }

    // Do the pointer swap to remove the chosen node
    temp = *rNode;
    if (temp->next == NULL)
        *rNode = NULL;
    else
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
        if (createNewFile != 'y' && createNewFile != 'Y') { // If the user DOES NOT enter yes, return to the program
            printf("Save cancelled\n");
            return 0; // Return 0 because the file was not saved
        }
    } else { // Otherwise, make sure to close the file before the next step
        fclose(file);
    }
    file = fopen(fileName, "w"); // NOW open it in write mode. If it did not exist before, it will be created

    // Save each node as long as they exist, and move to the next each time
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

// Comparison and predicate functions
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

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption) {
    int i;
    char out[] = " \0";
    printf(SPACER"%s\n", message);
    // Print out each option in the menu for the user
    for (i = 0; i < numOptions; i++)
        printf("%s (%c)\n", optionsText[i], options[i]);
    printf("> ");

    // Loop to get the proper input
    do {
        if (out[1] != '\0')
            printf("Invalid option. Try again\n> ");
        strcpy(out, "  ");
        scanf("%c", out);
        while (out[0] != '\n' && out[1] != '\n') { scanf("%c", out + 1); }
        // Repeat as long the user input is not within the options given in the options array
    } while (!contains(out[0], options, numOptions) && out[0] != '\n');

    if (out[0] == '\n') // If the user returned with no input, use the default option
        return options[defaultOption];
    return out[0];
}

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2) {
    sightingNode *tmp = ptr2->next;
    ptr2->next = ptr1;
    ptr1->next = tmp;
    return ptr2;
}

/**
* SOURCES:
* Data: https://www.kaggle.com/datasets/NUFORC/ufo-sightings/data?select=scrubbed.csv
* Linked list bubble sort: https://www.prepbytes.com/blog/linked-list/c-program-for-performing-bubble-sort-on-linked-list/
* Higher order functions: https://medium.com/nerd-for-tech/higher-order-functions-in-c-74f6c4b550ee
*/