#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#define FOLDER "new-data/"

void transfer_to_shared_mem(const char *file_name, int mem_key) {
    FILE *file_ptr = fopen(file_name, "r");
    if (file_ptr == NULL) {
        perror("Unable to open file");
        return;
    }
    
    fseek(file_ptr, 0, SEEK_END);
    long file_sz = ftell(file_ptr);
    rewind(file_ptr);

    int shared_mem_id = shmget(mem_key, file_sz, IPC_CREAT | 0666);
    if (shared_mem_id == -1) {
        perror("Shared memory creation failed");
        fclose(file_ptr);
        return;
    }

    char *shared_mem_ptr = (char *)shmat(shared_mem_id, NULL, 0);
    if (shared_mem_ptr == (void *)-1) {
        perror("Shared memory attachment failed");
        fclose(file_ptr);
        return;
    }

    size_t read_bytes = fread(shared_mem_ptr, 1, file_sz, file_ptr);
    if (read_bytes != file_sz) {
        perror("File read error");
        shmdt(shared_mem_ptr);
        fclose(file_ptr);
        return;
    }

    shmdt(shared_mem_ptr);
    fclose(file_ptr);
}

void transfer_filename_to_shared_mem(const char *file_name, int mem_key) {
    int shared_mem_id = shmget(mem_key, strlen(file_name) + 1, IPC_CREAT | 0666);
    if (shared_mem_id == -1) {
        perror("Shared memory creation failed");
        return;
    }

    char *shared_mem_ptr = (char *)shmat(shared_mem_id, NULL, 0);
    if (shared_mem_ptr == (void *)-1) {
        perror("Shared memory attachment failed");
        return;
    }

    strcpy(shared_mem_ptr, file_name);
    shmdt(shared_mem_ptr);
}

int main() {
    char folder_path[256];
    if (getcwd(folder_path, sizeof(folder_path)) == NULL) {
        perror("Unable to get current directory");
        return 1;
    }
    strcat(folder_path, "/");
    strcat(folder_path, FOLDER);

    DIR *folder = opendir(folder_path);
    if (folder == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    struct dirent *dir_entry;
    while ((dir_entry = readdir(folder)) != NULL) {
        if (dir_entry->d_type == DT_REG) {
            if (strstr(dir_entry->d_name, "trashcan.csv") != NULL) {
                char file_path[256];
                snprintf(file_path, sizeof(file_path), "%s%s", folder_path, dir_entry->d_name);
                transfer_filename_to_shared_mem(dir_entry->d_name, 1235);
                transfer_to_shared_mem(file_path, 1236);
            } else if (strstr(dir_entry->d_name, "parkinglot.csv") != NULL) {
                char file_path[256];
                snprintf(file_path, sizeof(file_path), "%s%s", folder_path, dir_entry->d_name);
                transfer_filename_to_shared_mem(dir_entry->d_name, 1233);
                transfer_to_shared_mem(file_path, 1234);
            } else {
                char file_path[256];
                snprintf(file_path, sizeof(file_path), "%s%s", folder_path, dir_entry->d_name);
                remove(file_path);
            }
        }
    }
    closedir(folder);

    return 0;
}
