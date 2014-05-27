/*
 * sp.c:
 * Split a file into chunks separated by a given pattern, piping each chunk to
 * a new program or writing it to a new file.
 *
 * Copyright (c) 2001 Chris Lightfoot. All rights reserved.
 * Email: chris@ex-parrot.com; WWW: http://www.ex-parrot.com/~chris/
 *
 */

static const char rcsid[] = "$Id: sp.c,v 1.6 2002/11/11 01:06:03 chris Exp $";

#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

/* sigchld:
 * SIGCHLD handler.
 */
volatile int num_children = 0;

void sigchld(int s) {
    pid_t pid;
    int status = 0;

    while (1) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0) --num_children;
        else if (pid == 0 || (pid == -1 && errno != EINTR)) return;
    }
}

/* xwrite:
 * POSIX write, ignoring signals.
 */
ssize_t xwrite(int fd, const void *buf, size_t count) {
    size_t c = count;
    const char *b = (const char*)buf;
    while (c > 0) {
        int e = write(fd, b, count);
        if (e > 0) {
            c -= e;
            b += e;
        } else return e;
    } while (c > 0);
    return count;
}

/* usage:
 * Print usage to a stream.
 */
void usage(FILE *fp) {
    fprintf(fp,
"sp: split a file into chunks separated by a given pattern\n"
"%s\n"
"\n"
"usage: sp [-spi] [-j num] pattern file program [arg] ...\n"
"   or  sp [-si] -f pattern file [name-spec]\n"
"   or  sp -h\n"
"\n"
"   -s          Include the separator line in the output written to the\n"
"               file/program.\n"
"\n"
"   -p          Ignore `broken pipe' errors.\n"
"\n"
"   -i          Match separator lines case-insensitively.\n"
"\n"
"   -j num      Run num processes simultaneously (default: 1).\n"
"\n"
"   -f          Write chunks to files named by name-spec, instead of piping\n"
"               them to a program. Name-spec should contain a single\n"
"               printf(3)-style format specification suitable for formatting\n"
"               an integer, which will be used insert a number identifying\n"
"               each chunk into its name (default: %%08d).\n"
"\n"
"   -h          Display this message.\n"
"\n"
"Pattern is a POSIX extended regular expression. To split data from standard\n"
"input, specify file as `-'. To split data in a file named `-', use\n"
"redirection. Numbering of chunks is affected by the environment variable\n"
"FILENO, which will be set in child programs. You can choose the base number\n"
"by setting FILENO when running sp (default: 0).\n"
"\n"
"Copyright (c) 2001 Chris Lightfoot <chris@ex-parrot.com>. Distribute freely.\n",
            rcsid);

}

/* start_program:
 * Starts the program given in the arguments, returning a file descriptor for
 * a pipe connected to its standard input or -1 on error.
 */
int max_children = 1;
extern char **environ;

int start_program(char *const argv[], const int fileno) {
    pid_t child;
    int pp[2];
    char f[32], **ep;

    /* Wait until we can start a job. */
    while (num_children >= max_children)
        usleep(10000);

    if (pipe(pp) == -1) return -1;
    switch ((child = fork())) {
        case -1:
            /* Error. */
            return -1;

        case 0:
            /* Child. */
            close(pp[1]);
            close(0);
            dup2(pp[0], 0); /* redirect stdin */

            /* Put the appropriate fileno in the environment. */
            sprintf(f, "FILENO=%d", fileno);
            putenv(f);

            /* Execute the given program. */
            if (execvp(argv[0], argv) == -1)
                fprintf(stderr, "sp: child process: %s\n", strerror(errno));
            _exit(1);

        default:
            /* Parent. */
            close(pp[0]);
            ++num_children;
            return pp[1];
    }
}

/* open_file:
 * Open a file having a suitable name, returning a file descriptor available
 * for writing, or -1 on error.
 */
int open_file(const char *name_spec, const int fileno) {
    char *buf = malloc(strlen(name_spec) * + 32);
    int fd;
    sprintf(buf, name_spec, fileno);
    fd = open(buf, O_WRONLY | O_CREAT, 0666);
    free(buf);
    return fd;
}

/* main:
 * Entry point for sp.
 */
char optstr[] = "+hj:fspi";

int main(int argc, char *argv[]) {
    int tofiles = 0, inclseps = 0, nocase = 0;
    regex_t pattern;
    char *filename, *name_spec = "%08d", *buffer, *bufp, *ff;
    size_t l_buffer;
    char **prog_argv;
    int ignore_sigpipe;
    int fileno = 0;
    int c, err, first = 1;
    int fd_out = -1, fd_in = -1;
    struct sigaction sa = {0};

    /* Process command-line options. */
    opterr = 0; /* process errors ourselves */
    while ((c = getopt(argc, argv, optstr)) != -1) {
        switch (c) {
            case 'h':
                usage(stdout);
                return 0;

            case 'j':
                max_children = atoi(optarg);
                if (max_children <= 0) {
                    fprintf(stderr, "sp: argument `%s' to -j does not make sense\n", optarg);
                    fprintf(stderr, "Type sp -h for usage information.\n");
                    return 1;
                }
                break;

            case 'f':
                tofiles = 1;
                break;

            case 's':
                inclseps = 1;
                break;

            case 'p':
                ignore_sigpipe = 1;
                break;

            case 'i':
                nocase = REG_ICASE;
                break;

            case '?':
            default:
                if (optopt == 'j' && !optarg)
                    fprintf(stderr, "sp: option -j requires an argument");
                else
                    fprintf(stderr, "sp: unrecognised option -%c\n", optopt);
                fprintf(stderr, "Type sp -h for usage information.\n");
                return 1;
        }
    }

    /* Set signal handlers. */
    sa.sa_flags = SA_RESTART;
    if (ignore_sigpipe) {
        sa.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sa, NULL);
    }

    sa.sa_handler = sigchld;
    sigaction(SIGCHLD, &sa, NULL);

    /* Now we need to find a pattern, filename, and program args or name-spec. */
    argv += optind;
    argc -= optind;

    if (argc < (tofiles ? 2 : 3)) {
        fprintf(stderr, "sp: not enough arguments\n");
        usage(stderr);
        return 1;
    }

    /* Compile the regular expression. */
    if ((err = regcomp(&pattern, argv[0], REG_EXTENDED | REG_NOSUB | REG_NEWLINE | nocase))) {
        char buf[1024];
        regerror(err, &pattern, buf, sizeof(buf));
        fprintf(stderr, "sp: %s\n", buf);
        return 1;
    }

    /* Find args of program or spec of files to pass data to. */
    if (tofiles && argv[2]) name_spec = argv[2];
    else prog_argv = argv + 2; 
    
    /* Open input file. */
    filename = argv[1];
    if (strcmp(argv[1], "-") == 0)
        fd_in = 0;
    else {
        fd_in = open(argv[1], O_RDONLY);
        if (fd_in == -1) {
            fprintf(stderr, "sp: open(`%s'): %s\n", argv[1], strerror(errno));
            return 1;
        }
    }

    /* Find out file number to use. */
    if ((ff = getenv("FILENO")))
        fileno = atoi(ff);    /* XXX */

    /* Allocate space for a line buffer. */
    l_buffer = getpagesize();
    bufp = buffer = (char*)malloc(l_buffer);
    
    /* Go into a loop reading and writing data. */
    while (1) {
        ssize_t n;
        n = read(fd_in, bufp, (buffer + l_buffer - bufp));
        if (n > 0) {
            char *q;

            /* Read some data. */
            bufp += n;

            /* Process lines. */
            while (bufp > buffer && (q = memchr(buffer, '\n', bufp - buffer))) {
                int match = 0;
                size_t len;
                *q = 0;
                len = q - buffer + 1;

                match = (regexec(&pattern, buffer, 0, NULL, 0) == 0);

                /* If we matched a separator, or if this is the first line
                 * we've seen, open a new file.
                 */
                if (first || match) {
                    if (fd_out != -1) close(fd_out);
                    fd_out = tofiles ? open_file(name_spec, fileno)
                                     : start_program(prog_argv, fileno);
                    ++fileno;
                }

                /* If appropriate, write the line out. */
                if (!match || inclseps) {
                    *q = '\n';
                    xwrite(fd_out, buffer, len);
                }

                /* Swallow up the data. */
                memmove(buffer, q + 1, l_buffer - len);
                bufp -= len;

                first = 0;
            }

            /* If there were no lines, realloc. */
            if (bufp >= buffer + l_buffer) {
                int a = bufp - buffer;
                l_buffer *= 2;
                buffer = realloc(buffer, l_buffer);
                bufp = buffer + a;
            }
        } else if (n == 0) {
            close(fd_in);
            return 0;
        } else {
            close(fd_in);
            fprintf(stderr, "sp: %s: %s\n", fd_in ? filename : "standard input", strerror(errno));
        }
    }
}
