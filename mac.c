#include   <stdio.h>
#include   <sys/ioctl.h>
#include   <sys/socket.h>
#include   <netinet/in.h>
#include   <net/if.h>
#include   <string.h>
char *get_local_mac(char *if_name){
    struct ifreq m_ifreq;
    int sock = 0;
    char mac[32] = " ";

    sock = socket(AF_INET,SOCK_STREAM,0);
    strcpy(m_ifreq.ifr_name,if_name);

    ioctl(sock,SIOCGIFHWADDR,&m_ifreq);
    int i = 0;
    for(i = 0; i < 6; i++){
        sprintf(mac+3*i, "%02X:", (unsigned char) m_ifreq.ifr_hwaddr.sa_data[i]);
    }
    mac[strlen(mac) - 1] = 0;
    printf("%s\n", mac);

    return NULL;
}

int main(){
    get_local_mac("enp0s3");
    return 0;
}
