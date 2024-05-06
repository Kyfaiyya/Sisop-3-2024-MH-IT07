#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Kata -> Angka
int convNumb(char *str) {
    if (strcmp(str, "satu") == 0) return 1;
        else if (strcmp(str, "dua") == 0) return 2;
        else if (strcmp(str, "tiga") == 0) return 3;
        else if (strcmp(str, "empat") == 0) return 4;
        else if (strcmp(str, "lima") == 0) return 5;
        else if (strcmp(str, "enam") == 0) return 6;
        else if (strcmp(str, "tujuh") == 0) return 7;
        else if (strcmp(str, "delapan") == 0) return 8;
        else if (strcmp(str, "sembilan") == 0) return 9;
        else {
             printf("Yang bener abangku!\n");
             exit(EXIT_FAILURE);
        }
}

// Angka -> Kata
void convWords(int number, char *words) {
    const char *word[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    const char *teen[] = {"sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};

    if (number >= 1 && number <= 9) {
        strcpy(words, word[number]);
    } else if (number >= 10 && number <= 19) {
        strcpy(words, teen[number - 10]);
    } else if (number >= 20 && number <= 99) {
        int tens = number / 10;
        int ones = number % 10;
        if (ones == 0) {
            strcpy(words, word[tens]);
        } else {
            sprintf(words, "%s puluh %s", word[tens], word[ones]);
        }
    }
}

int main(int argc, char *argv[]) {
    // Input
    char input1[20], input2[20];
    scanf("%s %s", input1, input2);

    // Pipes
    int fd1[2], fd2[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // Fork
    pid_t pid = fork();
    if (pid < 0) {
        perror("Gagal FORK!");
        exit(EXIT_FAILURE);
    }

    // Parent 
    if (pid > 0) {
        close(fd1[0]);
        close(fd2[1]);

        // Konversi -> Angka
        int num1 = convNumb(input1);
        int num2 = convNumb(input2);

        // Result
        int result;
        if (strcmp(argv[1], "-kali") == 0) {
            result = num1 * num2;
        } else if (strcmp(argv[1], "-tambah") == 0) {
            result = num1 + num2;
        } else if (strcmp(argv[1], "-kurang") == 0) {
            result = num1 - num2;
            if (result < 0) {
                printf("ERROR");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[1], "-bagi") == 0) {
            if (num2 == 0) {
                printf("ERROR\n");
                exit(EXIT_FAILURE);
            }
            result = num1 / num2;
        }


        write(fd1[1], &result, sizeof(result));
        wait(NULL);

	// hasil konversi dari pipe
        char words[50];
        read(fd2[0], words, sizeof(words));
        printf("%s", words);

        close(fd1[1]);
        close(fd2[0]);

        // Membuat file .log
        FILE *logFile = fopen("histori.log", "a");
        if (logFile != NULL) {
            char message[100];
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            convWords(result, words);
             fprintf(logFile, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s %s %s sama dengan %s\n",
                    tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, argv[1],input1, argv[1], input2, words);
          fclose(logFile);
        } else {
            perror("Gagal Membuat file .log!");
        }
    }

    // Child
    else {
        close(fd1[1]);
        close(fd2[0]);

        int result;
        read(fd1[0], &result, sizeof(result));

        // Konversi ->  words
        char words[50];
        convWords(result, words);

        // konversi ->  pipe
        write(fd2[1], words, strlen(words)+1);

        close(fd1[0]);
        close(fd2[1]);

        exit(EXIT_SUCCESS);
    }

    return 0;
}
