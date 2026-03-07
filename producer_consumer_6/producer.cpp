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
    // Create/open message queue
    struct mq_attr attr;
    std::memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;

    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        std::perror("mq_open (producer)");
        return 1;
    }

    // Create/open semaphores
    sem_t* sem_prod = sem_open(SEM_PROD_NAME, O_CREAT, 0666, 1);
    sem_t* sem_cons = sem_open(SEM_CONS_NAME, O_CREAT, 0666, 0);

    if (sem_prod == SEM_FAILED || sem_cons == SEM_FAILED) {
        std::perror("sem_open (producer)");
        mq_close(mq);
        return 1;
    }

    // Send NUM_MESSAGES messages
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        char msg[MSG_SIZE];
        std::snprintf(msg, sizeof(msg), "Producer -> %d", i);

        if (sem_wait(sem_prod) == -1) {
            std::perror("sem_wait (producer)");
            break;
        }

        if (mq_send(mq, msg, std::strlen(msg) + 1, 0) == -1) {
            std::perror("mq_send");
            sem_post(sem_cons); // avoid deadlock if consumer is waiting
            break;
        }

        std::printf("Sent: %s\n", msg);

        if (sem_post(sem_cons) == -1) {
            std::perror("sem_post (producer)");
            break;
        }
    }

    // Close handles
    mq_close(mq);
    sem_close(sem_prod);
    sem_close(sem_cons);

    return 0;
}