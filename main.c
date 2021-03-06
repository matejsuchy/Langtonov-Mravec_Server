#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 11124

static const int MAX_CONNECTIONS = 5;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }
    printf("Server sa spustil a pocuva na porte %d \n", PORT);
    while (1) {

        listen(sockfd, MAX_CONNECTIONS);
        cli_len = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return 3;
        }

        int velkostX;
        int velkostY;
        int status = -1;
        int dotazPred;
        n = read(newsockfd, &dotazPred, sizeof(dotazPred));
        int dotaz = ntohl(dotazPred);
        char buffer[256];
        bzero(buffer, 256);

        if (dotaz) {
            //nacitaj zo servera
            read(newsockfd, buffer, 255);
            printf("nacitaj zo servera subor %s \n", buffer);
            FILE *file = fopen(buffer, "r");
            if (file == 0) {
                //error
                int convertedStatus = htonl(status);
                n = write(newsockfd, &convertedStatus, sizeof(convertedStatus));
            } else {
                status = 0;
                int convertedStatus = htonl(status);
                n = write(newsockfd, &convertedStatus, sizeof(convertedStatus));

                fscanf(file, "%d ", &velkostX);
                fscanf(file, "%d ", &velkostY);
                int pole[velkostX * velkostY];

                for (int i = 0; i < velkostX * velkostY; ++i) {
                    int farba;
                    fscanf(file, "%d ", &farba);
                    pole[i] = farba;
                }
                fclose(file);

                int converted_numberA = htonl(velkostX);
                int converted_numberB = htonl(velkostY);

                write(newsockfd, &converted_numberA, sizeof(converted_numberA));
                write(newsockfd, &converted_numberB, sizeof(converted_numberB));
                for (int i = 0; i < velkostX * velkostY; ++i) {

                    int converted_numberPole = htonl(pole[i]);
                    write(newsockfd, &converted_numberPole, sizeof(converted_numberPole));
                }
                printf("Koniec nacitania\n");
            }

        } else {
            //uloz na server
            int predX;
            int predY;

            n = read(newsockfd, &predX, sizeof(predX));
            n = read(newsockfd, &predY, sizeof(predY));

            velkostX = ntohl(predX);
            velkostY = ntohl(predY);
            printf("X: %d\n",velkostX);
            printf("Y: %d\n",velkostY);
            int pole[velkostX * velkostY];

            for (int i = 0; i < velkostX * velkostY; ++i) {
                int farba;
                n = read(newsockfd, &farba, sizeof(farba));
                pole[i] = ntohl(farba);
            }
            n = read(newsockfd, buffer, 255);
            printf("uloz na server subor %s\n", buffer);
            FILE *file = fopen(buffer, "w");

            fprintf(file, "%d \n%d \n", velkostX, velkostY);
            fclose(file);
            FILE *filea = fopen(buffer, "a");
            for (int i = 0; i < velkostX * velkostY; ++i) {
                fprintf(filea, "%d \n", pole[i]);
            }
            fclose(filea);
            printf("Koniec ukladania\n");
            status = 0;
        }
    }
    close(newsockfd);
    close(sockfd);
}
