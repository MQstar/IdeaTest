#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#define FrameLen 14
typedef unsigned int u_int32;
typedef unsigned short u_int16;
typedef unsigned char u_int8;

typedef struct FrameHeader_t
{
u_int8 DstMAC[6];
u_int8 SrcMAC[6];
u_int16 FrameType;
} FrameHeader_t;

typedef struct IPHeader_t
{
u_int8 Ver_IHLen;
u_int8 TOS;
u_int16 TotalLen;
u_int16 ID;
u_int16 Flag_Segment;
u_int8 TTL;
u_int8 Protocol;
u_int16 Checksum;
u_int32 SrcIP;
u_int32 DstIP;
} IPHeader_t;

typedef struct UDPHeader_t
{
u_int16 SrcPort;
u_int16 DstPort;
u_int16 Len;
u_int16 Checksum;
} UDPHeader_t;

void analyseFrame(FrameHeader_t *frame);
void analyseIP(IPHeader_t *ip);
void analyseUDP(UDPHeader_t *udp);


int main(void)
{
    int sockfd;
    FrameHeader_t *frame;
    char buf[10240];//10kb
    ssize_t n;
    /* capture ip datagram without ethernet header */
    if ((sockfd = socket(PF_PACKET,  SOCK_RAW, htons(ETH_P_ALL)))== -1)
    {    
        printf("socket error!\n");
        return 1;
    }
    while (1)
    {
        n = recv(sockfd, buf, sizeof(buf), 0);
        if (n == -1)
        {
            printf("recv error!\n");
            break;
        }
        else if (n==0)
            continue;
        frame = (FrameHeader_t *)(buf);
        analyseFrame(frame);
        IPHeader_t *ip = ( IPHeader_t *)(buf + FrameLen);
        analyseIP(ip);
        size_t iplen =  (ip->Ver_IHLen&0x0f)*4;
        if (ip->Protocol == IPPROTO_UDP)
        {
            UDPHeader_t *udp = (UDPHeader_t *)(buf + FrameLen + iplen);
            analyseUDP(udp);
        }
        else if (ip->Protocol == IPPROTO_TCP)
        {
            printf("TCP----\n");
        }
        else if (ip->Protocol == IPPROTO_ICMP)
        {
            printf("ICMP----\n");
        }
        else if (ip->Protocol == IPPROTO_IGMP)
        {
            printf("IGMP----\n");
        }
        else
        {
            printf("other protocol!\n");
        }        
        printf("\n\n");
    }
    close(sockfd);
    return 0;
}

void analyseFrame(FrameHeader_t *frame)
{
    u_int8* p = (unsigned char*)&frame->DstMAC;
    printf("Destination Mac\t: %x:%x:%x:%x:%x:%x\n",p[0],p[1],p[2],p[3],p[4],p[5]);
    p = (unsigned char*)&frame->SrcMAC;
    printf("Source MAC\t: %x:%x:%x:%x:%x:%x\n",p[0],p[1],p[2],p[3],p[4],p[5]);

}

void analyseIP(IPHeader_t *ip)
{
    u_int8* p = (unsigned char*)&ip->SrcIP;
    printf("Source IP\t: %u.%u.%u.%u\n",p[0],p[1],p[2],p[3]);
    p = (unsigned char*)&ip->DstIP;
    printf("Destination IP\t: %u.%u.%u.%u\n",p[0],p[1],p[2],p[3]);

}

void analyseUDP(UDPHeader_t *udp)
{
    printf("UDP -----\n");
    printf("Source port: %u\n", ntohs(udp->SrcPort));
    printf("Dest port: %u\n", ntohs(udp->DstPort));
}
