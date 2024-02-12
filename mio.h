#ifndef MIO_H_
#define MIO_H_
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h> //for isdigit


// Defined Constants
#define MBSIZE 10    // default size for buffers
#define MODE_R 0    // read only
#define MODE_WA 1    // write only create/append
#define MODE_WT 2    // write only truncate
#define MTAB '\t'       // Tab
#define MNLINE '\n'    // Newline
#define MCRET '\r'    // Carriage return
#define MSPACE ' '    // Space

// Macros
// Is char X whitespace: 1 - yes, 0 - no
#define M_ISWS(X) (((X==MTAB)||(X==MNLINE)||(X==MSPACE)||(X==MCRET)) ? (1) : (0))
// Is int X mode a write type: 1 - yes, 0 - no
#define M_ISMW(X) (((X==MODE_WA)||(X==MODE_WT)) ? (1) : (0))




// mininum information for MILE
struct _mile {
    int fd;                    // file descriptor
    int rw;                   // 0 - read, 1 - write append, 2 - write truncate
    char *rb, *wb;           // buffers
    int bsize;              //buffer size
    int rsize, wsize;       // buffer sizes
    int rs, re, ws, we;    // buffer indices
};
typedef struct _mile MILE;

//globals
extern MILE *mtdin;
extern MILE *mtdout;
extern MILE *mtderr;

//minit function
void minit();

// open/close functions
MILE *mopen(const char *name, const int mode, const int bsize);
MILE *mdopen(const int fd, const int mode, const int bsize);
int mclose(MILE *m);

// read functions
int mread(MILE *m, char* const b, const int size);
int mgetc(MILE *m, char *c);
char *mgets(MILE *m, int *len);

char *mgetline(MILE *m, int *length); // for MyShell
//char **mgetline(MILE *m, int *num_tokens);

// write functions
int mwrite(MILE *m, const char *b, const int size);
int mflush(MILE *m);
int mputc(MILE *m, const char c);
int mputs(MILE *m, const char *str, const int len);

int mgeti(MILE *m, int *val);
int mputi(MILE *m, const int val);

#endif
