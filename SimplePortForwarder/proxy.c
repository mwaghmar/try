/*
 * proxy:
 * Proxy/forward arbitrary tcp ports to other arbitrary tcp ports on other
 * hosts, perhaps checking that the client is allowed to connect using an
 * authorisation file listing either hosts or regular expressions matching
 * host names or equivalent dotted internet addresses.
 *
 * Also a good example of the Microsoft BASIC Memorial Not Many Subroutines
 * Here form of programming.
 *
 * Copyright (c) 1998-2001 Chris Lightfoot <chris@ex-parrot.com>. Additions
 * thanks to Richard Smith.
 *
 * This code is covered by the GNU public license of any version you choose.
 * 
 */
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>

const static char rcsid[] = "$Id: proxy.c,v 1.4 2002/11/11 01:06:03 chris Exp $";

#define PROGNAME    "proxy"
#define VERSION     "0.2"

/* strategy is to listen on one socket, then connect on another when that */
/* socket is connected */
int s_listen;
int s_target;
int s_client;

struct in_addr client_host;
struct sockaddr_in local_address, target_address, client_address;

char *target_name;
const char *format_ = "%a %b %d %H:%M:%S %Y";

#define MAXPEERNAME 256
char client_name[256];

/* the numeric name of the client */
char client_numname[20];

struct in_addr target_host;
int local_port, target_port;

/* struct bw_scoreboard:
 * `Scoreboard' shared state structure used to enforce bandwidth limits among
 * several processes.
 */
struct bw_scoreboard {
    long long bwlimit, bwused;
    long double tick;
    int sem;
};

int fdscoreboard;
struct bw_scoreboard *scoreboard;

/* gettime:
 * Return the `time of day' as a long double.
 */
long double gettime() {
    struct timeval tv;
    long double l;
    gettimeofday(&tv, NULL);
    l = (long double)tv.tv_sec + 1e-6 * (long double)tv.tv_usec;
    return l;
}

/* alloc_scoreboard:
 * Create and map a scoreboard file. Returns 0 on success or -1 on failure.
 */
int alloc_scoreboard() {
    char templ[] = "proxyXXXXXX";
    fdscoreboard = mkstemp(templ);
    if (fdscoreboard == -1) return -1;
    ftruncate(fdscoreboard, sizeof(struct bw_scoreboard));
    if (unlink(templ) == -1) {
        close(fdscoreboard);
        return -1;
    }

    scoreboard = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fdscoreboard, 0);
    if (scoreboard == MAP_FAILED) {
        close(fdscoreboard);
        return -1;
    }

    return 0;
}

/* lock_scoreboard:
 * Lock the scoreboard that we may fiddle with its contents. Returns 0 on
 * success or -1 on failure.
 */
int lock_scoreboard(void) {
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    return fcntl(fdscoreboard, F_SETFL, &fl);
}

/* unlock_scoreboard:
 * Unlock the scoreboard once we're done with it. Returns 0 on success or -1
 * on failure.
 *
 * XXX uses fcntl, which is slow.
 */
int unlock_scoreboard(void) {
    struct flock fl = {0};
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    return fcntl(fdscoreboard, F_SETFL, &fl);
}

/* atoport:
 * Convert a string into a port in network byte order.
 */
int atoport(char *service, char *proto)
{
    int port;
    long int lport;
    struct servent *serv;
    char *errpos;

    /* first try to read it from /etc/services */
    serv = getservbyname(service, proto);

    if (serv != NULL)
        port = serv->s_port;

    /* try it as a number */
    else {
        lport = strtol(service, &errpos, 0);

        if ((errpos[0] != 0) || (lport < 1) || (lport > 65535))
            return -1;

        port = htons(lport);
    }

    return port;
}

/* atoaddr:
 * Convert a string to an IP address.
 */
struct in_addr *atoaddr(char *address)
{
    struct hostent *host;
    static struct in_addr saddr;

    /* first try it as aaa.bbb.ccc.ddd */
    saddr.s_addr = inet_addr(address);
    if (saddr.s_addr != -1)
        return &saddr;

    /* now as a symbolic name */
    host = gethostbyname(address);

    if (host != NULL)
        return (struct in_addr *) *host->h_addr_list;

    return NULL;
}

/* atobytes:
 * Convert a string to a number of bytes.
 */
long long atobytes(const char *bytes)
{
    long long b;
    char *epos;

    b = (long long) strtol(bytes, &epos, 10);

    if (*epos == 'b')
        b >>= 3, ++epos;
    else if (*epos == 'k')
        b <<= 10, ++epos;
    else if (*epos == 'M')
        b <<= 20, ++epos;
    else if (*epos == 'G')
        b <<= 30, ++epos;

    if (*epos != '\0')
        return -1ll;
    else
        return b;
}

/* struct hostmatch:
 * ACL entry.
 */
struct hostmatch {
#define HOST    1
#define REGEXP  2
    int type;

    struct in_addr address;
    regex_t regexp;

    struct hostmatch *next;
};

struct hostmatch *hm;

#define FILEBUFFERLEN	1024

/* read_authfile:
 * Obtain authentication entries from the passed filename, storing them in
 * the global ACL.
 */
int read_authfile(char *filename)
{
    char *buffer, *p, *q;
    struct hostmatch *h;
    struct in_addr *addr;
    regex_t rx;
    FILE *fp;
    int i, j;

    buffer = (char *) malloc(FILEBUFFERLEN);

    if (!buffer) {
        perror(PROGNAME ": malloc");
        return -1;
    }

    fp = fopen(filename, "rt");

    if (!fp) {
        perror(PROGNAME ": fopen");
        return -1;
    }

    i = 0;

    while (!feof(fp)) {
        i++;
        fgets(buffer, FILEBUFFERLEN, fp);

        for (p = buffer; *p && isspace(*p); p++);

        if (*p && *p != '#') {
            for (q = p; *q && !isspace(*q) && *q != '#'; q++);

            *q = 0;

            if (*p == '/') {
                if (*(q - 1) == '/') {
                    p++;
                    q--;
                    *q = 0;

                    j = regcomp(&rx, p, REG_EXTENDED | REG_NOSUB | REG_ICASE);

                    if (j)
                        fprintf(stderr, PROGNAME ": %s: badly formed regexp near line %d\n", filename, i);

                    else {
                        h = (struct hostmatch *) malloc(sizeof(struct hostmatch));

                        if (!h) {
                            perror(PROGNAME ": malloc");
                            return -1;
                        }

                        h->type = REGEXP;

                        memcpy(&(h->regexp), &rx, sizeof(rx));

                        h->next = hm;
                        hm = h;
                    }

                } else
                    fprintf(stderr, PROGNAME ": %s: badly formed regexp near line %d\n", filename, i);
            } else {
                addr = atoaddr(p);

                if (!addr)
                    fprintf(stderr, PROGNAME ": %s: unknown host near line %d\n", filename, i);
                else {
                    h = (struct hostmatch *) malloc(sizeof(struct hostmatch));

                    if (!h) {
                        perror(PROGNAME ": malloc");
                        return -1;
                    }

                    h->type = HOST;

                    memcpy(&(h->address), atoaddr(p), sizeof(h->address));

                    h->next = hm;
                    hm = h;
                }
            }
        }
    }

    fclose(fp);

    return 0;
}

/* check_auth:
 * Check the client host against the auth entries. Returns 1 if the host
 * matches, or 0 otherwise.
 */
int check_auth()
{
    struct hostmatch *h;

    for (h = hm; h; h = h->next) {
        if (h->type == HOST) {
            if (!memcmp(&client_host, &(h->address), sizeof(client_host)))
                return 1;
        } else if (h->type == REGEXP) {
            if (!regexec(&(h->regexp), client_name, 0, NULL, 0) || !regexec(&(h->regexp), client_numname, 0, NULL, 0))
                return 1;
        }
    }

    return 0;
}

/* sigchld_handler:
 * Handle SIGCHLD messages.
 */
void sigchld_handler(int i) {
    while (1) {
        pid_t p;
        p = waitpid(-1, NULL, WNOHANG);
        if (p > 0) {
            /* Reaped child */
            ;
        } else if (p == 0                       /* No child */
                || p == -1 && errno != EINTR)   /* Error other than interrupted */
            return; /* Done */
    }
    /*NOTREACHED*/
}

/* printhelp:
 * Print a usage message.
 */
void printhelp() {
    fprintf(stderr,
            "proxy: proxy a tcp service from one machine to another\n"
            "%s\n"
            "\n"
            "usage: proxy [options]\n"
            "       -h            Display this message.\n"
            "       -v            Be verbose.\n"
            "\n"
            "       -l port       Local port to listen on.\n"
            "       -t host:port  Target to connect to.\n"
            "\n"
            "       -a file       File listing hosts allowed to use proxy.\n"
            "       -d file       File listing hosts not allowed to use proxy.\n"
            "                     Only one of -a, -d may be given.\n"
            "\n"
            "       -D file       Dump all data received to file, or standard\n"
            "                     output if `-'; file will be suffixed with\n"
            "                     a timestamp and process ID.\n"
            "\n"
            "       -L speed      Limit downstream bandwidth to n bytes per second\n"
            "                     per connection (may use the suffices b(bits),\n"
            "                     k, M, or G). Bandwidth usage is aggregated in\n"
            "                     0.1s intervals.\n"
            "\n"
            "Copyright (c) 1998-2001 Chris Lightfoot <chris@ex-parrot.com>.\n"
            "Distribute freely.\n", rcsid);
}

#define BUFFERLEN   8192

#define AUTHNULL    0
#define AUTHALLOW   1
#define AUTHDENY    2

/* main:
 * Entry point for proxy.
 */
char optstr[] = "hvl:t:a:d:D:L:s";

int main(int argc, char **argv)
{
    char *t;

    int verbosity = 0;
    int dumppackets = 0;
    char *dump_filename;
    int authmode = AUTHNULL;

    struct in_addr *ia;
    struct hostent *he;

    int reuse_addr = 1;
    int l, m;

    char *x_buffer;
    size_t x_buffer_len;

    pid_t child;

    int c;

    struct sigaction sig;

    /* ignore SIGPIPE in case of error on sockets */
    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE, &sig, NULL);

    /* handle SIGCHLD to avoid zombie processes */
    sig.sa_handler = sigchld_handler;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGCHLD, &sig, NULL);

    if (argc == 1) {
        fprintf(stderr, PROGNAME ": you must supply arguments\n");
        printhelp();
        return 1;
    }

    /* process arguments */
    opterr = 0; /* process errors ourself */
    while ((c = getopt(argc, argv, optstr)) != -1 ) {
        switch (c) {
            case 'h':
                /* help etc. */
                printhelp();
                return 0;

            case 'v':
                verbosity = 1;
                break;

            case 'D':
                /* dump packets to a file */
                if (dumppackets) {
                    fprintf(stderr, PROGNAME ": you have already given a dump file template\n");
                    return 1;
                } else {
                    dumppackets = 1;
                    dump_filename = optarg;
                }
                break;

            case 'L':
                if (scoreboard) {
                    fprintf(stderr, PROGNAME ": you have already given a bandwidth limit\n");
                    return 1;
                } else {
                    long long l = atobytes(optarg);

                    if (l == -1ll) {
                        fprintf(stderr, PROGNAME ": %s doesn't look like a size to me\n", optopt);
                        return 1;
                    } else if (l < 16ll) {
                        fprintf(stderr, PROGNAME ": I refuse to set the bandwidth to less than 128 bps\n");
                        return 1;
                    } else if (alloc_scoreboard() == -1) {
                        perror(PROGNAME ": alloc_scoreboard");
                        return 1;
                    } else {
                        /* no need to lock it yet */
                        memset(scoreboard, 0, sizeof(struct bw_scoreboard));
                        scoreboard->bwlimit = l;
                    }
                }
                break;

            case 'l':
                if (local_port) {
                    fprintf(stderr, PROGNAME ": you may only specify one local port\n");
                    return 1;
                } else {
                    local_port = atoport(optarg, "tcp");

                    if (local_port == -1) {
                        fprintf(stderr, PROGNAME ": the local port was not resolved\n");
                        return 1;
                    }
                    if (getuid() && local_port < 1024) {
                        fprintf(stderr, PROGNAME ": the local port must be unprivileged unless running as root\n");
                        return 1;
                    }
                }
                break;

            case 't':
                if (target_port || target_name) {
                    fprintf(stderr, PROGNAME ": you may only specify one target\n");
                    return 1;
                } else {
                    t = strchr(optarg, ':');

                    if (!t) {
                        fprintf(stderr, PROGNAME ": target should be specified as host:port\n");
                        return 1;
                    }

                    *t++ = 0;

                    target_name = strdup(optarg);
                    if (!target_name) {
                        perror(PROGNAME ": strdup");
                        return 1;
                    }

                    /* try to resolve the target port */
                    target_port = atoport(t, "tcp");
                    if (target_port == -1) {
                        fprintf(stderr, PROGNAME ": the target port was not resolved\n");
                        return 1;
                    }

                    /* now try to resolve the target name */
                    ia = atoaddr(target_name);
                    if (!ia) {
                        fprintf(stderr, PROGNAME ": the target host was not resolved\n");
                        return 1;
                    }

                    memcpy(&target_host, ia, sizeof(struct in_addr));
                }
                break;

            case 'a':
            case 'd':
                /* authentication modes. */
                if (authmode != AUTHNULL) {
                    fprintf(stderr, PROGNAME ": you may only specify a single -a or -d option\n");
                    return 1;
                } else if (read_authfile(optarg) < 0)
                    return 1;

                authmode = (optopt == 'a') ? AUTHALLOW : AUTHDENY;
                break;

            case '?':
            default:
                if (strchr(optstr, optopt) && *(strchr(optstr, optopt) + 1) == ':')
                    fprintf(stderr, PROGNAME ": option -%c requires an argument\n", optopt);
                else
                    fprintf(stderr, PROGNAME ": unrecognised option -%c\n", optopt);
                fprintf(stderr, "Type " PROGNAME " -h for usage information.\n");
                return 1;
        }
    }

    if (!local_port || !target_port || !target_name) {
        fprintf(stderr, PROGNAME ": you must specify a local port and a target host and port\n");
        return 1;
    }

    /* create a socket to listen on */
    s_listen = socket(AF_INET, SOCK_STREAM, 0);

    if (s_listen < 0) {
        perror(PROGNAME ": socket(s_listen)");
        return 1;
    }

    /* set up the listening address */
    memset(&local_address, 0, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_port = local_port;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the target address */
    memset(&target_address, 0, sizeof(target_address));
    target_address.sin_family = AF_INET;
    target_address.sin_port = target_port;
    memcpy(&(target_address.sin_addr), &target_host, sizeof(target_host));

    /* reuse the address */
    if (setsockopt(s_listen, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        perror(PROGNAME ": setsockopt");
        return 1;
    }
    
    /* bind to address */
    if (bind(s_listen, (struct sockaddr *) &local_address, sizeof(local_address)) < 0) {
        perror(PROGNAME ": bind");
        return 1;
    }
    
    /* start listening */
    if (listen(s_listen, SOMAXCONN) < 0) {
        perror(PROGNAME ": listen");
        return 1;
    }
    
    /* be garrulous */
    if (verbosity) {
        fprintf(stderr, PROGNAME ": being garrulous\n");
        fprintf(stderr, PROGNAME ": listening on local port %d\n", ntohs(local_port));
        fprintf(stderr, PROGNAME ": forwarding connections to %s:%d\n", target_name, ntohs(target_port));

        switch (authmode) {
            case AUTHNULL:
                fprintf(stderr, PROGNAME ": not using an auth file\n");
                break;

            case AUTHALLOW:
                fprintf(stderr, PROGNAME ": allowing access to hosts in auth file\n");
                break;
                
            case AUTHDENY:
                fprintf(stderr, PROGNAME ": denying access to hosts in auth file\n");
                break;
        }

        if (scoreboard)
            fprintf(stderr, PROGNAME ": limiting bandwidth to %lld bps\n", scoreboard->bwlimit * 8);
    }

    /* now we get to the meat of the problem
     *
     * we listen for incoming connections, and fork off sub-processes to
     * deal with them.
     */
    for (;;) {
        int this_pid;

        l = sizeof(client_address);
        s_client = accept(s_listen, (struct sockaddr*)&client_address, &l);

        if (s_client < 0) {
            if (errno != EINTR) perror(PROGNAME ": accept");
            continue;
        }
        
        /* try to find out who is connecting */
        he = gethostbyaddr((char *) &client_address.sin_addr, sizeof(client_address.sin_addr), AF_INET);

        strcpy(client_numname, inet_ntoa(client_address.sin_addr));
        memcpy(&client_host, &(client_address.sin_addr), sizeof(struct in_addr));

        if (!he) {
            if (verbosity) fprintf(stderr, PROGNAME ": failed to resolve symbolic name for peer\n");
        } else strcpy(client_name, he->h_name);

        if (verbosity) fprintf(stderr, PROGNAME ": got connection from %s\n", *client_name ? client_name : client_numname);

        child = fork();

        if (child < 0) {          /* oops */
            perror(PROGNAME ": fork");
        } else if (child > 0)   /* this is still the parent */
            /* close our copy of connected socket */
            close(s_client);
        else if (!child) {      /* this is the child */
            int dump_fd = -1;

            /* close our copy of listening socket */
            close(s_listen);

            this_pid = getpid();

            if (verbosity)
                fprintf(stderr, PROGNAME ": forked child %d to handle connection\n", this_pid);

            if (authmode != AUTHNULL) {
                l = check_auth();

                if (verbosity)
                    fprintf(stderr, PROGNAME ": %s client in authorisation list\n", l ? "found" : "did not find");

                if ((l && authmode == AUTHDENY) || (!l && authmode == AUTHALLOW)) {
                    if (verbosity)
                        fprintf(stderr, PROGNAME ": rejected connection from %s\n", *client_name ? client_name : client_numname);

                    shutdown(s_client, 2);

                    return 0;
                }
            }

            if (!scoreboard || BUFFERLEN < (scoreboard->bwlimit / 100))
                x_buffer_len = BUFFERLEN;
            else
                x_buffer_len = (scoreboard->bwlimit / 100);

            x_buffer = (char *) malloc(x_buffer_len);

            if (!x_buffer) {
                perror(PROGNAME ": malloc");
                return 1;
            }

            /* create also a socket to connect on */
            s_target = socket(AF_INET, SOCK_STREAM, 0);

            if (s_target < 0) {
                perror(PROGNAME ": socket(s_target)");
                return 1;
            }
            
            /* now we connect to the remote host */
            if (connect(s_target, (struct sockaddr*)&target_address, sizeof(target_address)) < 0) {
                perror(PROGNAME ": connect");
                return 1;
            }

            /* record start time. */
            if (scoreboard) scoreboard->tick = gettime();

            for (;;) {
                fd_set rd_set;
                struct timeval timeout;
                
                /* we go into a loop select'ing on the two sockets */
                FD_SET(s_client, &rd_set);
                FD_SET(s_target, &rd_set);

                timeout.tv_sec = 0;
                timeout.tv_usec = 50000;	/* 0.05s */

                if (select((s_client > s_target ? s_client : s_target) + 1, &rd_set, NULL, NULL, &timeout)) {
                    /* can we read from client and write to target? */
                    if (FD_ISSET(s_client, &rd_set)) {
                        l = read(s_client, x_buffer, x_buffer_len);

                        if (l < 0 && errno != EINTR) {
                            perror(PROGNAME ": read(client)");
                            return 1;
                        } else if (!l) {
                            if (verbosity)
                                fprintf(stderr, PROGNAME ": closed connection from %s\n", *client_name ? client_name : client_numname);

                            if (shutdown(s_client, 2) < 0)
                                perror(PROGNAME ": shutdown(client)");
                            if (shutdown(s_target, 2) < 0)
                                perror(PROGNAME ": shutdown(target)");

                            if (verbosity)
                                fprintf(stderr, PROGNAME ": child %d exiting\n", this_pid);

                            return 0;
                        }

                        for (t = x_buffer; l > 0;) {
                            m = write(s_target, t, l);

                            /* we may need to dump transferred data to a file or standard output. */
                            if (dumppackets) {
                                if (strcmp(dump_filename, "-") == 0) {
                                    /* dump to standard output. */
                                    char buffer[1024], tbuf[128];
                                    int bl;
                                    time_t time_ = time(NULL);
                                    strftime(tbuf, 128, format_, gmtime(&time_));
                                    bl = snprintf(buffer, 1024, "\r\n[[Pid: %d --%s]]\r\n", this_pid, tbuf);
                                    write(1, buffer, bl);
                                    dump_fd = 1;
                                } else if (dump_fd == -1) {
                                    /* dump to some random file. */
                                    char *buffer;
                                    buffer = (char*)malloc(strlen(dump_filename) + 24);
                                    sprintf(buffer, "%s.%d.%d", dump_filename, (int)getpid(), (int)time(NULL));
                                    dump_fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, 0666); /* XXX not NFS safe (but what is?) */
                                    if (dump_fd == -1)
                                        fprintf(stderr, PROGNAME ": dump file `%s': %s\n", buffer, strerror(errno));
                                    else if (verbosity)
                                        fprintf(stderr, PROGNAME ": dumping to file `%s'\n", buffer);
                                    free(buffer);
                                }

                                if (dump_fd != -1) write(dump_fd, t, l);
                            }

                            if (m > 0) {
                                t += m;
                                l -= m;
#if 0                           /* don't count upstream bandwidth */
                                if (scoreboard) {
                                    lock_scoreboard();
                                    scoreboard->bwused += m;
                                    unlock_scoreboard();
                                }
#endif
                            } else if (errno != EINTR) {
                                perror(PROGNAME ": write(target)");
                                return 1;
                            }

                        }
                    }

                    /* can we read from target and write to client? */
                    if (FD_ISSET(s_target, &rd_set)) {
                        l = read(s_target, x_buffer, x_buffer_len);

                        if (l < 0 && errno != EINTR) {
                            perror(PROGNAME ": read(target)");
                            return 1;
                        } else if (!l) {
                            if (verbosity)
                                fprintf(stderr, PROGNAME ": closed connection from %s\n", *client_name ? client_name : client_numname);

                            if (shutdown(s_client, 2) < 0)
                                perror(PROGNAME ": shutdown(client)");
                            if (shutdown(s_target, 2) < 0)
                                perror(PROGNAME ": shutdown(target)");

                            if (verbosity)
                                fprintf(stderr, PROGNAME ": child %d exiting\n", this_pid);
                            return 0;
                        }

                        for (t = x_buffer; l > 0;) {
                            m = write(s_client, t, l);

                            if (dumppackets && dump_fd != -1) {
                                if (dump_fd == 1) {
                                    char buffer[1024], tbuf[128];
                                    int bl;
                                    time_t time_ = time(NULL);
                                    strftime(tbuf, 128, format_, gmtime(&time_));
                                    bl = snprintf(buffer, 1024, "\r\n[[Pid: %d --%s]]\r\n", this_pid, tbuf);
                                    write(1, buffer, bl);
                                }
                                
                                write(dump_fd, t, l);
                            }

                            if (m > 0) {
                                t += m;
                                l -= m;
                                /* account downstream bandwidth */
                                if (scoreboard) {
                                    lock_scoreboard();
                                    scoreboard->bwused += m;
                                    unlock_scoreboard();
                                }
                            } else if (errno != EINTR) {
                                perror(PROGNAME ": write(client)");
                                return 1;
                            }
                        }
                    }

                    /* Figure out the rate of bandwidth usage going on at the
                     * moment, and, if it is too high, sleep until it declines
                     * to below the limit.
                     */
                    if (scoreboard) {
                        long double delta, rate;

                        do {
                            delta = gettime() - scoreboard->tick;
                            rate = scoreboard->bwused / delta;
                            if (rate > scoreboard->bwlimit)
                                usleep(100);
                        } while (rate > scoreboard->bwlimit);
                        
                        lock_scoreboard();
                        /* Perhaps reset timer. */
                        if ((gettime() - scoreboard->tick) > 0.5) {
                            scoreboard->tick = gettime();
                            scoreboard->bwused = 0;
                        }
                        unlock_scoreboard();
                    }
                }
            }
        }
    }
}
