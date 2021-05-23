#include <stdio.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define sem_name "/sisopTugas"

pthread_t fact_func_do[4][6];

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


long long int fact(int val, int lim)
{
    if (val == 0) {
        return 1;
    }

    int i;
    long long int res = 1;
    for (i = val; i > lim || i > 1; --i) {
        if (i == lim) break;
        res *= i;
    }

    return res;
}

void input_max_fact_matrice(matrices_t *m)
{
    int i, j;

    printf("--- INPUT MATRIKS MAX FAKTORIAL (4x6) ---\n");
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        scanf("%d", &(m->max_fact[i][j]));
        // m->max_fact[i][j] = (j > 2)? j - 2 : j + 1;
    }}
}

void* calc_fact(void *mp)
{
    int i, j;

    matrices_t *m = (matrices_t*) mp;
    printf("--- MATRIKS AKHIR (4x6) ---\n");
    pthread_t id=pthread_self();
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        if (pthread_equal(id,fact_func_do[i][j])) {
            if (m->result[i][j] == 0 &&  m->max_fact[i][j] == 0) {
                m->result_fact[i][j] = 0;
            } else if (m->result[i][j] >= m->max_fact[i][j]) {
                m->result_fact[i][j] = fact(m->result[i][j], (m->result[i][j] - m->max_fact[i][j]));
            } else {
                m->result_fact[i][j] = fact(m->result[i][j], 1);
            }
        } else {
            continue;
        }
    }
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("BUTUH SATU ARGUMEN UNTUK PENGAMBILAN SHARED MEMORY");
        return -1;
    }
    sem_t *sem = sem_open(sem_name, 0);
    if (!sem) {
        printf("PROGRAM 1 BELUM DIJALANKAN\n");
        while(!(sem = sem_open(sem_name, 0))) {
            sleep(1);
        }
    }

    printf("PROGRAM 1 TELAH BERJALAN, MENUNGGU HASIL PERKALIAN MATRIKS...\n");
    fflush(stdout);

    sem_wait(sem);
    sem_unlink(sem_name);

    key_t key = ftok(argv[1], 1);
    // key_t key = 1235678;
    if (key == -1) {
        perror("ftok: ");
        return -1;
    }

    matrices_t *m;
    int shmid = shmget(key, sizeof(matrices_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmid: ");
        printf("shmid = %x, error = %d\n\n", shmid, errno);
        return -1;
    }

    m = shmat(shmid, NULL, 0);

    printf("\n## ARRAY HASIL YANG DITERIMA ##\n");
    int i, j;
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        printf("%d ", m->result[i][j]);
    }
        printf("\n");
    }
    printf("\n");

    input_max_fact_matrice(m);
    printf("\n");
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        pthread_create(&(fact_func_do[i][j]),NULL,&calc_fact,(void*)m);
    }}
    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        pthread_join(fact_func_do[i][j],NULL);
    }}

    for (i = 0; i < 4; ++i) {
    for (j = 0; j < 6; ++j) {
        printf("%llu ", m->result_fact[i][j]);
    }
        printf("\n");
    }
    shmdt(m);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
