/*
 * housekeeping-tools.c
 *
 * This file created for removing file based on

 * - support yaml config file format
 *      - specify the directories and configurations
 *       * - max age in days (in a directory)
 *          - max number to keep (in a directory)
 *          - remove the oldest file first
 *
 * Creator: Daniel Chu , Aegis
 * Date: 03 Feb 2022
 *
 * It will work in Linux & windows env only
 * No running in background
 * Ask 2 input about yaml file and process to extract information and compare the file
 *
 *
 * We will split program into 2 functions and execute in main()
 * 1. removeFile(maxNumber , maxDate , destDir )
 *      - it will remove the file base on the max number and max date
 * 2. readYaml( destDirForYaml )
 *      - it will read the yaml file from the directory
 *      - Several keys will be read and return
 *          - maxNumber
 *          - maxDate
 *          - destDir
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cyaml/cyaml.h>

#define MAX_INPUT_LENGTH 128
int FIRST_VAR;
char* SECOND_VAR=NULL;


/* Top-level structure for storing a plan */
struct plan {
    const char *name;
    const char *version;
    struct task *tasks;
    uint64_t tasks_count;
};

/* Structure for storing target_dir */
struct task {
    const char *target_dir;
    struct max_age_in_days *max_age_in_days;
    struct max_number_of_files *max_number_of_files;
};



/* Structure for storing max_age_in_days */
struct max_age_in_days {
    uint8_t days;
};

/* Structure for storing max_number_of_files */
struct max_number_of_files {
    uint8_t counts;
};

/* Schema for string pointer values (used in sequences of strings). */
static const cyaml_schema_value_t string_ptr_schema = {
        CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};


/* The duration mapping's field definitions schema is an array.
 *
 * All the field entries will refer to their parent mapping's structure,
 * in this case, `struct max_age_in_days`.
 */
static const cyaml_schema_field_t max_age_in_days_fields_schema[] = {
        /* The first field in the mapping is a string.
         *
         * YAML key: "days".
         * C structure member for this key: "days".
         *
         * Its CYAML type is string pointer, and we have no minimum or maximum
         * string length constraints.
         */
        CYAML_FIELD_STRING_PTR(
                "days", CYAML_FLAG_POINTER,
                struct max_age_in_days, days, 0, CYAML_UNLIMITED),


        /* The field array must be terminated by an entry with a NULL key.
         * Here we use the CYAML_FIELD_END macro for that. */
        CYAML_FIELD_END
};


/* The duration mapping's field definitions schema is an array.
 *
 * All the field entries will refer to their parent mapping's structure,
 * in this case, `struct max_number_of_files`.
 */
static const cyaml_schema_field_t max_number_of_files_fields_schema[] = {
        /* The first field in the mapping is a string.
         *
         * YAML key: "days".
         * C structure member for this key: "days".
         *
         * Its CYAML type is string pointer, and we have no minimum or maximum
         * string length constraints.
         */
        CYAML_FIELD_STRING_PTR(
                "counts", CYAML_FLAG_POINTER,
                struct max_number_of_files, counts, 0, CYAML_UNLIMITED),


        /* The field array must be terminated by an entry with a NULL key.
         * Here we use the CYAML_FIELD_END macro for that. */
        CYAML_FIELD_END
};


/* The task mapping's field definitions schema is an array.
 *
 * All the field entries will refer to their parent mapping's structure,
 * in this case, `struct task`.
 */
static const cyaml_schema_field_t task_fields_schema[] = {
        /* The first field in the mapping is a task name.
         *
         * YAML key: "target_dir".
         * C structure member for this key: "target_dir".
         *
         * Its CYAML type is string pointer, and we have no minimum or maximum
         * string length constraints.
         */
        CYAML_FIELD_STRING_PTR(
                "target_dir", CYAML_FLAG_POINTER,
                struct task, target_dir, 0, CYAML_UNLIMITED),


        /* The next field is the task max_age_in_days.
         *
         * YAML key: "max_age_in_days".
         * C structure member for this key: "max_age_in_days".
         *
         * Its CYAML type is a mapping.
         *
         * Since it's a mapping type, the schema for its mapping's fields must
         * be provided too.  In this case, it's `max_age_in_days_fields_schema`.
         */
        CYAML_FIELD_MAPPING(
                "max_age_in_days", CYAML_FLAG_DEFAULT,
                struct task, max_age_in_days, max_age_in_days_fields_schema),


        /* The next field is the task max_age_in_days.
         *
         * YAML key: "max_number_of_files".
         * C structure member for this key: "max_number_of_files".
         *
         * Its CYAML type is a mapping.
         *
         * Since it's a mapping type, the schema for its mapping's fields must
         * be provided too.  In this case, it's `max_number_of_files_fields_schema`.
         */
        CYAML_FIELD_MAPPING(
                "max_number_of_files", CYAML_FLAG_DEFAULT,
                struct task, max_number_of_files, max_number_of_files_fields_schema),

        /* The field array must be terminated by an entry with a NULL key.
         * Here we use the CYAML_FIELD_END macro for that. */
        CYAML_FIELD_END
};


/* The value for a task is a mapping.
 *
 * Its fields are defined in task_fields_schema.
 */
static const cyaml_schema_value_t task_schema = {
        CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT,
                            struct task, task_fields_schema),
};


/* The plan mapping's field definitions schema is an array.
 *
 * All the field entries will refer to their parent mapping's structure,
 * in this case, `struct plan`.
 */
static const cyaml_schema_field_t plan_fields_schema[] = {
        /* The first field in the mapping is a project name.
         *
         * YAML key: "name".
         * C structure member for this key: "name".
         *
         * Its CYAML type is string pointer, and we have no minimum or maximum
         * string length constraints.
         */
        CYAML_FIELD_STRING_PTR(
                "name", CYAML_FLAG_POINTER,
                struct plan, name, 0, CYAML_UNLIMITED),

        /* The next field is the project tasks.
         *
         * YAML key: "tasks".
         * C structure member for this key: "tasks".
         *
         * Its CYAML type is a sequence.
         *
         * Since it's a sequence type, the value schema for its entries must
         * be provided too.  In this case, it's task_schema.
         *
         * Since it's not a sequence of a fixed-length, we must tell CYAML
         * where the sequence entry count is to be stored.  In this case, it
         * goes in the "tasks_count" C structure member in `struct plan`.
         * Since this is "tasks" with the "_count" postfix, we can use
         * the following macro, which assumes a postfix of "_count" in the
         * struct member name.
         */
        CYAML_FIELD_SEQUENCE(
                "tasks", CYAML_FLAG_POINTER,
                struct plan, tasks,
                &task_schema, 0, CYAML_UNLIMITED),

        /* If the YAML contains a field that our program is not interested in
         * we can ignore it, so the value of the field will not be loaded.
         *
         * Note that unless the OPTIONAL flag is set, the ignored field must
         * be present.
         *
         * Another way of handling this would be to use the config flag
         * to ignore unknown keys.  This config is passed to libcyaml
         * separately from the schema.
         */
        CYAML_FIELD_IGNORE("irrelevant", CYAML_FLAG_OPTIONAL),

        /* The field array must be terminated by an entry with a NULL key.
         * Here we use the CYAML_FIELD_END macro for that. */
        CYAML_FIELD_END
};

/* Top-level schema.  The top level value for the plan is a mapping.
 *
 * Its fields are defined in plan_fields_schema.
 */
static const cyaml_schema_value_t plan_schema = {
        CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER,
                            struct plan, plan_fields_schema),
};

/******************************************************************************
 * Actual code to load and save YAML doc using libcyaml.
 ******************************************************************************/

/* Our CYAML config.
 *
 * If you want to change it between calls, don't make it const.
 *
 * Here we have a very basic config.
 */
static const cyaml_config_t config = {
        .log_fn = cyaml_log,            /* Use the default logging function. */
        .mem_fn = cyaml_mem,            /* Use the default memory allocator. */
        .log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
};




/*==================================================*/
//Task 2. Propose a housekeeping tool: to remove files based on maximum age (days), or maximum number to keep (using C programming language).
//
//support two configuration, maximum age in days, or maximum number of files to keep
//maximum age: remove all files that are older than X days, maximum number of files: remove oldest files if the number of files are more than X files.
//support yaml config file format to specify the directories and configurations
//        GUI is not necessary, cli program with a YAML config file to read
/*==================================================*/


int main() {
    printf("This is housing keeping programme\n");
    printf("Please kindly input based on the instruction below \n");
    printf(" Option: e.g.: 1 or 0 | To read the yaml is 1, not reading yaml is 0 \n");


    // Ask for reading config from yaml
    char buf[MAX_INPUT_LENGTH];
    char SECOND_VAR[MAX_INPUT_LENGTH];
    fgets(buf, MAX_INPUT_LENGTH , stdin);
    printf("input is: %s", buf);

    FIRST_VAR = atoi(buf);
    printf("FIRST VAR: %d \n ", FIRST_VAR);

    printf(" 2nd Option: e.g.: ./config.yaml | The full path with the yaml file name \n");
    fgets(buf, MAX_INPUT_LENGTH , stdin);
    memcpy(SECOND_VAR, buf, MAX_INPUT_LENGTH);
    SECOND_VAR[MAX_INPUT_LENGTH-1]=0;
    printf("SECOND_VAR: %s \n", SECOND_VAR);

    // Read the yaml
    cyaml_err_t err;
    struct plan *plan;
    enum {
        ARG_PROG_NAME,
        ARG_PATH_IN,
        ARG_PATH_OUT,
        ARG__COUNT,
    };

    /* Load input file. */
    err = cyaml_load_file(SECOND_VAR, &config,
                          &plan_schema, (void **) &plan, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "ERROR: %s\n", cyaml_strerror(err));
        return EXIT_FAILURE;
    }

    /* Use the data. */
    printf("Project: %s\n", plan->name);
    for (unsigned i = 0; i < plan->tasks_count; i++) {
        printf("%u. %s\n", i + 1, plan->tasks[i].target_dir);
    }


    // after read the yaml
    // need to check for permission




    return 0;
}



