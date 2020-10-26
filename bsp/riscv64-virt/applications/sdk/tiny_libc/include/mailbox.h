#ifndef INCLUDE_MAIL_BOX_
#define INCLUDE_MAIL_BOX_

#include <mthread.h>

#define MAX_MBOX_LENGTH (64)

typedef struct mailbox
{
    // TODO:
    char mailbox_name[MAX_MBOX_LENGTH];
    char mailbox_data[MAX_MBOX_LENGTH];
    mthread_mutex_t mutex;
    mthread_cond_t condition;
    int allocated;
    int info_valid; 
} mailbox_t;


mailbox_t *mbox_open(char *);
void mbox_close(mailbox_t *);
void mbox_send(mailbox_t *, void *, int);
void mbox_recv(mailbox_t *, void *, int);

#endif
