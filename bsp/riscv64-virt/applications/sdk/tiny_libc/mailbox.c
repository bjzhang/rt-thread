#include <mailbox.h>
#include <string.h>

#define MAX_NUM_BOX 32
mailbox_t mailbox[MAX_NUM_BOX];
mthread_mutex_t mail_lock;
mailbox_t *mbox_open(char *name)
{
    // TODO:
    //mthread_mutex_lock(&mail_lock);
    int len = strlen(name);
    int hash = 0;
    for (int i = 0; i < len; ++i) {
        hash += name[i] & 0xdeadbeef;
    }
    hash %= MAX_NUM_BOX;
    while (mailbox[hash].allocated && strcmp(mailbox[hash].mailbox_name,name) != 0) {
        ++hash;
        hash %= MAX_NUM_BOX;
    }
    if (mailbox[hash].allocated) {
        // already have one 
        //mthread_mutex_unlock(&mail_lock);
        return &(mailbox[hash]);
    }
    mailbox[hash].allocated = 1;
    strcpy(&(mailbox[hash].mailbox_name),name);
    mailbox[hash].info_valid = 0;
    
    mthread_cond_init(&mailbox[hash].condition);
    mthread_mutex_init(&mailbox[hash].mutex);
    //mthread_mutex_unlock(&mail_lock);
    return &(mailbox[hash]);
}

void mbox_close(mailbox_t *mailbox)
{
    // TODO:
    mthread_mutex_lock(&mail_lock);
    mailbox->allocated = 0;
    mthread_mutex_unlock(&mail_lock);
}

void mbox_send(mailbox_t *mailbox, void *msg, int msg_length)
{
    // TODO:
    mthread_mutex_lock(&mail_lock);
    while (mailbox->info_valid == 1) {
        mthread_cond_wait(&mailbox->condition,&mail_lock);
    }
    mailbox->info_valid = 1;
    memcpy(&(mailbox->mailbox_data), msg, msg_length);
    mthread_mutex_unlock(&mail_lock);
    mthread_cond_broadcast(&(mailbox->condition));
}

void mbox_recv(mailbox_t *mailbox, void *msg, int msg_length)
{
    // TODO:
    mthread_mutex_lock(&mail_lock);
    while (mailbox->info_valid == 0) {
        mthread_cond_wait(&mailbox->condition,&mail_lock);
    }
    mailbox->info_valid = 0;
    memcpy(msg, mailbox->mailbox_data, msg_length);

    mthread_mutex_unlock(&mail_lock);
    mthread_cond_broadcast(&(mailbox->condition));
}
