# Penyelesaian Soal Shift Modul 3

- Ega Prabu Pamungkas (05111940000014)
- A. Zidan Abdillah Majid (05111940000070)
- Jundullah Hanif Robbani (05111940000144)

-------------------------------------------
## Nomor 1
### 1a
Pada soal ini diharuskan implementasi server client menggunakan multiconnection, dimana saat ada satu client yang menggunakan server, client lain harus mengantri untuk masuk sampai client yang sudah masuk itu keluar. Permasalahan ini dapat diselesaikan menggunakan `socket` dengan mengimplentasikan `select` sehingga bisa menghandle lebih dari satu socket connection. Selain itu karena dibuat antrian, yang diimplementasikan selanjutnya adalah `Queue` sederhana, dimana yang mau digunakan di-push ke `Queue` dan jika sudah keluar akan di-pop dari `Queue`. Berikut pengimplementasiannya. 
```c
	/*
	** server.c
	*/
	...
	...
	// Initial Server
    Queue svQueue;
    qinit(&svQueue);
    fd_now = -1;
    client_login = false;
    ...
    ...
                    for (i=0;i < MAX_CONNECTIONS;i++) {
                        if (all_connections[i] < 0) {
                        	printf("Someone came in with fd: %d\n", new_fd);
                            all_connections[i] = new_fd;
                            qpush(&svQueue, i);
                            if(fd_now == -1 || q_isEmpty(&svQueue)){
                            	fd_now = qfront(&svQueue);
                            	ret_val = send(all_connections[i], SERVER_FREE, sizeof(SERVER_FREE), 0);                            	
                            }
                            else if(i != fd_now){
                            	ret_val = send(all_connections[i], SERVER_OCCUPIED, sizeof(SERVER_OCCUPIED), 0);
                            }                
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
	...
   	...
    			if ((all_connections[i] > 0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))) {

                    /* read incoming data */
                    memset(buf, '\0', DATA_BUFFER);
                    ret_val =  recv(all_connections[i], buf, DATA_BUFFER, 0);
                    if (ret_val == 0) {
                        printf("Goodbye connection with fd: %d\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */

                        // Change to next connection
                        qpop(&svQueue);
                        if(!q_isEmpty(&svQueue)){
                        	fd_now = qfront(&svQueue);
                        	ret_val = send(all_connections[fd_now], SERVER_FREE, sizeof(SERVER_FREE), 0);
                        }
                        else fd_now = -1;
                        client_login = 0;
                    }
	...
	...
```
```c
	/*
	** client.c
	*/
	...
	...
    // Check if server is occupied or not
    printf("Pending...\n");
    ret_val = recv(fd, message, sizeof(message), 0);
    while(strcmp(message, SERVER_OCCUPIED) == 0){
        ret_val = recv(fd, message, sizeof(message), 0);
    }
    ...
    ...
```
Potongan di atas adalah modifikasi agar `Queueu` dapat diimplementasikan. Pertama pada _server.c_, saat client baru telah terhubung dengan server maka akan di-push ke dalam `Queue` lalu dicek jika `Queue` masih kosong maka yang dilayani sekarang adalah client tersebut. Jika ada client baru yang masuk akan di-push juga namun akan dikirim status kepada client bahwa server sedang digunakan, terlihat juga pada _client.c_ terdapat loop untuk menghentikan jalannya cliet sementara sampai server tersedia lagi.

| ![image](https://user-images.githubusercontent.com/68368240/119251748-5b0c6f80-bbd2-11eb-935d-257e622ebcca.png) | ![image](https://user-images.githubusercontent.com/68368240/119251773-81caa600-bbd2-11eb-80b9-639de6665330.png) |
| --- | --- |
| _client masuk_ | _client pending_ |

Setelah melakukan registrasi, data id dan password yang digunakan akan di simpan di *akun.txt*. Dapat diselesaikan dengan sederhana menggunakan fungsi bawaan dari `C` untuk menghandle file. Saat login hanya perlu mengecek line-per-line apakah data id dan password yang digunakan sudah ada di *akun.txt*

|![image](https://user-images.githubusercontent.com/68368240/119251846-0289a200-bbd3-11eb-9ce4-322db7c201e6.png)|
|---|
|_akun.txt_|

### 1b
Untuk memasukkan data kedalam *files.tsv* dapat menggunakan `\t` antar data yang ingin dimasukkan. Karena path yang akan disimpan adalah path asli menuju file yang nanti akan disimpan di folder `Server/FILES/`, maka pertama dapat menggunakan fungsi `realpath` untuk mengetahui path asli dari folder `Server/FILES/` setelah itu dapat menambahkan nama file yang ingin dimasukkan dengan menggunakan `strcat`.

|![image](https://user-images.githubusercontent.com/68368240/119251865-19c88f80-bbd3-11eb-899c-18e731a5eaf7.png)|
|---|
|_files.tsv_|

### 1c
Folder `FILES` dapat dibuat dengan menggunkan fungsi `mkdir()`, memasukkan filenya dan memasukkan data ke dalam *files.tsv* setelah memanggil command *add* sama dengan cara yang telah dijealskan di nomor *1b*. Setelah menerima data publisher, tahun publikasi, dan filepathnya, juga akan dikirim content file dari client ke dalam server sehingga pada sisi server dapat membuat file yang sama persis.

|![image](https://user-images.githubusercontent.com/68368240/119251894-3a90e500-bbd3-11eb-8f1e-aef896aa51fa.png)|
|---|
|_Folder FILES_|

### 1d
Melakukan download (command *download* diikuti nama file) dapat dilakukan sama dengan saat ingin menambahkan file ke dalam server. Dengan cara mengirim nama file dari client ke server, lalu server dapat mengirim content/isi dari file tersebut ke client jika ada.

|![image](https://user-images.githubusercontent.com/68368240/119251990-b0954c00-bbd3-11eb-8486-0106a7175b1e.png)|
|---|
|_hasil download inifile.txt_|

### 1e
Menghapus file, command *delete* diikuti nama file, dapat dilakukan dengan pertama mengecek apakah ada filenya atau tidak sekaligus mengganti nama filenya menjadi *old-Namafile.ekstensi* menggunakan `rename`. Fungsi `rename` akan mengembalikan nilai 0 jika filenya ada, mengembalikan nilai -1 jika tidak ada. Jika ada, maka baris yang mengandung nama file yang ingin dihapus akan dihapus dari *files.tsv*. Jika tidak ada, akan mengirim pesan error ke sisi client.

|![image](https://user-images.githubusercontent.com/68368240/119252000-cc98ed80-bbd3-11eb-9e00-58813ac38a51.png)|
|---|
|_rename inifile.txt_|

|![image](https://user-images.githubusercontent.com/68368240/119252012-da4e7300-bbd3-11eb-8d69-1374b1da8fce.png)|
|---|
|_hapus inifile.txt pada files.tsv_|


### 1f
Melihat semua isi file dapat dilakukan dengan melihat isi dari *files.tsv* lalu mengambil data yang perlu dan menyesuaikan format yang diberikan.

|![image](https://user-images.githubusercontent.com/68368240/119252018-ed614300-bbd3-11eb-8445-02f176eebe5f.png)|
|---|
|_command see_|

### 1g
Mencari file juga dapat dilakukan dengan melihat isi dari *files.tsv* pada kolom pertama lalu mengambil namafile yang sesuai dengan argumen kedua yang diberikan menggunakan fungsi `strstr`.

|![image](https://user-images.githubusercontent.com/68368240/119252032-0833b780-bbd4-11eb-8cc6-001f0ecafa6b.png)|
|---|
|_find file_|

### 1h
Kegiatan log dapat dilakukan dengan mempassing kegiatannya apakah *Tambah* atau *Hapus* lalu nama file yang dilakukan perubahan atau penambahan, dan terakhir akun yang masuk dapat diambil dengan cara pada saat login di sisi server langsung menyimpan id dan password yang telah login pada waktu itu.

|![image](https://user-images.githubusercontent.com/68368240/119252045-1681d380-bbd4-11eb-8d7d-aa30752d290f.png)|
|---|
|_isi running.log_|

### Kendala
Dalam memahami bagaimana `socket` dengan mengimplementasikan `select` cukup membingungkan dan perlu mencari-cari di internet untuk referensi penggunaanya. Soal ini juga cukup panjang dari segi solusinya sehingga cukup bingung dan capek, but it's quite fun. Bisa jadi dari solusi saya yang belum terlalu teroptimasi. Yang cukup menghambat juga dari sisi eksternal adalah device yang dipakai terkadang menjadi lambat saat membuka VirtualBox, barangkali akhir semester dapat dilakukan servis.

|![image](https://user-images.githubusercontent.com/68368240/119252077-5c3e9c00-bbd4-11eb-913a-170db3d106dc.png)|
|---|
|_Tampak Task Manager_|

## Nomor 2
### 2a
### 2b
### 2c
### kendala
## Nomor 3
