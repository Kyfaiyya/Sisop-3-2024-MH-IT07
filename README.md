# Sisop-3-2024-MH-IT07

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
