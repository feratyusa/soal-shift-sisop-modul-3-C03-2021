#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define sem_name "/sisopTugas"

typedef struct matrices
{
    int 
        multiplicand[4][3],
        multiplier[3][6],
        max_fact[4][6];

    unsigned long long int
        result[4][6],
        result_fact[4][6];
} matrices_t;

void multiply(matrices_t *m)
{
    int i, j, k;

    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        m->result[i][j] = 0;
    for (k = 0; k < 3; ++k) {
        m->result[i][j] += m->multiplicand[i][k] * m->multiplier[k][j];
    }}}
}

void input_matrice(matrices_t *m)
{
    int i, j;

    printf("--- INPUT MATRIKS MULTIPLICAND (4x3) ---\n");
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 3; ++j) {
        scanf("%d", &(m->multiplicand[i][j]));
        // m->multiplicand[i][j] = (j > 2)? j - 2 : j + 1;
    }}

    printf("--- INPUT MATRIKS MULTIPLIER (3x6) ---\n");
    for (i = 0; i < 3; ++i) {
    for (j = 0; j < 6; ++j) {
        scanf("%d", &(m->multiplier[i][j]));
        // m->multiplier[i][j] = (j > 2)? j - 2 : j + 1;
    }}
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("BUTUH SATU ARGUMEN UNTUK PENGAMBILAN SHARED MEMORY");
        return -1;
    }
    sem_unlink(sem_name); // unlink kalau entah gimana sudah terset dulu
    sem_t *sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 1);
    if (sem == (void*) 0) {
        perror("sem: ");
        return -1;
    }
    matrices_t *m;

    sem_wait(sem);
    key_t key = ftok(argv[1], 1);
    if (key == -1) {
        perror("ftok: ");
        return -1;
    }

    int shmid = shmget(key, sizeof(matrices_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget: ");
        printf("shmid = %x, error = %d\n\n", shmid, errno);
        return -1;
    }

    m = shmat(shmid, NULL, 0);
    if (m == (void*) -1) {
        perror("Attachment: ");
        printf("Alamat m = %x\n\n", m);
        return -1;
    }
    
    input_matrice(m);
    multiply(m);

    sem_post(sem);
    shmdt(m);
    return 0;
}
