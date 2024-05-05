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

void loadData(char fileName[], sightingNode *head);

void readStringToSpacer(FILE *csv, char string[]);

void freeData(sightingNode *head);

int in(char c, char arr[], int len);

char menu(char message[], char optionsText[][MAX_MENU_OPTION], char options[], int numOptions, int defaultOption);

int main(void) {
    char menuInput;
    // DECLARE MENUS
    char menu1[][MAX_MENU_OPTION] = {"View", "Sort", "Filter", "Add", "Delete", "Save"};
    char menu1o[] = {'v', 'o', 'f', 'a', 'r', 's'}; // TODO this can be put into a struct

    int size;
    sightingNode *headNode = malloc(sizeof(sightingNode));

    size = loadData("../test.csv", headNode);

    sortByDate(headNode, size); // todo testing



//    menuInput = menu("here is menu", menu1, menu1o, sizeof(menu1) / sizeof(menu1[0]), 0);

    printf("SELECTION: %c\n", menuInput);

    freeData(headNode);
    return 0;
}

void loadData(char fileName[], sightingNode *head) {
    FILE *csv = fopen(fileName, "r");
    sightingNode *node = head;

    while (1) {
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

int in(char c, char arr[], int len) {
    int i;
    for (i = 0; i < len; i++)
        if (arr[i] == c)
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
    } while (!in(out, options, numOptions) && out != '\n');

    if (out == '\n')
        return options[defaultOption];
    else
        scanf("%c", &_);
    return out;
}