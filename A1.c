 
/** @file event_manager.c
 *  @brief A pipes & filters program that uses conditionals, loops, and string processing tools in C to process iCalendar
 *  events and printing them in a user-friendly format.
 *  @author Felipe R.
 *  @author Hausi M.
 *  @author Jose O.
 *  @author Victoria L.
 *  @author Dorsa Peikani
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief The maximum line length.
 *
 */
#define MAX_LINE_LEN 132

/**
 * Function: main
 * --------------
 * @brief The main function and entry point of the program.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The list of arguments passed to the program.
 * @return int 0: No errors; 1: Errors produced.
 *
 */
 


char* startDateArg = NULL;
char* endDateArg = NULL;
char* fileNameArg = NULL;
char startDate[11];
char endDate[11];
int new_line = 0;


// Function to parse the date and time from a given string
// takes a string dateTime as input and extracts individual components of the date and time from it
void parseDateTime(const char* dateTime, int* year, int* month, int* day, int* hour, int* minute) {
    sscanf(dateTime, "%4d%2d%2dT%2d%2d", year, month, day, hour, minute);
}



// Function to print a formatted date header for a specific date
//takes three arguments: month, day, and year, representing the components of the date.
void printDateHeader(int month, int day, int year) {
     
    char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

     if (new_line != 0){
        printf("\n");
     }
    
    printf("%s %02d, %d\n", months[month - 1], day, year);
    for (int i = 0; i < strlen(months[month - 1]) + 3 + 2 + 4; i++) {
        printf("-");
    }
    printf("\n");
    new_line = 1; 
}



// Function to print the formatted date and time range along with summary and location
//takes six arguments: startHour, startMinute, endHour, endMinute, summary, and location.
void printDateTimeRange(int startHour, int startMinute, int endHour, int endMinute, const char* summary, const char* location) {
    printf("%2d:%02d %s to %2d:%02d %s: %s {{%s}}\n",
        (startHour % 12 == 0) ? 12 : startHour % 12, startMinute, startHour < 12 ? "AM" : "PM",
        (endHour % 12 == 0) ? 12 : endHour % 12, endMinute, endHour < 12 ? "AM" : "PM",
        summary, location);
}



// is the main function that combines the other functions (parseDateTime, printDateHeader, and printDateTimeRange) to print the formatted representation of a date and time range, along with the provided summary and location.
void printFormattedDateTime(const char* dtstart, const char* dtend, const char* summary, const char* location) {
     
    static int prevDay = -1; // variable to store the previous day
    int startYear, startMonth, startDay, startHour, startMinute;
    int endYear, endMonth, endDay, endHour, endMinute;

    parseDateTime(dtstart, &startYear, &startMonth, &startDay, &startHour, &startMinute);
    parseDateTime(dtend, &endYear, &endMonth, &endDay, &endHour, &endMinute);

    if (startDay != prevDay) {
    
        printDateHeader(startMonth, startDay, startYear);
        prevDay = startDay;
    }

    printDateTimeRange(startHour, startMinute, endHour, endMinute, summary, location);
    
}
  

// Extract the start date, end date, and file name from command-line arguments
void extractArguments(int argc, char* argv[]) {
    
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            startDateArg = argv[i] + 8;
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            endDateArg = argv[i] + 6;
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            fileNameArg = argv[i] + 7;
        }
    }
}


// takes input date strings in the format YYYY/MM/DD, extracts the individual components, and formats them into strings in the format YYYYMMDD
void formatDateToInt() {
      
    int year, month, day;
    sscanf(startDateArg, "%4d/%2d/%2d", &year, &month, &day);
    sprintf(startDate, "%04d%02d%02d", year, month, day);

    sscanf(endDateArg, "%4d/%2d/%2d", &year, &month, &day);
    sprintf(endDate, "%04d%02d%02d", year, month, day);
}


// reads an input file line by line, extracts specific information, and closes it
// arguments: fileNameArg, startDate, endDate 
//process each event separately
void processFile(const char* fileNameArg, const char* startDate, const char* endDate) {
       
    FILE* file = fopen(fileNameArg, "r");
    // Read the file line by line
    char line[256];
    char dtstart[16], dtend[16], summary[100], location[100];
    int eventStarted = 0; // Flag to indicate if an event is being processed

    while (fgets(line, sizeof(line), file) != NULL) {
        if (!eventStarted) {
            // Check if an event is starting
            if (strncmp(line, "BEGIN:VEVENT", 12) == 0) {
                eventStarted = 1;
            }
        } else {
            // Check if an event is ending
            if (strncmp(line, "END:VEVENT", 10) == 0) {
                // Convert dtstart and dtend and startDate and endDate to integers for comparison
                int startInt = atoi(dtstart);
                int endInt = atoi(dtend);
                int integerStartDateArg = atoi(startDate);
                int integerEndDateArg = atoi(endDate);

                if ((integerStartDateArg <= startInt) && (integerEndDateArg >= endInt)) {
                    printFormattedDateTime(dtstart, dtend, summary, location);
                }
            } else {
                // Extract relevant information within the event
                if (strncmp(line, "DTSTART:", 8) == 0) {
                    sscanf(line + 8, "%s", dtstart);
                } else if (strncmp(line, "DTEND:", 6) == 0) {
                    sscanf(line + 6, "%s", dtend);
                } else if (strncmp(line, "SUMMARY:", 8) == 0) {
                    sscanf(line + 8, "%[^\n]", summary);
                } else if (strncmp(line, "LOCATION:", 9) == 0) {
                    sscanf(line + 9, "%[^\n]", location);
                }
            }
        }
    }
    fclose(file);
}

 

// main Function
int main(int argc, char* argv[]) { 
    extractArguments(argc, argv);
    formatDateToInt();
    processFile(fileNameArg, startDate, endDate);
     
    return 0;
}