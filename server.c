//
// Created by kevinche on 26.05.2021.
//

#include <errno.h>
#include "server.h"

int server_end = 1;
int port_num = 9090;

int listen_inet_socket() {

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;

    if (sock_fd < 0) {
        perror_die("[ERROR]: opening socket");
    } else {
        printf("[INFO]: opened socket");
    }

    // This helps avoid spurious EADDRINUSE when the previous instance of this
    // server died.
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror_die("[ERROR]: set sock opt");
    } else {
        printf("[INFO]: set sock opt");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);

    if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror_die("[ERROR]: on binding");
    }

    if (listen(sock_fd, N_BACKLOG) < 0) {
        perror_die("[ERROR]: on listen");
    }

    return sock_fd;
}

void make_socket_non_blocking(int sock_fd) {

    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags == -1) {
        perror_die("[ERROR]: while set fcntl F_GETFL");
    }

    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror_die("[ERROR]: while set fcntl F_SETFL O_NONBLOCK");
    }
}

void *launch_listener_thread(void * params){
    printf("[INFO]: Serving on port %d\n", port_num);
    int listener_sock_fd = listen_inet_socket();
    make_socket_non_blocking(listener_sock_fd);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror_die("[ERROR]: epoll_create failed");
    }

    struct epoll_event accept_event;
    accept_event.data.fd = listener_sock_fd;
    accept_event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener_sock_fd, &accept_event) < 0) {
        perror_die("[ERROR]: register listener fd - epoll_ctl EPOLL_CTL_ADD");
    }

    struct epoll_event* events = calloc(MAX_FDS, sizeof(struct epoll_event));

    if (events == NULL) {
        perror_die("[ERROR]: Unable to allocate memory for epoll_events");
    }

    while (server_end) {
        int new_events_count = epoll_wait(epoll_fd, events, MAX_FDS, -1);
        for (int i = 0; i < new_events_count; i++) {
            if (events[i].events & EPOLLERR) {
                perror_die("[ERROR]: epoll_wait returned EPOLLERR");
            }

            if (events[i].data.fd == listener_sock_fd) {
                // The listening socket is ready; this means a new peer is connecting.

                struct sockaddr_in peer_addr;
                socklen_t peer_addr_len = sizeof(peer_addr);
                int new_sock_fd = accept(listener_sock_fd, (struct sockaddr*)&peer_addr,
                                         &peer_addr_len);

                if (new_sock_fd < 0) {

                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // This can happen due to the nonblocking socket mode; in this
                        // case don't do anything, but print a notice (since these events
                        // are extremely rare and interesting to observe...)
                        printf("[INFO]: accept returned EAGAIN or EWOULDBLOCK\n");
                    } else {
                        perror_die("[ERROR]: accept connection error");
                    }
                } else {
                    make_socket_non_blocking(new_sock_fd);
                    if (new_sock_fd >= MAX_FDS) {
                        die("[ERROR]: socket fd (%d) >= MAX_FDS (%d)", new_sock_fd, MAX_FDS);
                    }

                    fd_status_t status = on_peer_connected(new_sock_fd, &peer_addr, peer_addr_len);
                    struct epoll_event event = {0};
                    event.data.fd = new_sock_fd;
                    if (status.want_read) {
                        event.events |= EPOLLIN;
                    }
                    if (status.want_write) {
                        event.events |= EPOLLOUT;
                    }

                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_sock_fd, &event) < 0) {
                        perror_die("epoll_ctl EPOLL_CTL_ADD");
                    }
                }
            } else {
                // A peer socket is ready.
                if (events[i].events & EPOLLIN) {
                    // Ready for reading.
                    int fd = events[i].data.fd;
                    fd_status_t status = on_peer_ready_recv(fd);
                    struct epoll_event event = {0};
                    event.data.fd = fd;
                    if (status.want_read) {
                        event.events |= EPOLLIN;
                    }
                    if (status.want_write) {
                        event.events |= EPOLLOUT;
                    }
                    if (event.events == 0) {
                        printf("socket %d closing\n", fd);
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
                            perror_die("epoll_ctl EPOLL_CTL_DEL");
                        }
                        close(fd);
                    } else if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) < 0) {
                        perror_die("epoll_ctl EPOLL_CTL_MOD");
                    }
                } else if (events[i].events & EPOLLOUT) {
                    // Ready for writing.
                    int fd = events[i].data.fd;
                    fd_status_t status = on_peer_ready_send(fd);
                    struct epoll_event event = {0};
                    event.data.fd = fd;

                    if (status.want_read) {
                        event.events |= EPOLLIN;
                    }
                    if (status.want_write) {
                        event.events |= EPOLLOUT;
                    }
                    if (event.events == 0) {
                        printf("socket %d closing\n", fd);
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
                            perror_die("epoll_ctl EPOLL_CTL_DEL");
                        }
                        close(fd);
                    } else if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) < 0) {
                        perror_die("epoll_ctl EPOLL_CTL_MOD");
                    }
                }
            }
        }
    }

}

int server_mode(){
    pthread_t listener;
    pthread_create(&listener, NULL, launch_listener_thread, NULL);
    while (server_end){
        char c = (char)getchar();
        if(c=='Q'){
            server_end = 0;
        }
    }
    return 0;

}