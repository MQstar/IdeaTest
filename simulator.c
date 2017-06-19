#include <stdio.h>
#include <stdlib.h>
#include <netinet/if_ether.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_UDP, INET_ADDRSTRLEN
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <errno.h>

int8_t* parseMac(char mac[],char result[])
{
    uint8_t a,b,c,d,e,f;
    sscanf(mac,"%x:%x:%x:%x:%x:%x",&a,&b,&c,&d,&e,&f);
    uint8_t tmp[6]={a,b,c,d,e,f};
    int i=0;
    for(i=0;i<6;i++)
    {
        result[i]=tmp[i];
    }
    result=tmp;
    return result; 
}
int main()
{
    FILE * file = fopen("br.pcap", "r+");  
    if (file == NULL)  
    {  
        return -1;
    }
    int sd,bytes;
    uint8_t buf[1500];
    uint32_t* size;
    fseek(file,24,SEEK_CUR);
    uint8_t srcmac[6];
    while(1)
    {
    if(fseek(file,12,SEEK_CUR)<0)
    break;
    printf("start read\n");
    if(fread(size,1,4,file)<0)
    break;
    if(fread(buf,(int)*size,1,file)<0)
    break;
    struct sockaddr_ll device;
        memset (&device, 0, sizeof (device));
        if ((device.sll_ifindex = if_nametoindex ("eno1")) == 0)
        {
            perror ("if_nametoindex() failed to obtain interface index ");
            exit (EXIT_FAILURE);
        } 
        // Fill out sockaddr_ll.
        device.sll_family = AF_PACKET;
        memcpy (device.sll_addr, parseMac("ec:b1:d7:84:2c:14",srcmac), 6 * sizeof (uint8_t));
        device.sll_halen = htons (6);    
if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) 
        {
            exit (EXIT_FAILURE);
        }
        // Send ethernet frame to socket.
        uint8_t* ether_frame=buf;
        int length = ((*(buf+16)<<8)+*(buf+17));
        printf("length is %d\n",(length));
        if ((bytes = sendto (sd, ether_frame, (int)*size, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
        {
            exit (EXIT_FAILURE);
        }
        close(sd);
    }
}
