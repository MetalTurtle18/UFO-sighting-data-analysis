#include <stdio.h>
#include <stdlib.h>

#define MAX_CITY 70 // 69 characters is the longest city name
#define MAX_SHAPE 10 // 9 characters is the longest shape name
#define MAX_COMMENT 236 // 235 characters is the longest comment
#define MAX_MENU_OPTION 50

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

void readStringToSpacer(FILE *csv, char string[]);

void freeData(sightingNode *head);

void sortByDate(sightingNode **head, int size);

void printList(sightingNode *head, int maxNodes);

void printNode(sightingNode *node);

sightingNode *swapNodes(sightingNode *ptr1, sightingNode *ptr2);

int loadData(char fileName[], sightingNode *head);

int contains(char c, char arr[], int len);

int datecmp(date d1, date d2);

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption);

int main(void) {
    char menuInput;
    // DECLARE MENUS
    char menu1[][MAX_MENU_OPTION] = {"View", "Sort", "Filter", "Add", "Delete", "Save"};
    char menu1o[] = {'v', 'o', 'f', 'a', 'r', 's'}; // TODO this can be put into a struct

    int size;
    sightingNode *headNode = malloc(sizeof(sightingNode));

    size = loadData("../test.csv", headNode);

    printList(headNode, size);
    sortByDate(&headNode, size); // todo testing
    printList(headNode, size);

//    menuInput = menu("here is menu", menu1, menu1o, sizeof(menu1) / sizeof(menu1[0]), 0);

    printf("SELECTION: %c\n", menuInput);

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

void printNode(sightingNode *node) {
    printf("%s", node->city);
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

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption) {
    int i;
    int first = 1;
    char out, _;
    printf("%s\n", message);
    for (i = 0; i < numOptions; i++)
        printf("%s (%c)\n", optionsText[i], options[i]);
    printf("> ");

    do {
        if (!first) {
            printf("Invalid option. Try again\n> ");
            scanf("%c", &_);
        }
        scanf("%c", &out);
        first = 0;
    } while (!contains(out, options, numOptions) && out != '\n');

    if (out == '\n')
        return options[defaultOption];
    else
        scanf("%c", &_);
    return out;
}