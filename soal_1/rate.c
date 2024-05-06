#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

char *get_file_name(char *path) {
    char *file_name = strrchr(path, '/');
    if (file_name == NULL)
        return path;
    else
        return file_name + 1;
}

void display_data(key_t key) {
    int segment_id = shmget(key, 0, 0666);
    char *shared_memory = (char *)shmat(segment_id, NULL, 0);

    char *file_name = NULL;
    if (key == 1233) {
        file_name = get_file_name(shared_memory);
        printf("Type: Parking lot\nFile Name: %s\n", file_name);
    } else if (key == 1235) {
        file_name = get_file_name(shared_memory);
        printf("Type: Trash can\nFile Name: %s\n", file_name);
    }

    printf("-------------------------------------\n");

    shmdt(shared_memory);
}

void highest_rating(key_t key) {
    float max_rating = -1;
    char item_name[100] = "";

    int segment_id = shmget(key, 0, 0666);
    char *shared_memory = (char *)shmat(segment_id, NULL, 0);
    char *data = strdup(shared_memory);

    char *line = strtok(data, "\n");

    while (line != NULL) {
        char item[100];
        float rating;
        sscanf(line, "%[^,], %f", item, &rating);

        if (rating > max_rating) {
            max_rating = rating;
            strcpy(item_name, item);
        }

        line = strtok(NULL, "\n");
    }

    free(data);
    shmdt(shared_memory);

    printf("Highest Rating: %.1f - Item: %s\n", max_rating, item_name);
}

int main() {
    display_data(1233);
    highest_rating(1234);
    printf("\n\n");
    display_data(1235);
    highest_rating(1236);

    return 0;
}
