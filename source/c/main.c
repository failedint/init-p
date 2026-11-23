// TODO, add git and multi language support.
// DO NOT TOUCH WORKING PARTS.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define Lsize 128

char project_name[50];
char current_cmd[Lsize];

char *get_input(char str_name[], size_t size) {
    fgets(str_name, size, stdin);
    size_t len = strlen(str_name);
    if (len > 0 && str_name[len - 1] == '\n')
        str_name[len - 1] = '\0';
    return str_name;
}

char *whitespace_elim(char string[]) {
    if (string[0] == '\0') {
        puts("invalid project name, or did not enter a name");
        exit(1);
    }
    for (int i = 0; string[i]; i++) {
        if (string[i] == ' ' || string[i] == '\t')
            string[i] = '_';
    }
    return string;
}

void cmd_writer(char subcmd[], size_t size_of_cmd) {
    char main_cmd[Lsize];
    strcpy(main_cmd, current_cmd);
    snprintf(current_cmd, size_of_cmd, "%s %s", main_cmd, subcmd);
    FILE* startfile = fopen("starterfile.sh", "a");
    if (startfile) {
        fprintf(startfile, "%s\n", current_cmd);
        fclose(startfile);
    }
}

void populate_file_tree() {
    FILE* file = fopen("starterfile.sh", "a");
    if (file) {
        fprintf(file, "cd %s\n", project_name);
        fprintf(file, "mkdir -p source include build credit source/c build/hints\n");
        fprintf(file, "touch source/c/main.c credit/credits.txt build/buildfile.sh\n");
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    char question_answer[256];

    if (argc >= 2) {
        if (strcmp(argv[1], "-h") == 0) {
            printf("-h              Prints this help message.\n");
            printf("--build         Builds your project.\n");
            printf("--run           Builds then runs your project.\n");
            exit(0);
        }
        else if (strcmp(argv[1], "--build") == 0) {
            printf("Building project...\n");
            system("sh build/buildfile.sh");
            exit(0);
        }
        else if (strcmp(argv[1], "--run") == 0) {
            FILE *name = fopen("build/hints/run", "r");
            if (!name) {
                printf("Error: No project is in this directory. Run the tool first.\n");
                exit(1);
            }
            fgets(project_name, sizeof(project_name), name);
            fclose(name);

            project_name[strcspn(project_name, "\n")] = 0;

            printf("Building and running %s...\n", project_name);
            char build_cmd[128];
            snprintf(build_cmd, sizeof(build_cmd), "sh build/buildfile.sh");
            if (system(build_cmd) == 0) {
                char run_cmd[128];
                snprintf(run_cmd, sizeof(run_cmd), "./build/%s", project_name);
                system(run_cmd);
            } else {
                printf("Build unfortunatly failed, try again if this is a repeated error look at your build/buildfile.sh\n");
            }
            exit(0);
        }
    }

    if (argc < 2) {
        puts("What would you like your project name to be:\t");
        get_input(project_name, sizeof(project_name));
        whitespace_elim(project_name);
        strcpy(current_cmd, "mkdir");
    } else {
        project_name[0] = '\0';
        for (int i = 1; i < argc; i++) {
            strncat(project_name, argv[i], sizeof(project_name) - strlen(project_name) - 1);
            if (i < argc - 1) {
                strncat(project_name, " ", sizeof(project_name) - strlen(project_name) - 1);
            }
        }
        whitespace_elim(project_name);

        printf("Detected project name from arguments, is this good? [%s] (y/n):\t", project_name);
        get_input(question_answer, sizeof(question_answer));
        if (question_answer[0] == 'y' || question_answer[0] == 'Y') {
            strcpy(current_cmd, "mkdir");
        } else {
            puts("Retry with the project name of your choice.");
            exit(0);
        }
    }

    cmd_writer(project_name, Lsize);
    populate_file_tree();

    FILE* run_hints = fopen("starterfile.sh", "a");
    if (run_hints) {
        fprintf(run_hints, "echo \"%s\" > build/hints/run\n", project_name);
        fclose(run_hints);
    }

    puts("Would you like a README.md (y/n)?\t");
    get_input(question_answer, sizeof(question_answer));
    if (question_answer[0] == 'y' || question_answer[0] == 'Y') {
        FILE* file = fopen("starterfile.sh", "a");
        if (file) {
            fprintf(file, "touch README.md\n");
            fclose(file);
        }
    }

    puts("Would you like to add a copyright? (y/n):\t");
    get_input(question_answer, sizeof(question_answer));

    if (question_answer[0] == 'y' || question_answer[0] == 'Y') {
        puts("What license would you like to use?");
        puts("1: MIT");
        puts("2: Apache 2.0");
        puts("3: GNU GPLv3");
        puts("4: BSD 2-Clause");
        puts("5: ISC");
        puts("6: custom - \"or import from file\"");
        puts("Enter your choice (1-6) or type a simplefied form of the license name:\t");

        get_input(question_answer, sizeof(question_answer));

        FILE* file = fopen("starterfile.sh", "a");
        if (file != NULL) {
            fprintf(file, "mkdir -p copyright\n");

            if (strcmp(question_answer, "1") == 0 || strcmp(question_answer, "mit") == 0) {
                fprintf(file, "curl -sL https://api.github.com/licenses/mit | jq -r '.body' > copyright/LICENSE.txt\n");
            }
            else if (strcmp(question_answer, "2") == 0 || strcmp(question_answer, "apache-2.0") == 0) {
                fprintf(file, "curl -sL https://api.github.com/licenses/apache-2.0 | jq -r '.body' > copyright/LICENSE.txt\n");
            }
            else if (strcmp(question_answer, "3") == 0 || strcmp(question_answer, "gpl-3.0") == 0) {
                fprintf(file, "curl -sL https://api.github.com/licenses/gpl-3.0 | jq -r '.body' > copyright/LICENSE.txt\n");
            }
            else if (strcmp(question_answer, "4") == 0 || strcmp(question_answer, "bsd-2-clause") == 0) {
                fprintf(file, "curl -sL https://api.github.com/licenses/bsd-2-clause | jq -r '.body' > copyright/LICENSE.txt\n");
            }
            else if (strcmp(question_answer, "5") == 0 || strcmp(question_answer, "isc") == 0) {
                fprintf(file, "curl -sL https://api.github.com/licenses/isc | jq -r '.body' > copyright/LICENSE.txt\n");
            }
            else {
                puts("May you please give me a file to import custom license from (Full file location):\t");
                get_input(question_answer, sizeof(question_answer));
                fprintf(file, "cp \"%s\" copyright/LICENSE.txt\n", question_answer);
            }
            fclose(file);
        } else {
            puts("Error opening starterfile.sh to writ einto\n.");
        }
    }

    printf("Enter your wanted way of compiling, or leave blank for default: default = \"cc source/c/*.c -o build/%s\"\n", project_name);
    get_input(question_answer, sizeof(question_answer));

    FILE* file = fopen("starterfile.sh", "a");
    if (file) {
        if (question_answer[0] == '\0') {
            fprintf(file, "echo \"cc source/c/*.c -o build/%s\" > build/buildfile.sh\n", project_name);
        } else {
            fprintf(file, "echo \"%s\" > build/buildfile.sh\n", question_answer);
        }
        fclose(file);
    }

    system("sh starterfile.sh");
    system("rm starterfile.sh");

    puts("Done! You may go on with your C programming!\n");
    return 0;
}
