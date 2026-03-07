#include <mqueue.h>
#include <semaphore.h>
#include <cstdio>
#include <cerrno>
#include <cstring>

static const char* MQ_NAME = "/lab6_mq";
static const char* SEM_PROD_NAME = "/lab6_sem_prod";
static const char* SEM_CONS_NAME = "/lab6_sem_cons";

int main() {
    if (mq_unlink(MQ_NAME) == -1) {
        std::printf("mq_unlink: %s\n", std::strerror(errno));
    } else {
        std::printf("Message queue removed: %s\n", MQ_NAME);
    }

    if (sem_unlink(SEM_PROD_NAME) == -1) {
        std::printf("sem_unlink(prod): %s\n", std::strerror(errno));
    } else {
        std::printf("Semaphore removed: %s\n", SEM_PROD_NAME);
    }

    if (sem_unlink(SEM_CONS_NAME) == -1) {
        std::printf("sem_unlink(cons): %s\n", std::strerror(errno));
    } else {
        std::printf("Semaphore removed: %s\n", SEM_CONS_NAME);
    }

    return 0;
}