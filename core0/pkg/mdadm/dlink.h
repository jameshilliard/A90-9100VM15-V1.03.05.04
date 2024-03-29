
/* doubley linked lists */
/* This is free software. No strings attached. No copyright claimed */

struct __dl_head
{
    struct __dl_head *	dh_prev;
    struct __dl_head *	dh_next;
};

#define	dl_alloc(size)	((void*)(((char*)calloc(1,(size)+sizeof(struct __dl_head)))+sizeof(struct __dl_head)))
#define	dl_new(t)	((t*)dl_alloc(sizeof(t)))
#define	dl_newv(t,n)	((t*)dl_alloc(sizeof(t)*n))

#define dl_next(p) *((void**)&(((struct __dl_head*)(p))[-1].dh_next))
#define dl_prev(p) *((void**)&(((struct __dl_head*)(p))[-1].dh_prev))

void *dl_head(void);
char *dl_strdup(char *);
char *dl_strndup(char *, int);
void dl_insert(void*, void*);
void dl_add(void*, void*);
void dl_del(void*);
void dl_free(void*);
void dl_init(void*);
