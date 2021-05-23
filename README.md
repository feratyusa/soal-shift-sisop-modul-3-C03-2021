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

| (client1) | (client2) |
| --- | --- |
| _client masuk_ | _client pending_ |

Setelah melakukan registrasi, data id dan password yang digunakan akan di simpan di *akun.txt*. Dapat diselesaikan dengan sederhana menggunakan fungsi bawaan dari `C` untuk menghandle file. Saat login hanya perlu mengecek line-per-line apakah data id dan password yang digunakan sudah ada di *akun.txt*

|(akun.txt)|
|---|
|_akun.txt_|

### 1b
Untuk memasukkan data kedalam *files.tsv* dapat menggunakan `\t` antar data yang ingin dimasukkan. Karena path yang akan disimpan adalah path asli menuju file yang nanti akan disimpan di folder `Server/FILES/`, maka pertama dapat menggunakan fungsi `realpath` untuk mengetahui path asli dari folder `Server/FILES/` setelah itu dapat menambahkan nama file yang ingin dimasukkan dengan menggunakan `strcat`.

|(files.tsv)|
|---|
|_files.tsv_|

### 1c
Folder `FILES` dapat dibuat dengan menggunkan fungsi `mkdir()`, memasukkan filenya dan memasukkan data ke dalam *files.tsv* setelah memanggil command *add* sama dengan cara yang telah dijealskan di nomor *1b*. Setelah menerima data publisher, tahun publikasi, dan filepathnya, juga akan dikirim content file dari client ke dalam server sehingga pada sisi server dapat membuat file yang sama persis.

|(FILES)|
|---|
|_Folder FILES_|

### 1d
Melakukan download (command *download* diikuti nama file) dapat dilakukan sama dengan saat ingin menambahkan file ke dalam server. Dengan cara mengirim nama file dari client ke server, lalu server dapat mengirim content/isi dari file tersebut ke client jika ada.

|(download)|
|---|
|_hasil download inifile.txt_|

### 1e
Menghapus file, command *delete* diikuti nama file, dapat dilakukan dengan pertama mengecek apakah ada filenya atau tidak sekaligus mengganti nama filenya menjadi *old-Namafile.ekstensi* menggunakan `rename`. Fungsi `rename` akan mengembalikan nilai 0 jika filenya ada, mengembalikan nilai -1 jika tidak ada. Jika ada, maka baris yang mengandung nama file yang ingin dihapus akan dihapus dari *files.tsv*. Jika tidak ada, akan mengirim pesan error ke sisi client.

|(delete1)|
|---|
|_rename inifile.txt_|

|(delete2)|
|---|
|_hapus inifile.txt pada files.tsv_|


### 1f
Melihat semua isi file dapat dilakukan dengan melihat isi dari *files.tsv* lalu mengambil data yang perlu dan menyesuaikan format yang diberikan.

|(see)|
|---|
|_command see_|

### 1g
Mencari file juga dapat dilakukan dengan melihat isi dari *files.tsv* pada kolom pertama lalu mengambil namafile yang sesuai dengan argumen kedua yang diberikan menggunakan fungsi `strstr`.

|(find)|
|---|
|_find file_|

### 1h
Kegiatan log dapat dilakukan dengan mempassing kegiatannya apakah *Tambah* atau *Hapus* lalu nama file yang dilakukan perubahan atau penambahan, dan terakhir akun yang masuk dapat diambil dengan cara pada saat login di sisi server langsung menyimpan id dan password yang telah login pada waktu itu.

|(log)|
|---|
|_isi running.log_|

### Kendala
Dalam memahami bagaimana `socket` dengan mengimplementasikan `select` cukup membingungkan dan perlu mencari-cari di internet untuk referensi penggunaanya. Soal ini juga cukup panjang dari segi solusinya sehingga cukup bingung dan capek, but it's quite fun. Bisa jadi dari solusi saya yang belum terlalu teroptimasi. Yang cukup menghambat juga dari sisi eksternal adalah device yang dipakai terkadang menjadi lambat saat membuka VirtualBox, barangkali akhir semester dapat dilakukan servis.

|(manager)|
|---|
|_Tampak Task Manager_|

## Nomor 2
## Nomor 3
