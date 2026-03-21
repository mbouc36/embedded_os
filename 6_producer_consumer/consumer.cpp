#include <mqueue.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>

// Shared names (must match in all programs)
static const char* MQ_NAME = "/lab6_mq";
static const char* SEM_PROD_NAME = "/lab6_sem_prod";
static const char* SEM_CONS_NAME = "/lab6_sem_cons";

// Message format
static const int NUM_MESSAGES = 20;
static const size_t MSG_SIZE = 64;

int main() {
    // Open existing message queue
    mqd_t mq = mq_open(MQ_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        std::perror("mq_open (consumer)");
        return 1;
    }

    // Open existing semaphores
    sem_t* sem_prod = sem_open(SEM_PROD_NAME, 0);
    sem_t* sem_cons = sem_open(SEM_CONS_NAME, 0);

    if (sem_prod == SEM_FAILED || sem_cons == SEM_FAILED) {
        std::perror("sem_open (consumer)");
        mq_close(mq);
        return 1;
    }

    // Receive NUM_MESSAGES messages
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        char msg[MSG_SIZE];
        unsigned int prio = 0;

        if (sem_wait(sem_cons) == -1) {
            std::perror("sem_wait (consumer)");
            break;
        }

        ssize_t n = mq_receive(mq, msg, sizeof(msg), &prio);
        if (n == -1) {
            std::perror("mq_receive");
            sem_post(sem_prod); // avoid deadlock if producer is waiting
            break;
        } else {
            std::printf("Received: %s\n", msg);
        }

        if (sem_post(sem_prod) == -1) {
            std::perror("sem_post (consumer)");
            break;
        }
    }

    // Close handles
    mq_close(mq);
    sem_close(sem_prod);
    sem_close(sem_cons);

    return 0;
}