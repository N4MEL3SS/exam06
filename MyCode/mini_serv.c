#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

const int BUF_SIZE = 4096 * 4;

int clients[1024];
int max_fd = 0, next_id = -1, sockfd;
char buf_read[BUF_SIZE], buf_write[BUF_SIZE], msg[BUF_SIZE];
fd_set ready_read, ready_write, active;

void f_error(char *str)
{
	write(2, str, strlen(str));
	exit(1);
}

void send_all(int self)
{
	for (int i = sockfd; i <= max_fd; ++i)
	{
		if (FD_ISSET(i, &ready_write) && i != self)
			send(i, buf_write, strlen(buf_write), 0);
	}
}

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	int len, res, connfd;

	if (argc != 2)
		f_error("Wrong number of arguments\n");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		f_error("Fatal error\n");

	max_fd = sockfd;
	FD_ZERO(&active);
	FD_SET(sockfd, &active);
	bzero(&clients, sizeof(clients));

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		f_error("Fatal error\n");
	if (listen(sockfd, 128) < 0)
		f_error("Fatal error\n");

	while (1)
	{
		ready_read = ready_write = active;
		if (select(max_fd + 1, &ready_read, &ready_write, NULL, NULL) < 0)
			continue;

		for (int fd_i = sockfd; fd_i <= max_fd; ++fd_i)
		{
			if (FD_ISSET(fd_i, &ready_read) && fd_i == sockfd)
			{
				if ((connfd = accept(sockfd, (struct sockaddr*)&servaddr, (socklen_t*)&len)) < 0)
					continue;

				if (connfd > max_fd)
					max_fd = connfd;
				clients[connfd] = ++next_id;
				FD_SET(connfd, &active);
				sprintf(buf_write, "server: client %d just arrived\n", clients[connfd]);
				send_all( connfd);
				break;
			}
			else if (FD_ISSET(fd_i, &ready_read) && fd_i != sockfd)
			{
				if ((res = recv(fd_i, buf_read, BUF_SIZE, 0)) <= 0)
				{
					sprintf(buf_write, "server: client %d just left\n", clients[connfd]);
					send_all(fd_i);
					FD_CLR(fd_i, &active);
					close(fd_i);
					break;
				}
				else
				{
//					int j = 0;
//					for (int i = 0; i < res; ++i)
//					{
//						if (buf_read[i] == '\n')
//						{
//							strncpy(msg, &buf_read[j], i + 1 - j);
//							msg[strlen(msg) - 1] = '\0';
//							j = i;
//							sprintf(buf_write, "client %d: %s\n", clients[connfd], msg);
//							send_all(fd_i);
//							bzero(&msg, strlen(msg));
//						}
//					}
					for (int i = 0, j = strlen(msg); i < res; ++i, ++j)
					{
						msg[j] = buf_read[i];
						if (msg[j] == '\n')
						{
							msg[j] = '\0';
							sprintf(buf_write, "client %d: %s\n", clients[fd_i], msg);
							send_all(fd_i);
							bzero(&msg, strlen(msg));
							j = -1;
						}
					}
					break;
				}
			}
		}
	}
}
