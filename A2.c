/** @file music_manager.c
 *  @brief A small program to analyze songs data.
 *  @author Mike Z.
 *  @author Felipe R.
 *  @author Hausi M.
 *  @author Jose O.
 *  @author Victoria L.
 *  @author Dorsa Peikani
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list.c"

#define MAX_LINE_LEN 180
 
/**
 * @brief An struct that encapsulates program arguments such as sorting, display preferences, file names, and numerical parameters like energy and danceability.
 */
typedef struct {
    char* sortBy;
    int display;
    char** files;
    int numFiles;
    float energy;          
    float danceability;    
} Options;

/**
 * Function: parse_arguments
 * -------------------------
 * @brief Parses command-line arguments and populates an Options struct with the parsed values.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *
 * @return Options The Options struct populated with the parsed values.
 */
Options parse_arguments(int argc, char* argv[]) {
    Options options;
    options.sortBy = NULL;
    options.display = 0;
    options.files = NULL;
    options.numFiles = 0;
    options.energy = 0.0;          
    options.danceability = 0.0;      

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--sortBy=", 9) == 0) {
            options.sortBy = strdup(argv[i] + 9);
        } else if (strncmp(argv[i], "--display=", 10) == 0) {
            options.display = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--files=", 8) == 0) {
            char* filesArg = strdup(argv[i] + 8);
            char* token = strtok(filesArg, ",");
            while (token != NULL) {
                options.files = realloc(options.files, (options.numFiles + 1) * sizeof(char*));
                options.files[options.numFiles] = strdup(token);
                options.numFiles++;
                token = strtok(NULL, ",");
            }
            free(filesArg);
        } else if (strncmp(argv[i], "--energy=", 9) == 0) {
            options.energy = atof(argv[i] + 9);
        } else if (strncmp(argv[i], "--danceability=", 15) == 0) {
            options.danceability = atof(argv[i] + 15);
        }
    }

    return options;
}

/**
 * Function: print_next_nodes
 * --------------------------
 * @brief Prints a specified number of next nodes from the linked list and writes the data to a CSV file.
 *
 * @param list A pointer to the head of the linked list.
 * @param display The number of nodes to display and write to the CSV file.
 * @param options The Options struct containing configuration settings.
 */
void print_next_nodes(node_t* list, int display, Options options) {
    node_t* current = list;
    int count = 0;

    FILE* output_file = fopen("output.csv", "w");
    if (output_file == NULL) {
        printf("Failed to open output.csv for writing.\n");
        return;
    }
    fprintf(output_file, "artist,song,year,%s\n", options.sortBy);
    while (current != NULL && count < display) {
        fprintf(output_file, "%s,%s,%d,%g\n", current->artist, current->song, current->year, current->sorting);
        current = current->next;
        count++;
    }
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    fclose(output_file);
}

/**
 * Function: openFileForReading
 * ---------------------------
 * @brief Opens a file for reading and returns a pointer to the file stream.
 *
 * @param filename The name of the file to be opened for reading.
 *
 * @return FILE* A pointer to the `FILE` structure representing the file stream if the file was opened successfully,
 *         or NULL if the file could not be opened.
 *
 */ 
FILE* openFileForReading(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file %s for reading.\n", filename);
    }
    return file;
}

/**
 * Function: parseLine
 * -------------------
 * @brief Parses a line of text from the CSV file to extract data fields.
 *
 * This function takes a line of text (a row from the CSV file) and extracts the artist, song, year, and sorting values
 * based on the field index. The field values are then stored in the respective output parameters.
 *
 * @param line The line of text to be parsed from the CSV file.
 * @param artist A pointer to the character array to store the artist name.
 * @param song A pointer to the character array to store the song name.
 * @param year A pointer to the integer variable to store the year value.
 * @param sorting A pointer to the float variable to store the sorting value.
 * @param options A pointer to the Options struct containing configuration settings.
 *
 */ 
void parseLine(char* line, char* artist, char* song, int* year, float* sorting, const Options* options) {
    char* token = strtok(line, ",");
    int field = 0;

    while (token != NULL) {
        switch (field) {
            case 0:
                strcpy(artist, token);
                break;
            case 1:
                strcpy(song, token);
                break;
            case 4:
                *year = atoi(token);
                break;
            case 5:
                if (strcmp(options->sortBy, "popularity") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 6:
                if (strcmp(options->sortBy, "danceability") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 7:
                if (strcmp(options->sortBy, "energy") == 0) {
                    *sorting = atof(token);
                }
                break;
        }
        field++;
        token = strtok(NULL, ",");
    }
}

/**
 * Function: createNode
 * --------------------
 * @brief Creates a new node for the linked list with the provided data.
 *
 * @param artist The artist name to be associated with the new node.
 * @param song The song name to be associated with the new node.
 * @param year The year value to be associated with the new node.
 * @param sorting The sorting value to be associated with the new node.
 *
 * @return node_t* A pointer to the newly created node.
 *
 */ 
node_t* createNode(const char* artist, const char* song, int year, float sorting) {
    node_t* new_node = emalloc(sizeof(node_t));
    new_node->artist = emalloc(strlen(artist) + 1);
    new_node->song = emalloc(strlen(song) + 1);
    strcpy(new_node->artist, artist);
    strcpy(new_node->song, song);
    new_node->year = year;
    new_node->sorting = sorting;
    new_node->next = NULL;
    return new_node;
}

/**
 * Function: extractDataFromCSV
 * ----------------------------
 * @brief Extracts data from CSV files and populates a linked list.
 *
 * @param options The `Options` struct containing configuration settings for data extraction.
 * @param list A double pointer to the head of the linked list, which will be populated with the extracted data.
 *
 */ 
void extractDataFromCSV(Options options, node_t** list) {
    for (int i = 0; i < options.numFiles; i++) {
        FILE* file = openFileForReading(options.files[i]);
        if (file == NULL) {
            continue;
        }

        char line[MAX_LINE_LEN];
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file) != NULL) {
            char artist[100];
            char song[100];
            int year;
            float sorting;

            parseLine(line, artist, song, &year, &sorting, &options);
            node_t* new_node = createNode(artist, song, year, sorting);
            *list = add_inorder(*list, new_node);
        }

        fclose(file);
    }

    print_next_nodes(*list, options.display, options);
}

/**
 * @brief The main function and entry point of the program.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The list of arguments passed to the program.
 * @return int 0: No errors; 1: Errors produced.
 *
 */
int main(int argc, char* argv[]) {
    node_t* list = NULL;
    Options options = parse_arguments(argc, argv);
    extractDataFromCSV(options, &list);
    
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    exit(0);
}

 
//emalloccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
/** @file music_manager.c
 *  @brief A small program to analyze songs data.
 *  @author Mike Z.
 *  @author Felipe R.
 *  @author Hausi M.
 *  @author Jose O.
 *  @author Victoria L.
 *  @author Dorsa Peikani
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list.c"

#define MAX_LINE_LEN 180
 
/**
 * @brief An struct that encapsulates program arguments such as sorting, display preferences, file names, and numerical parameters like energy and danceability.
 */
typedef struct {
    char* sortBy;
    int display;
    char** files;
    int numFiles;
    float energy;          
    float danceability;    
} Options;

/**
 * Function: parse_arguments
 * -------------------------
 * @brief Parses command-line arguments and populates an Options struct with the parsed values.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *
 * @return Options The Options struct populated with the parsed values.
 */
Options parse_arguments(int argc, char* argv[]) {
    Options options;
    options.sortBy = NULL;
    options.display = 0;
    options.files = NULL;
    options.numFiles = 0;
    options.energy = 0.0;          
    options.danceability = 0.0;      

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--sortBy=", 9) == 0) {
            options.sortBy = strdup(argv[i] + 9);
        } else if (strncmp(argv[i], "--display=", 10) == 0) {
            options.display = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--files=", 8) == 0) {
            char* filesArg = strdup(argv[i] + 8);
            char* token = strtok(filesArg, ",");
            while (token != NULL) {
                options.files = realloc(options.files, (options.numFiles + 1) * sizeof(char*));
                options.files[options.numFiles] = strdup(token);
                options.numFiles++;
                token = strtok(NULL, ",");
            }
            free(filesArg);
        } else if (strncmp(argv[i], "--energy=", 9) == 0) {
            options.energy = atof(argv[i] + 9);
        } else if (strncmp(argv[i], "--danceability=", 15) == 0) {
            options.danceability = atof(argv[i] + 15);
        }
    }

    return options;
}

/**
 * Function: print_next_nodes
 * --------------------------
 * @brief Prints a specified number of next nodes from the linked list and writes the data to a CSV file.
 *
 * @param list A pointer to the head of the linked list.
 * @param display The number of nodes to display and write to the CSV file.
 * @param options The Options struct containing configuration settings.
 */
void print_next_nodes(node_t* list, int display, Options options) {
    node_t* current = list;
    int count = 0;

    FILE* output_file = fopen("output.csv", "w");
    if (output_file == NULL) {
        printf("Failed to open output.csv for writing.\n");
        return;
    }
    fprintf(output_file, "artist,song,year,%s\n", options.sortBy);
    while (current != NULL && count < display) {
        fprintf(output_file, "%s,%s,%d,%g\n", current->artist, current->song, current->year, current->sorting);
        current = current->next;
        count++;
    }
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    fclose(output_file);
}

/**
 * Function: openFileForReading
 * ---------------------------
 * @brief Opens a file for reading and returns a pointer to the file stream.
 *
 * @param filename The name of the file to be opened for reading.
 *
 * @return FILE* A pointer to the `FILE` structure representing the file stream if the file was opened successfully,
 *         or NULL if the file could not be opened.
 *
 */ 
FILE* openFileForReading(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file %s for reading.\n", filename);
    }
    return file;
}

void parseLine(char* line, char** artist, char** song, int* year, float* sorting, const Options* options) {
    char* token = strtok(line, ",");
    int field = 0;

    while (token != NULL) {
        switch (field) {
            case 0:
                *artist = emalloc(strlen(token) + 1); // Allocate memory for artist
                strcpy(*artist, token);
                break;
            case 1:
                *song = emalloc(strlen(token) + 1); // Allocate memory for song
                strcpy(*song, token);
                break;
            case 4:
                *year = atoi(token);
                break;
            case 5:
                if (strcmp(options->sortBy, "popularity") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 6:
                if (strcmp(options->sortBy, "danceability") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 7:
                if (strcmp(options->sortBy, "energy") == 0) {
                    *sorting = atof(token);
                }
                break;
        }
        field++;
        token = strtok(NULL, ",");
    }
}

 


/**
 * Function: createNode
 * --------------------
 * @brief Creates a new node for the linked list with the provided data.
 *
 * @param artist The artist name to be associated with the new node.
 * @param song The song name to be associated with the new node.
 * @param year The year value to be associated with the new node.
 * @param sorting The sorting value to be associated with the new node.
 *
 * @return node_t* A pointer to the newly created node.
 *
 */ 
node_t* createNode(const char* artist, const char* song, int year, float sorting) {
    node_t* new_node = emalloc(sizeof(node_t));
    new_node->artist = emalloc(strlen(artist) + 1);
    new_node->song = emalloc(strlen(song) + 1);
    strcpy(new_node->artist, artist);
    strcpy(new_node->song, song);
    new_node->year = year;
    new_node->sorting = sorting;
    new_node->next = NULL;
    return new_node;
}
void extractDataFromCSV(Options options, node_t** list) {
    for (int i = 0; i < options.numFiles; i++) {
        FILE* file = openFileForReading(options.files[i]);
        if (file == NULL) {
            continue;
        }

        char line[MAX_LINE_LEN];
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file) != NULL) {
            char* artist;  
            char* song;
            int year;
            float sorting;

            parseLine(line, &artist, &song, &year, &sorting, &options);
            node_t* new_node = createNode(artist, song, year, sorting);
            *list = add_inorder(*list, new_node);

            free(artist);  
            free(song);   
        }

        fclose(file);
    }

    print_next_nodes(*list, options.display, options);
}
  

/**
 * @brief The main function and entry point of the program.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The list of arguments passed to the program.
 * @return int 0: No errors; 1: Errors produced.
 *
 */
int main(int argc, char* argv[]) {
    node_t* list = NULL;
    Options options = parse_arguments(argc, argv);
    extractDataFromCSV(options, &list);
    
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    exit(0);
}

 //parsetwooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo(final version)
 /** @file music_manager.c
 *  @brief A small program to analyze songs data.
 *  @author Mike Z.
 *  @author Felipe R.
 *  @author Hausi M.
 *  @author Jose O.
 *  @author Victoria L.
 *  @author Dorsa Peikani
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list.c"

#define MAX_LINE_LEN 180
 
/**
 * @brief An struct that encapsulates program arguments such as sorting, display preferences, file names, and numerical parameters like energy and danceability.
 */
typedef struct {
    char* sortBy;
    int display;
    char** files;
    int numFiles;
    float energy;          
    float danceability;    
} Options;

/**
 * Function: parse_files
 * ---------------------
 * @brief Parses command-line arguments to extract file names.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @param numFiles A pointer to an integer, used to store the count of files found.
 *
 * @return char** An array of strings containing the extracted file names, or NULL if the "--files" option is not found.
 */ 
char** parse_files(int argc, char* argv[], int* numFiles) {
    char** files = NULL;
    *numFiles = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--files=", 8) == 0) {
            char* filesArg = strdup(argv[i] + 8);
            char* token = strtok(filesArg, ",");
            while (token != NULL) {
                files = realloc(files, (*numFiles + 1) * sizeof(char*));
                files[*numFiles] = strdup(token);
                (*numFiles)++;
                token = strtok(NULL, ",");
            }
            free(filesArg);
            return files;
        }
    }

    return NULL;  
}

/**
 * Function: parse_arguments
 * -------------------------
 * @brief Parses command-line arguments and populates an Options struct with the parsed values.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *
 * @return Options The Options struct populated with the parsed values.
 */
Options parse_arguments(int argc, char* argv[]) {
    Options options;
    options.sortBy = NULL;
    options.display = 0;
    options.files = NULL;
    options.numFiles = 0;
    options.energy = 0.0;          
    options.danceability = 0.0;      

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--sortBy=", 9) == 0) {
            options.sortBy = strdup(argv[i] + 9);
        } else if (strncmp(argv[i], "--display=", 10) == 0) {
            options.display = atoi(argv[i] + 10);
        } else if (strncmp(argv[i], "--energy=", 9) == 0) {
            options.energy = atof(argv[i] + 9);
        } else if (strncmp(argv[i], "--danceability=", 15) == 0) {
            options.danceability = atof(argv[i] + 15);
        }
    }
    options.files = parse_files(argc, argv, &options.numFiles);

    return options;
}

/**
 * Function: print_next_nodes
 * --------------------------
 * @brief Prints a specified number of next nodes from the linked list and writes the data to a CSV file.
 *
 * @param list A pointer to the head of the linked list.
 * @param display The number of nodes to display and write to the CSV file.
 * @param options The Options struct containing configuration settings.
 * @return nothing
 */
void print_next_nodes(node_t* list, int display, Options options) {
    node_t* current = list;
    int count = 0;

    FILE* output_file = fopen("output.csv", "w");
    if (output_file == NULL) {
        printf("Failed to open output.csv for writing.\n");
        return;
    }
    fprintf(output_file, "artist,song,year,%s\n", options.sortBy);
    while (current != NULL && count < display) {
        fprintf(output_file, "%s,%s,%d,%g\n", current->artist, current->song, current->year, current->sorting);
        current = current->next;
        count++;
    }
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    fclose(output_file);
}

/**
 * Function: openFileForReading
 * ---------------------------
 * @brief Opens a file for reading and returns a pointer to the file stream.
 *
 * @param filename The name of the file to be opened for reading.
 *
 * @return FILE* A pointer to the `FILE` structure representing the file stream if the file was opened successfully,
 *         or NULL if the file could not be opened.
 *
 */ 
FILE* openFileForReading(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file %s for reading.\n", filename);
    }
    return file;
}

/**
 * Function: parseLine
 * -------------------
 * @brief Parses a line of CSV data and extracts relevant song information.
 *
 * @param line The line of CSV data to be parsed.
 * @param artist A pointer to a pointer to store the dynamically allocated memory for the extracted artist name.
 * @param song A pointer to a pointer to store the dynamically allocated memory for the extracted song name.
 * @param year A pointer to an integer to store the extracted year.
 * @param sorting A pointer to a float to store the extracted sorting value.
 * @param options The Options struct containing configuration settings for the data parsing.
 *
 * @return nothing
 */ 
void parseLine(char* line, char** artist, char** song, int* year, float* sorting, const Options* options) {
    char* token = strtok(line, ",");
    int field = 0;

    while (token != NULL) {
        switch (field) {
            case 0:
                *artist = emalloc(strlen(token) + 1);  
                strcpy(*artist, token);
                break;
            case 1:
                *song = emalloc(strlen(token) + 1);  
                strcpy(*song, token);
                break;
            case 4:
                *year = atoi(token);
                break;
            case 5:
                if (strcmp(options->sortBy, "popularity") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 6:
                if (strcmp(options->sortBy, "danceability") == 0) {
                    *sorting = atof(token);
                }
                break;
            case 7:
                if (strcmp(options->sortBy, "energy") == 0) {
                    *sorting = atof(token);
                }
                break;
        }
        field++;
        token = strtok(NULL, ",");
    }
}

/**
 * Function: createNode
 * --------------------
 * @brief Creates a new node for the linked list with the provided data.
 *
 * @param artist The artist name to be associated with the new node.
 * @param song The song name to be associated with the new node.
 * @param year The year value to be associated with the new node.
 * @param sorting The sorting value to be associated with the new node.
 *
 * @return node_t* A pointer to the newly created node.
 *
 */ 
node_t* createNode(const char* artist, const char* song, int year, float sorting) {
    node_t* new_node = emalloc(sizeof(node_t));
    new_node->artist = emalloc(strlen(artist) + 1);
    new_node->song = emalloc(strlen(song) + 1);
    strcpy(new_node->artist, artist);
    strcpy(new_node->song, song);
    new_node->year = year;
    new_node->sorting = sorting;
    new_node->next = NULL;
    return new_node;
}

/**
 * Function: extractDataFromCSV
 * ---------------------------
 * @brief Extracts data from CSV files and populates a linked list with song information.
 *
 * @param options The Options struct containing configuration settings for the data extraction.
 * @param list A pointer to the head of the linked list, where the extracted data will be stored.
 *
 * @return nothing
 */
void extractDataFromCSV(Options options, node_t** list) {
    for (int i = 0; i < options.numFiles; i++) {
        FILE* file = openFileForReading(options.files[i]);
        if (file == NULL) {
            continue;
        }

        char line[MAX_LINE_LEN];
        fgets(line, sizeof(line), file);

        while (fgets(line, sizeof(line), file) != NULL) {
            char* artist;  
            char* song;
            int year;
            float sorting;

            parseLine(line, &artist, &song, &year, &sorting, &options);
            node_t* new_node = createNode(artist, song, year, sorting);
            *list = add_inorder(*list, new_node);

            free(artist);  
            free(song);    
        }
        fclose(file);
     
        }
    print_next_nodes(*list, options.display, options);
    
}

/**
 * @brief The main function and entry point of the program.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The list of arguments passed to the program.
 * @return int 0: No errors; 1: Errors produced.
 *
 */
int main(int argc, char* argv[]) {
    node_t* list = NULL;
    Options options = parse_arguments(argc, argv);
    extractDataFromCSV(options, &list);
    
    free(options.sortBy);
    for (int i = 0; i < options.numFiles; i++) {
        free(options.files[i]);
    }
    free(options.files);

    exit(0);
}

 

  

 
 