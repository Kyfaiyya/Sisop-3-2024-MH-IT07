# Sisop-3-2024-MH-IT07

## Soal 2
Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.

  a. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada        input dari user berupa 2 string. Contoh input: tiga tujuh.
  b. Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah.           Contoh: tiga tujuh menjadi 21. 
  c. Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh:        `21` menjadi “dua puluh satu”.
  d. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah        argumen untuk menjalankan program : 
        - perkalian	: ./kalkulator -kali 
        - penjumlahan	: ./kalkulator -tambah
        - pengurangan	: ./kalkulator -kurang
        - pembagian	: ./kalkulator -bagi
Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.

  e. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan            contoh format : 
        - perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
        - penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
        - pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
        - pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”
  f. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process,            lakukan pembuatan file log berdasarkan data yang dikirim dari child process.
        - Format: [date] [type] [message]
        - Type: KALI, TAMBAH, KURANG, BAGI
        - Ex:
            	1. [10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
	            2. [10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
	            3. [10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.
