#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"
#include <time.h>
#include <ctype.h>

/*
* Purpose: takes integer year, month, day and converts into one string
* Parameters: char *str_dt - the year, month, day string
*             int year - the integer year
*             int month - the integer month
*             int day - the integer day
* Returns: void - nothing
*/
void make_str_dt(char *str_dt, int year, int month, int day) {
    char str_y[5];
    char str_m[3];
    char str_d[3];
    char temp;

    sprintf(str_y, "%d", year);

    str_dt[0] = str_y[0];
    str_dt[1] = str_y[1];
    str_dt[2] = str_y[2];
    str_dt[3] = str_y[3];

    if (month < 10) {
        sprintf(str_m, "%d", month);
        temp = str_m[0];
        str_m[0] = '0';
        str_m[1] = temp;
    } else {
        sprintf(str_m, "%d", month);
    }

    str_dt[4] = str_m[0];
    str_dt[5] = str_m[1];

    if (day < 10) {
        sprintf(str_d, "%d", day);
        temp = str_d[0];
        str_d[0] = '0';
        str_d[1] = temp;
    } else {
        sprintf(str_d, "%d", day);
    }

    str_dt[6] = str_d[0];
    str_dt[7] = str_d[1];
    str_dt[8] = '\0';

}

/*
* Purpose: takes a date string and converts the time into a 12 hour time
* Parameters: char *formatted_12_hrs_tm - the time in 12 hours time 
*             char *dt_time - the given date string
*             int len - the length of the string
* Returns: void - nothing
*/
void get_12_hrs_tm(char *formatted_12_hrs_tm, char *dt_time, int len) {
struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour,
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_12_hrs_tm, len, "%I:%M %p",
        localtime(&full_time));

    if (formatted_12_hrs_tm[0] == '0') {
        formatted_12_hrs_tm[0] = ' ';
    }

}

/*
* Purpose: takes the given values for date times, summary, location and
* repeating date times and created an event
* Parameters: event_t *event - the created event
*             char *start_time - the starting date string of the event
*             char *end_time - the ending date string of the event
*             char *summ - the summary of the event
*             char *loc - the location of the event
*             char *rrule - the date to repeat to
* Returns: event_t * - the created event
*/
event_t  *create_event(event_t *event, char *start_time, char *end_time,
    char *summ, char *loc, char *rrule)
    {
        event = (event_t *)emalloc(sizeof(event_t));

        strncpy(event->dtstart, start_time, DT_LEN);
        strncpy(event->dtend, end_time, DT_LEN);
        strncpy(event->summary, summ, SUMMARY_LEN);
        strncpy(event->location, loc, LOCATION_LEN);
        strncpy(event->rrule, rrule, RRULE_LEN);

        return event;
    }

/*
* Purpose: formats the given date string into a more readable format
* Parameters: char *formatted_time - the new formatted time
*             const char *dt_time - the given date string
*             const int len - the length of the string
* Returns: void - nothing
*/
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)",
        localtime(&full_time));
}

/*
* Purpose: adds the hours and minutes to the new date string
* Parameters: char *full_time - the complete date string
*             char *new_ft - the incremented date string with only years
*             months and days
* Returns: void - nothing
*/
void add_hrs_mins(char *full_time, char *new_ft) {
    for (int i = 8; i < 15; i++) {
        new_ft[i] = full_time[i];
    }
    new_ft[15] = '\0';
}

/*
* Puropose: increments a given date string by the amount of days desired
* Parameters: char* after - the updated date string
*             char * before - the date string before it is updated
*             int const num_days - the amount of day to increment
* Returns: void - nothing
*/
void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%d", localtime(&full_time));
    strncpy(after + 16, before + 16, 132 - 16);
    after[132 - 1] = '\0';
}

/*
* Purpose: Takes an event that is scheduled to repeat for a given
* amount of time and adds all incremented events to the linked list
* Parameters: node_t *event_list - the linked list of events
*             event_t *first_event - the event to be be added repeatedly
* Returns: node_t * - the uptaded linked list of events
*/
node_t *add_repeating(node_t *event_list, event_t *first_event) {
    char start_time[132];
    char end_time[132];
    event_t *cur_event = NULL;

    strncpy(start_time, first_event->dtstart, 17);
    strncpy(end_time, first_event->dtend, 17);

    while (strncmp(start_time, first_event->rrule, 8) < 0) {
        cur_event = create_event(cur_event, start_time, end_time,
            first_event->summary, first_event->location, 
            first_event->rrule);
        event_list = add_inorder(event_list, new_node(cur_event));

        dt_increment(start_time, start_time, 7);
        add_hrs_mins(first_event->dtstart, start_time);

        dt_increment(end_time, end_time, 7);
        add_hrs_mins(first_event->dtend, end_time);
    }

    return event_list;
}

/*
* Purpose: prints out all of the events within the given time frame
* in a nice readable format
* Parameters: node_t *event_list - the linked list of events
*             char *start_dt - the date to start printing from
*             char *end_dt - the date to stop printing at
* Returns: void - nothing
*/
void print_events(node_t *event_list, char *start_dt, char *end_dt) {

    node_t *cur = event_list;
    node_t *prev = NULL;
    char format_dt[80];

    while (cur != NULL && strcmp(cur->val->dtstart, start_dt) < 0) {
        cur = cur->next;

        if (cur == NULL) {
            exit(0);
        }

    }

    int valid_events = 0;

    while (cur != NULL) {
        if (strncmp(cur->val->dtstart, end_dt, 8) > 0) {
            break;
        }
        char start_12_hrs[9];
        char end_12_hrs[9];

        if (prev != NULL && strncmp(cur->val->dtstart, prev->val->dtstart, 8) == 0) {
            get_12_hrs_tm(start_12_hrs, cur->val->dtstart, 80);
            get_12_hrs_tm(end_12_hrs, cur->val->dtend, 80);

            printf("%s to %s: %s {{%s}}\n", start_12_hrs, end_12_hrs, cur->val->summary, cur->val->location);

            prev = cur;
            cur = cur->next;
            valid_events++;
        } else {
        dt_format(format_dt, cur->val->dtstart, 80);
        int dt_format_len = strlen(format_dt);

        if (valid_events > 0) {
            printf("\n");
        }

        printf("%s\n", format_dt);

        for (int i = 0; i < dt_format_len; i++) {
            printf("-");
        }
        get_12_hrs_tm(start_12_hrs, cur->val->dtstart, 80);
        get_12_hrs_tm(end_12_hrs, cur->val->dtend, 80);

        printf("\n");
        printf("%s to %s: %s {{%s}}\n", start_12_hrs, end_12_hrs, cur->val->summary, cur->val->location);

        prev = cur;
        cur = cur->next;
        valid_events++;
        }
    }
}


/*
* Purpose: Reads a given ics file and creates event_t objects
* which are then added to the linked list in the form node_t
* Parameters: node_t *event_list - the linked list of events
*             char *filename - the given ics file
* Returns: node_t * - the linked list of events
*/
node_t *readfile(node_t *event_list, char *filename) {
    FILE *file = fopen(filename, "r");
    char line[132];
    fgets(line, 132, file);
    event_t *cur_event = NULL;
    char start_time[DT_LEN];
    char end_time[DT_LEN];
    char summ[SUMMARY_LEN];
    char loc[LOCATION_LEN];
    char rrule[RRULE_LEN];
   
    while (!(strncmp(line, "END:VCALENDAR", 13) == 0)) {
        fgets(line, 132, file);
        int line_len = strlen(line);

        if (strncmp(line, "DTSTART:", 8) == 0) {
            for (int i = 0; i < 15; i++) {
                start_time[i] = line[i+8];
            }
            start_time[15] = '\0';
        } else if (strncmp(line, "DTEND:", 6) == 0) {
            for (int i = 0; i < 15; i++) {
                end_time[i] = line[6+i];
            }
            end_time[15] = '\0';
        } else if (strncmp(line, "RRULE:", 6) == 0) {
            int k = 0;
            while (isdigit(line[k]) == 0) {
                k++;
            }
            for (int l = k; l < (k+15); l++) {
                rrule[l-k] = line[l];
            }
             rrule[15] = '\0';
        } else if (strncmp(line, "LOCATION:", 9) == 0){
            for (int i = 0; i < line_len; i++) {
                loc[i] = line[i+9];
            }
            loc[line_len-10] = '\0';
        } else if (strncmp(line, "SUMMARY:", 8) == 0) {
            for (int i = 0; i < line_len; i++) {
                summ[i] = line[i+8];
            }
            summ[line_len-9] = '\0';
        } else if (strncmp(line, "END:VEVENT:", 10) == 0) {
            if (rrule[0] == '\0') {
                cur_event = create_event(cur_event, start_time, end_time, summ, loc, rrule);
                event_list = add_inorder(event_list, new_node(cur_event));
            } else {
                cur_event = create_event(cur_event, start_time, end_time, summ, loc, rrule);
                event_list = add_repeating(event_list, cur_event);
            }
            

            rrule[0] = '\0';
        } else {

        }
    }

    fclose(file);
    return event_list;
}

int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char start_dt[9];
    char end_dt[9];
    char *filename = NULL;
    int i;

    for (i = 0; i < argc; i++) {
    if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr,
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    make_str_dt(start_dt, from_y, from_m, from_d);
    make_str_dt(end_dt, to_y, to_m, to_d);
    
    node_t *event_list = NULL;
    event_list = readfile(event_list, filename);

    print_events(event_list, start_dt, end_dt);

    node_t  *temp = NULL;
    while (event_list != NULL) {
        temp = event_list;
        event_list = event_list->next;
        free(temp->val);
        free(temp);
    }
        exit(0);
}
