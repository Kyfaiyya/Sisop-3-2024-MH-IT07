# Sisop-3-2024-MH-IT07

## Soal 1
Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:

a. Dalam auth.c pastikan file yang masuk ke folder new-entry adalah file csv dan berakhiran  trashcan dan parkinglot. Jika bukan, program akan secara langsung akan delete file tersebut. 
Contoh dari nama file yang akan diautentikasi:
- belobog_trashcan.csv
- osaka_parkinglot.csv

b. Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:

``belobog_trashcan.csv``

`name, rating`

`Qliphoth Fort, 9.7`

`Everwinter Hill, 8.7`

`Rivet Town, 6.0`

atau

``osaka_parkinglot``

`name, rating`

`Dotonbori, 8.6`

`Kiseki, 9.7`

`Osaka Castle, 8.5`

c. File csv yang lolos tahap autentikasi akan dikirim ke shared memory.

d. Dalam rate.c, proses akan mengambil data csv dari shared memory dan akan memberikan output Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.

``output``

`Type: Trash Can`

`Filename: belobog_trashcan.csv`

`Name: Qliphoth Fort`

`Rating: 8.9`

atau

`Type: Parking Lot`

`Filename: osaka_parkinglot.csv`

`Name: Kiseki`

`Rating: 9.7`

e. Pada db.c, proses bisa memindahkan file dari new-data ke folder microservices/database, WAJIB MENGGUNAKAN SHARED MEMORY.

f. Log semua file yang masuk ke folder microservices/database ke dalam file db.log dengan contoh format sebagai berikut:

[DD/MM/YY hh:mm:ss] [type] [filename]

ex : `[07/04/2024 08:34:50] [Trash Can] [belobog_trashcan.csv]


Contoh direktori awal:

    ├── auth.c

    ├── microservices

    │   ├── database

    │   │   └── db.log

    │   ├── db.c

    │   └── rate.c

    └── new-data

        ├── belobog_trashcan.csv

        ├── ikn.csv

        └── osaka_parkinglot.csv


Contoh direktori akhir setelah dijalankan auth.c dan db.c:

    ├── auth.c

    ├── microservices

    │   ├── database

    │   │   ├── belobog_trashcan.csv

    │   │   ├── db.log

    │   │   └── osaka_parkinglot.csv

    │   ├── db.c

    │   └── rate.c

    └── new-data

### langkah pengerjaan
- buat directory, c, csv, dan log seperti berikut

        ├── auth.c

        ├── microservices

        │   ├── database

        │   │   └── db.log

        │   ├── db.c

        │   └── rate.c

        └── new-data

            ├── belobog_trashcan.csv

            ├── ikn.csv

            └── osaka_parkinglot.csv

- kemudian isi auth.c seperti berikut

    ```
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
    ```

- kemudian jalankan command berikut ke auth.c

    ``gcc auth.c -o auth``

- jika sudah akan terbentuk directory `auth`
- kemudian jlanakan command `./auth`
- jika sudah file csv dengan judul selain trashcan dan parkinglot akan di hapus
- kemudian edit rate.c dengan isi seperti berikut

    ```
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
    ```

- jika sudah, jalankan command `gcc rate.c -o rate`
- kemudian jalankan file rate dengan comman `./rate`
- rate akan menampilkan rating tertinggi dari file csv

## Soal 2
Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.

a. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh.

b. Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21. 

c. Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.

d. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program :

- perkalian	: ./kalkulator -kali

- penjumlahan	: ./kalkulator -tambah

- pengurangan	: ./kalkulator -kurang

- pembagian	: ./kalkulator -bagi

Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah. 

e. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format :

- perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”

- penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”

- pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”

- pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”

f. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process.

- Format: [date] [type] [message]
- Type: KALI, TAMBAH, KURANG, BAGI
- Ex:

[10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.

[10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.

[10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.

### Langkah Pengerjaan : 
- Buat script `dudududu.c` dengan editor nano menggunakan command `nano dudududu.c`
- Isi script `dudududu.c`
  ```
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
    const char *ten[]  = {"", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (number >= 1 && number <= 9) {
        strcpy(words, word[number]);
    } else if (number >= 10 && number <= 19) {
        strcpy(words, teen[number - 10]);
    } else if (number >= 20 && number <= 99) {
        int tens = number / 10;
        int ones = number % 10;
        if (ones == 0) {
            strcpy(words, ten[tens]);
        } else {
            sprintf(words, "%s %s", ten[tens], word[ones]);
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

- Compile program menggunakan command `gcc -o kalkulator dudududu.c`
  
  -> `dudududu.c` adalah nama file yang akan dieksekusi
  
  -> `kalkulator` adalah nama output dari hasil eksekusi file `dudududu.c`
  
  -> Jadi, file `dudududu.c` dieksekusi dan disimpan sebagai `kalkulator`. Seperti bagaimana yang diminta dari soal

- Run program menggunakan command `./kalkulator <argumen>`

### Penjelasan Script

1. Library yang digunakan
```
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <string.h>
  #include <math.h>
  #include <time.h>
```
2. Deklarasi Fungsi
- Function `convNumb`

Fungsi ini untuk mengkonversi kata (string) menjadi angka
```
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
```
- Function `convWords`

Fungsi ini untuk mengkonversi angka menjadi kata
```
void convWords(int number, char *words) {
    const char *word[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    const char *teen[] = {"sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    const char *ten[]  = {"", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (number >= 1 && number <= 9) {
        strcpy(words, word[number]);
    } else if (number >= 10 && number <= 19) {
        strcpy(words, teen[number - 10]);
    } else if (number >= 20 && number <= 99) {
        int tens = number / 10;
        int ones = number % 10;
        if (ones == 0) {
            strcpy(words, ten[tens]);
        } else {
            sprintf(words, "%s %s", ten[tens], word[ones]);
        }
    }
}
```
- Function `main`

Berisikan :

-> Input : Mewakili 2 angka / atau dua input yang akan diproses

-> Pipe : Komunikasi proses parent-child

-> Fork : Membuat proses child

-> Parent Process

-> Child Proses
```
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
```
### Revisi Soal 2
1. Pada function `main` ->  result
```
int result;
    if (strcmp(argv[1], "-kali") == 0) {
        result = num1 * num2;
        printf("hasil perkalian %s dan %s adalah ", input1, input2);
    } else if (strcmp(argv[1], "-tambah") == 0) {
        result = num1 + num2;
        printf("hasil penjumlahan %s dan %s adalah ", input1, input2);
    } else if (strcmp(argv[1], "-kurang") == 0) {
        result = num1 - num2;
        printf("hasil pengurangan %s dan %s adalah ", input1, input2);

        if (result < 0) {
            printf("ERROR");
            exit(EXIT_FAILURE);
        } else if (result == 0) {
            printf("nol");
        }
    } else if (strcmp(argv[1], "-bagi") == 0) {
        if (num2 == 0) {
            printf("ERROR\n");
            exit(EXIT_FAILURE);
        }
        result = num1 / num2;
        printf("hasil pembagian %s dan %s adalah ", input1, input2);
    }

```
a. Sehingga outputnya sesuai dengan yang diminta dari soal.
- Sebelumnya :
```
└─$ ./kalkulator -kali
lima lima
dua puluh lima
```
- Setelah revisi :
```
└─$ ./kalkulator -kali
lima lima
hasil perkalian lima dan lima adalah dua puluh lima
```
b. Pada operasi pengurangan inputnya adalah angka yang sama, hasilnya `nol`.

Punya saya sudah bisa muncul nol, tapi pada belakang nol masih ada simbol-simbol yang tidak bisa dibaca.

Output :
```
└─$ ./kalkulator -kurang
lima lima
hasil pengurangan lima dan lima adalah nol�;���
```
2. Pada function `main` -> .log
```
FILE *logFile = fopen("histori.log", "a");
    if (logFile != NULL) {
        char message[100];
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        convWords(result, words);

        char op[20];
        if (strcmp(argv[1], "-kali") == 0) {
            strcpy(op, "KALI");
        } else if (strcmp(argv[1], "-tambah") == 0) {
            strcpy(op, "TAMBAH");
        } else if (strcmp(argv[1], "-kurang") == 0) {
            strcpy(op, "KURANG");
        } else if (strcmp(argv[1], "-bagi") == 0) {
            strcpy(op, "BAGI");
        }
        fprintf(logFile, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s %s %s sama dengan %s.\n",
                tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, op, input1, argv[1], input2, words);

        fclose(logFile);
    } else {
        perror("Gagal Membuat file .log!");
    }
```
Output : 
```
[09/05/2024 12:55:39] [KURANG] enam -kurang enam sama dengan x����^?.
[11/05/2024 01:54:18] [KURANG] lima -kurang lima sama dengan �D�e�^?.
[11/05/2024 01:54:46] [KALI] lima -kali enam sama dengan tiga puluh.
[11/05/2024 05:06:53] [KALI] lima -kali lima sama dengan dua puluh lima.
[11/05/2024 05:07:08] [KURANG] lima -kurang lima sama dengan �;���^?.
```
## soal 3
-

## Soal 4
Lewis Hamilton 🏎 seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat file (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya. 

a. Client dan server terhubung melalui socket. 

b. client.c di dalam folder client dan server.c di dalam folder server

c. Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server.

d. Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja. 

e. Server digunakan untuk membaca myanimelist.csv. Dimana terjadi pengiriman data antara client ke server dan server ke client.

 	- Menampilkan seluruh judul
	
 	- Menampilkan judul berdasarkan genre
	
 	- Menampilkan judul berdasarkan hari
	
 	- Menampilkan status berdasarkan berdasarkan judul
	
 	- Menambahkan anime ke dalam file myanimelist.csv
	
 	- Melakukan edit anime berdasarkan judul
	
 	- Melakukan delete berdasarkan judul
	
 	- Selain command yang diberikan akan menampilkan tulisan “Invalid Command”
  
f. Karena Lewis juga ingin track anime yang ditambah, diubah, dan dihapus. Maka dia membuat server dapat mencatat anime yang dihapus dalam sebuah log yang diberi nama change.log.
	
 	Format: [date] [type] [massage]
	Type: ADD, EDIT, DEL
	Ex:
		[29/03/24] [ADD] Kanokari ditambahkan.
		[29/03/24] [EDIT] Kamis,Comedy,Kanokari,completed diubah menjadi Jumat,Action,Naruto,completed.
		[29/03/24] [DEL] Naruto berhasil dihapus.
  
g. Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client.

### Langkah Pengerjaan : 
- Buat file client dan isi dengan client.c, di dalam client.c berisi :

Struktur Fungsi Main :
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 5000

int main() {
    int client;
    struct sockaddr_in server;
    char buffer[2048];
```

Membuat socket client :

```
  // Membuat socket client
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        printf("Error creating socket\n");
        return -1;
    }
  ```
Informasi Server :

1. `serverAddr.sin_family = AF_INET;`                       Mengatur tipe alamat yang digunakan (IPv4).
                                    
2. `serverAddr.sin_port = htons(PORT);`                     Menetapkan port server yang akan dituju.
                                           
3. `serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");`   Menetapkan alamat IP server yang akan dituju (localhost).

```
  // Mengisi informasi server
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
```
Mencoba melakukan koneksi ke server. Jika gagal, mencetak pesan kesalahan dan keluar :
```
   
  // Membuat koneksi ke server
    if (connect(client, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Error connecting to server\n");
        return -1;
    }
```

Memulai loop tak terbatas untuk berinteraksi dengan server :

```
    // Loop untuk terus menerima dan mengirim pesan antara klien dan server
    while (1) {
        printf("You: ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(client, buffer, strlen(buffer), 0);
      
        // Memeriksa apakah pengguna ingin keluar dari aplikasi
        if (strcmp(buffer, "exit") == 0) {
           exit(0);
        }
      
        // Mengosongkan buffer sebelum menerima respons dari server
        memset(buffer, 0, sizeof(buffer)); 
        // Menerima respons dari server
        int getbytes = recv(client, buffer, sizeof(buffer), 0); 
        if (getbytes <= 0) {
            printf("Connection closed by server\n");
            break;
        }
        printf("Server:\n%s\n", buffer);
    }
    printf("Exiting the client\n");
    close(client);
    return 0;
}
```
Buat file server dan isi dengan server.c, di dalam server.c berisi :
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>

#define PORT 5000
#define LIST "/home/kyfaiyya/modul3/soal4/myanimelist.csv"
#define LOG "/home/kyfaiyya/modul3/soal4/change.log"
#define TEMP "/home/kyfaiyya/modul3/soal4/temp.csv"
```
Struktur untuk menyimpan informasi file dan log :
```
typedef struct {
    FILE *file;
    FILE *logFile;
} FileData;
```
Struktur untuk menyimpan informasi koneksi server :
```
typedef struct {
    int serverSocket;
    int socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addr_size;
    int addrlen;
} ServerData;

void RequestClient(char *buffer, char *response, FileData *fileData);
void startServer(ServerData *serverData);
```
Struktur Main :
```
int main() {
    ServerData serverData;

    // Start the server
    startServer(&serverData);

    return 0;
}
```
Fungsi RequestClient : 

Fungsi ini digunakan untuk memproses permintaan dari klien.

Fungsi ini membaca perintah dari buffer yang diterima dari klien, kemudian menjalankan tindakan yang sesuai berdasarkan perintah tersebut.

Perintah yang didukung meliputi:

't' untuk mendapatkan daftar anime.

'h' untuk mencari anime berdasarkan hari tayang.

'g' untuk mencari anime berdasarkan genre.

's' untuk mencari anime berdasarkan status.

'a' untuk menambahkan anime ke daftar.

'e' untuk mengedit anime dalam daftar.

'd' untuk menghapus anime dari daftar.

Setiap perintah akan mempengaruhi file daftar anime (myanimelist.csv) dan file log (change.log).
```
void RequestClient(char *buffer, char *response, FileData *fileData) {
    // Implementation of RequestClient function
    fileData->file = fopen(LIST, "r");
    fileData->logFile = fopen(LOG, "a");
    
    char line[2048];
    char command[2048];
    char parameter[2048];
    int found = 0;

    sscanf(buffer, "%s %[^\n]", command, parameter);

    switch (command[0]) {
        case 't':
            {
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        token = strtok(NULL, ",");
                        char numberedLine[2048];
                        sprintf(numberedLine, "%d. %s", count, token);
                        strcat(response, numberedLine);
                        strcat(response, "\n");
                        count++;
                    }
                }
            }
            break;
        case 'h':
            {
                rewind(fileData->file);
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, ",");
                            token = strtok(NULL, ",");
                            char numberedLine[2048];
                            sprintf(numberedLine, "%d. %s", count, token);
                            strcat(response, numberedLine);
                            strcat(response, "\n");
                            found = 1;
                            count++;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime pada hari tersebut\n");
                }
            }
            break;
        case 'g':
            {
                rewind(fileData->file);
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, ",");
                            char numberedLine[2048];
                            sprintf(numberedLine, "%d. %s", count, token);
                            strcat(response, numberedLine);
                            strcat(response, "\n");
                            found = 1;
                            count++;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime dengan genre tersebut\n");
                }
            }
            break;
        case 's':
            {
                rewind(fileData->file);
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        token = strtok(NULL, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, "\n");
                            strcat(response, token);
                            strcat(response, "\n");
                            found = 1;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime dengan status tersebut\n");
                }
            }
            break;
        case 'a':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    fputs(line, temp);
                }
                fprintf(temp, "\n%s", parameter);
                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");

                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                char dateStr[20];
                strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                char *title = strtok(parameter, ",");
                title = strtok(NULL, ",");
                title = strtok(NULL, ",");

                fprintf(fileData->logFile, "[%s] [ADD] %s ditambahkan.\n", dateStr, title);
                strcat(response, "anime berhasil ditambahkan\n");
            }
            break;
        case 'e':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");

                char *pastTitle = strtok(parameter, ",");
                char *postLine = strtok(NULL, "\n");

                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    char *pastLine = strdup(line);
                    char *token = strtok(line, ",");
                    token = strtok(NULL, ",");
                    token = strtok(NULL, ",");
                    char *title = token;

                    if (strcmp(title, pastTitle) != 0) {
                        fputs(pastLine, temp);
                    } else {
                        fprintf(temp, "%s\n", postLine);

                        time_t now = time(NULL);
                        struct tm *t = localtime(&now);
                        char dateStr[20];
                        strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                        fprintf(fileData->logFile, "[%s] [EDIT] %s diubah menjadi %s.\n", dateStr, pastLine, postLine);
                    }

                    free(pastLine);
                }

                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");
                strcat(response, "anime berhasil diedit\n");
            }
            break;
        case 'd':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");
                char *title = parameter;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    char *pastLine = strdup(line);
                    char *titleLine = strtok(line, ",");
                    titleLine = strtok(NULL, ",");
                    titleLine = strtok(NULL, ",");

                    if (strcmp(titleLine, title) != 0) {
                        fputs(pastLine, temp);
                    } else {
                        time_t now = time(NULL);
                        struct tm *t = localtime(&now);
                        char dateStr[20];
                        strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                        fprintf(fileData->logFile, "[%s] [DEL] %s berhasil dihapus.\n", dateStr, title);
                    }
                }
                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");
                strcat(response, "anime berhasil dihapus\n");
            }
            break;
        default:
            strcat(response, "Invalid Command\n");
    }

    fclose(fileData->file);
    fclose(fileData->logFile);
}
```
Fungsi startServer : 

Fungsi ini bertanggung jawab untuk memulai server.

Pertama, server membuat soket menggunakan socket() dengan protokol AF_INET dan tipe soket SOCK_STREAM.

Kemudian, server mengikat soketnya ke alamat yang ditentukan oleh PORT menggunakan bind().

Setelah itu, server mulai mendengarkan koneksi masuk menggunakan listen().

Selanjutnya, server memasuki loop utama di mana ia menerima koneksi baru dari klien menggunakan accept().

Setiap koneksi baru akan ditangani dalam loop terpisah, di mana server akan menerima pesan dari klien, memprosesnya menggunakan RequestClient, dan mengirim respons kembali ke klien 
menggunakan send().

Proses ini terus berlanjut hingga klien mengirim pesan "exit".

Setelah klien keluar, server menutup soketnya dan keluar.
```
void startServer(ServerData *serverData) {
    serverData->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverData->serverSocket < 0) {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    serverData->serverAddr.sin_family = AF_INET;
    serverData->serverAddr.sin_port = htons(PORT);
    serverData->serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverData->serverSocket, (struct sockaddr *)&(serverData->serverAddr), sizeof(serverData->serverAddr)) < 0) {
        printf("Error binding socket\n");
        exit(EXIT_FAILURE);
    }

    if (listen(serverData->serverSocket, 1) < 0) {
        printf("Error listening\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        serverData->addr_size = sizeof(serverData->clientAddr);
        serverData->socket = accept(serverData->serverSocket, (struct sockaddr *)&(serverData->clientAddr), &(serverData->addr_size));
        if (serverData->socket < 0) {
            printf("Error accepting connection\n");
            continue;
        }

        FileData fileData;
        fileData.file = fopen(LIST, "r");
        fileData.logFile = fopen(LOG, "a");

        while (1) {
            char buffer[2048];
            char response[4096];

            memset(buffer, 0, sizeof(buffer));
            recv(serverData->socket, buffer, sizeof(buffer), 0);

            printf("received: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
                printf("Exiting the client\n");
                exit(0);
            }
            memset(response, 0, sizeof(response));
            RequestClient(buffer, response, &fileData);
            send(serverData->socket, response, strlen(response), 0);
        }
        fclose(fileData.file);
        fclose(fileData.logFile);
        close(serverData->socket);
    }
    close(serverData->serverSocket);
}
```

Setelah itu jalankan server :

Langkah - Langkah :

1. ubah server.c menjadi server dengan `gcc server.c -o server`

2. lakukan hal yang sama pada client.c `gcc client.c -o client`

3. kemudian jalankan server terlebih dahulu dengan `./server`

4. kemudian jalankan client dengan `./client`

5. kemudian lakukan sesuai dengan yang diperintahkan di soal

6. buka change.log untuk melihat apa saja perubahan yang sudah dilakukan


## Selesai