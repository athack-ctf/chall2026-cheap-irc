// gcc server.c -o server -O0 -g -fno-stack-protector -no-pie
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>

#define PORT 6667
#define MAX_USERS 16
#define MAX_MSGS 128

/* =======================
   Structures (VULN CORE)
   ======================= */

typedef struct {
    char name[32];
    int is_admin;
    int fd;
} User;

typedef struct {
    char text[32];
    int length;
    int id;
} Message;

/* =======================
   Globals (shared heap)
   ======================= */

User *active_users[MAX_USERS];
Message *messages[MAX_MSGS];
int user_count = 0;
int msg_count = 0;
int global_m_id = 0;

/* =======================
   Utility
   ======================= */


void dump_active_users(void) {
    puts("==== active_users dump ====");

    for (int i = 0; i < MAX_USERS; i++) {
        User *u = active_users[i];

        if (!u) {
            printf("[%d] NULL\n", i);
            continue;
        }

        printf(
            "[%d] ptr=%p | fd=%d | is_admin=%d | name=\"%s\"\n",
            i,
            (void *)u,
            u->fd,
            u->is_admin,
            u->name
        );
    }

    puts("===========================");
}


void send_to_all(const char *msg) {
    for (int i = 0; i < user_count; i++) {
        if (active_users[i]) {
            write(active_users[i]->fd, msg, strlen(msg));
        }
    }
}

void banner(int fd) {
    const char *b =
        "==============================\n"
        " Welcome to my cHeap IRC \nCome spreayd your joy!"
        "==============================\n"
        "Commands:\n"
        "  /users   - list users\n"
        "  /quit    - leave\n\n";
    write(fd, b, strlen(b));
}

int find_user_index(const char *name) {
    for (int i = 0; i < user_count; i++) {
        if (active_users[i] &&
            strcmp(active_users[i]->name, name) == 0)
            return i;
    }
    return -1;
}






void list_users(int fd) {
    write(fd, "Users:\n", 7);
    for (int i = 0; i < user_count; i++) {
        if (active_users[i]) {
            write(fd, " - ", 3);
            write(fd, active_users[i]->name,
                  strlen(active_users[i]->name));
            write(fd, "\n", 1);
        }
    }
}

void broadcast_join(User *u) {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "*** %s joined the channel\n", u->name);
    fprintf(stderr, "*** %s joined the channel\n", u->name);
    send_to_all(buf);
}

void broadcast_leave(User *u) {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "*** %s left the channel\n", u->name);
    fprintf(stderr, "*** %s left the channel\n", u->name);
    send_to_all(buf);
}

void handle_message(User *u, const char *msg) {
    Message *m = malloc(sizeof(Message));
    strncpy(m->text, msg, 31);
    m->text[strcspn(m->text, "\n")] = 0;
    m->text[31] = 0;
    m->length = strlen(msg);
    m->id = global_m_id++;

    messages[msg_count++ % MAX_MSGS] = m;

    char buf[256];
    if (u->is_admin) {
        snprintf(buf, sizeof(buf),
                 "[ADMIN][%s] %s", u->name, msg);
        fprintf(stderr, "[ADMIN][%s] %s", u->name, msg);
    } else {
        snprintf(buf, sizeof(buf),
                 "[%s] %s", u->name, msg);
        fprintf(stderr, "[%s] %s", u->name, msg);
    }
    send_to_all(buf);
    // dump_active_users();
}


void disconnect_user(int idx) {
    User *u = active_users[idx];
    if (!u) return;

    broadcast_leave(u);
    free(u);
    u->fd = -1;
    // dump_active_users();
}


int main() {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "socket\n");
        exit(-1);
    }

    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        fprintf(stderr, "setsockopt REUSEADDR\n");
        exit(-1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
        fprintf(stderr, "setsockopt REUSEPORT\n");
        exit(-1);
    }


    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "bind\n");
        exit(-1);
    }

    if (listen(server_fd, 5) < 0) {
        fprintf(stderr, "listen\n");
        exit(-1);
    }
        
    printf("[+] Cheap IRC running on port %d\n", PORT);

    fd_set rfds;
    int maxfd = server_fd;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(server_fd, &rfds);
        maxfd = server_fd;

        for (int i = 0; i < user_count; i++) {
            if (active_users[i] && active_users[i]->fd >= 0) {
                FD_SET(active_users[i]->fd, &rfds);
                if (active_users[i]->fd > maxfd)
                    maxfd = active_users[i]->fd;
            }
        }

        select(maxfd + 1, &rfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &rfds)) {
            int fd = accept(server_fd, NULL, NULL);
            banner(fd);

            char name[32];
            write(fd, "Nickname: ", 10);
            int n = read(fd, name, 31);
            if (n <= 0) { close(fd); continue; }

            name[n] = 0;
            name[strcspn(name, "\n")] = 0;

            int idx = find_user_index(name);

            User *u;
            if (idx == -1) {
                u = malloc(sizeof(User));
                memset(u, 0, sizeof(User));
                strncpy(u->name, name, 31);
                u->fd = fd;
                active_users[user_count++] = u;
            } else {
                u = active_users[idx];
                u->fd = fd;
            }

            broadcast_join(u);
        }

        for (int i = 0; i < user_count; i++) {
            User *u = active_users[i];
            if (!u || u->fd < 0)
                continue;

            if (FD_ISSET(u->fd, &rfds)) {
                char buf[128] = {0};
                int r = read(u->fd, buf, sizeof(buf)-1);
                if (r <= 0 || !strncmp(buf, "/quit", 5)) {
                    disconnect_user(i);
                    close(u->fd);
                    // u->fd = -1;
                    continue;
                }

                if (!strncmp(buf, "/users", 6)) {
                    list_users(u->fd);
                    continue;
                }

                if (!strncmp(buf, "/flag", 5)) {
                    if (u->is_admin) {
                        int fd = open("flag.txt", O_RDONLY);
                        if (fd < 0) {
                            write(u->fd, "Error opening flag\n", 19);
                            continue;
                        }

                        char buf[128];
                        int n;

                        write(u->fd, "FLAG: ", 6);

                        while ((n = read(fd, buf, sizeof(buf))) > 0) {
                            write(u->fd, buf, n);
                        }

                        write(u->fd, "\n", 1);
                        close(fd);                        
                    } else {
                        write(u->fd, "Not authorized.\n", 16);
                    }
                    continue;
                }

                handle_message(u, buf);
            }
        }
    }
}
