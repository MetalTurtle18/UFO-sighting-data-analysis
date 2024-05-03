#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 10
//#define ROWS 80332
#define ROWS 10 // TODO debug number
#define MAX_CITY 70 // 69 characters is the longest city name
#define MAX_SHAPE 10 // 9 characters is the longest shape name
#define MAX_COMMENT 236 // 235 characters is the longest comment
#define MAX_DURATION_CHARS 10

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
    char state[2]; // Note: NOT string
    char country[2]; // Note: NOT string
    char shape[MAX_SHAPE];
    int duration;
    char comment[MAX_COMMENT];
    date dateReported;
    double latitude;
    double longitude;
    struct sightingNode *next;
} sightingNode;

void loadData(char fileName[], sightingNode *head);

void readDate(FILE *csv, date *date);

void readTime(FILE *csv, dateTime *dateTime);

void readStringToSpacer(FILE *csv, char string[]);

void freeData(sightingNode *head);

int main(void) {
    sightingNode *headNode = malloc(sizeof(sightingNode));

    loadData("../scrubbed.csv", headNode);

    fflush(stdout);

//    freeData(headNode);
    free(headNode);
    return 0;
}

void loadData(char fileName[], sightingNode *head) {
    FILE *csv = fopen(fileName, "r");
    int line;
    char c;

    char date[11];
    char time[6];
    char city[MAX_CITY];
    char duration[MAX_DURATION_CHARS];

    for (line = 0; line < ROWS; line++) {
        fscanf(csv, "%s", date);
        fseek(csv, 1, SEEK_CUR);
        fscanf(csv, "%5s", time);
        fseek(csv, 1, SEEK_CUR);
        readStringToSpacer(csv, city);
        fseek(csv, 1, SEEK_CUR);
        fscanf(csv, "%c", &c);
        if (c != ',') {
            head->state[0] = c;
            fscanf(csv, "%c", &head->state[1]);
            fseek(csv, 1, SEEK_CUR);
        }
        fscanf(csv, "%c", &c);
        if (c != ',') {
            head->country[0] = c;
            fscanf(csv, "%c", &head->country[1]);
            fseek(csv, 1, SEEK_CUR);
        }
        readStringToSpacer(csv, head->shape);
        fseek(csv, 1, SEEK_CUR);
        readStringToSpacer(csv, duration);
        fseek(csv, 1, SEEK_CUR);
        // TODO: more reading
    }

    fclose(csv);
}

void readDate(FILE *csv, date *date) {

}

void readTime(FILE *csv, dateTime *dateTime) {

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
    fseek(csv, -1, SEEK_CUR);
}