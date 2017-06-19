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
#include <errno.h>            // errno, perror()
#define ETHER_LEN 14
#define IP4_LEN 20
#define UDP_LEN 8
#define GTP_LEN 8
#define MAX_BUF_LEN 1024
#define MAX_KEY_LEN 64
#define MAX_VAL_LEN 256
#define chartonumber(x)(x-'0')
typedef struct EtherHeader_t
{
uint8_t DstMAC[6];
uint8_t SrcMAC[6];
uint16_t EtherType;
} EtherHeader_t;

typedef struct IPHeader_t
{
uint8_t Ver_IHLen;
uint8_t TOS;
uint16_t TotalLen;
uint16_t ID;
uint16_t Flag_Segment;
uint8_t TTL;
uint8_t Protocol;
uint16_t Checksum;
uint32_t SrcIP;
uint32_t DstIP;
} IPHeader_t;

typedef struct UDPHeader_t
{
uint16_t SrcPort;
uint16_t DstPort;
uint16_t Len;
uint16_t Checksum;
} UDPHeader_t;

char* config[8];
int Trim(char s[])  
{  
    int n;  
    for(n = strlen(s) - 1; n >= 0; n--)  
    {  
        if(s[n]!=' ' && s[n]!='\t' && s[n]!='\n')  
            break;  
        s[n+1] = '\0';  
    }  
    return n;  
}  
  
int parseInt(char str[], int length)
{
    int number=0;
    int i=0;
    for(i=0;i<(length-1);i++)
    {
        int position=1;
        int a=0;
        for(a=0;a<i;a++)
        {
            position*=10;
        }
        number+=chartonumber(str[i])*position;
    }
    return number;
}

uint8_t* parseMac(char mac[],char result[])
{
    uint8_t add[6];
    sscanf(mac,"%2x:%2x:%2x:%2x:%2x:%2x",&add[0],&add[1],&add[2],&add[3],&add[4],&add[5]);
    int i=0;
    for(i=0;i<6;i++)
    {
        result[i]=add[i];
    }
    return result; 
}

int loadConfigDemo(const char* config_path)  
{  
    FILE * file = fopen(config_path, "r");  
    if (file == NULL)  
    {  
        printf("[Error]open %s failed.\n", config_path);
        return -1;
    }
    int c=0;
    for(c=0;c<8;c++)
    config[c]=malloc(MAX_VAL_LEN);
    char buf[MAX_BUF_LEN];  
    int text_comment = 0;  
    while(fgets(buf, MAX_BUF_LEN, file) != NULL)  
    {  
        Trim(buf);  
        // to skip text comment with flags /* ... */  
        if (buf[0] != '#' && (buf[0] != '/' || buf[1] != '/'))  
        {  
            if (strstr(buf, "/*") != NULL)  
            {  
                text_comment = 1;  
                continue;  
            }  
            else if (strstr(buf, "*/") != NULL)  
            {  
                text_comment = 0;  
                continue;  
            }  
        }  
        if (text_comment == 1)  
        {  
            continue;  
        }  
        int buf_len = strlen(buf);  
        // ignore and skip the line with first chracter '#', '=' or '/'  
        if (buf_len <= 1 || buf[0] == '#' || buf[0] == '=' || buf[0] == '/')  
        {  
            continue;  
        }  
        buf[buf_len-1] = '\0';  
        char _paramk[MAX_KEY_LEN] = {0}, _paramv[MAX_VAL_LEN] = {0};  
        int _kv=0, _klen=0, _vlen=0;  
        int i = 0;  
        for (i=0; i<buf_len; ++i)  
        {  
            if (buf[i] == ' ')  
                continue;  
            // scan param key name  
            if (_kv == 0 && buf[i] != '=')  
            {  
                if (_klen >= MAX_KEY_LEN)  
                    break;  
                _paramk[_klen++] = buf[i];  
                continue;  
            }  
            else if (buf[i] == '=')  
            {  
                _kv = 1;  
                continue;  
            }  
            // scan param key value  
            if (_vlen >= MAX_VAL_LEN || buf[i] == '#')  
                break;  
            _paramv[_vlen++] = buf[i];  
        }  
        if (strcmp(_paramk, "")==0 || strcmp(_paramv, "")==0)  
            continue;  
        if (strcmp(_paramk, "ports")==0)
        {
            strcpy(config[0],_paramv);
            //int port=parseInt(_paramv, _vlen);
        }
        if (strcmp(_paramk, "newport")==0)
        {
            strcpy(config[1],_paramv);
            //int newport=parseInt(_paramv, _vlen);
        }
        if (strcmp(_paramk, "olddest")==0)
        {
            strcpy(config[2],_paramv);
        }
        if (strcmp(_paramk, "newsrc")==0)
        {
            strcpy(config[3],_paramv);
        }
        if (strcmp(_paramk, "newdest")==0)
        {
            strcpy(config[4],_paramv);
        }
        if (strcmp(_paramk, "newsrcmac")==0)
        {
            strcpy(config[5],_paramv);
        }
        if (strcmp(_paramk, "newdstmac")==0)
        {
            strcpy(config[6],_paramv);
        }
        if (strcmp(_paramk, "iface")==0)
        {
            strcpy(config[7],_paramv);
        }
    }  
    return 0;  
}

int EtherFilter(EtherHeader_t *ether,uint8_t *mac)
{
    uint8_t* p = (uint8_t*)&ether->DstMAC;
    p = (uint8_t*)&ether->SrcMAC; 
    int i=0;
    for(i=0;i<6;i++)
        if(mac[i]!=p[i]) return 1;
    return 0;    
}

int IPFilter(IPHeader_t *ip)
{
    uint8_t* p = (uint8_t*)&ip->SrcIP;
    //printf("%u\n",p[1]);
    //printf("%u\n",(inet_addr(config[2])>>8)&0xff);
    char str[16]="";
    sprintf(str,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);
    if(strcmp(config[2],str)==0)
    return 0;
    else
    return 1;
}

uint16_t checksum (uint16_t *addr, int len)
{
  int nleft = len;
  int sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= sizeof (uint16_t);
  }

  if (nleft == 1) {
    *(uint8_t *) (&answer) = *(uint8_t *) w;
    sum += answer;
  }
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

uint16_t analyseIP(IPHeader_t *ip)
{
    ip->SrcIP=inet_addr(config[3]);
    ip->DstIP=inet_addr(config[4]);
    ip->Checksum=0;
    ip->Checksum=checksum((uint16_t *)&ip,IP4_LEN);//TODO
    int length = ((*((uint8_t*)ip+2)<<8)+*((uint8_t*)ip+3));
    return length;
}



int main()  
{  
    loadConfigDemo("./setting.conf");
    struct ifreq m_ifreq;
    int sd,frame_length,bytes,sock = 0;
    sock = socket(AF_INET,SOCK_STREAM,0);
    strcpy(m_ifreq.ifr_name,config[7]);
    ioctl(sock,SIOCGIFHWADDR,&m_ifreq);
    uint8_t* mac=(uint8_t*)m_ifreq.ifr_hwaddr.sa_data;
    EtherHeader_t *ether;
    char buf[12000];//1500 MTU
    uint8_t srcmac[6], dstmac[6];
    ssize_t n;
    /* capture ip datagram without ethernet header */
    if ((sock = socket(PF_PACKET,  SOCK_RAW, htons(ETH_P_ALL)))== -1)
    {    
        printf("socket error!\n");
        return 1;
    }
    while (1)
    {
        n = recv(sock, buf, sizeof(buf), 0);
        if (n == -1)
        {
            printf("recv error!\n");
            break;
        }
        else if (n==0)
            continue;
        ether = (EtherHeader_t *)(buf);
       // if(EtherFilter(ether,mac)==1)
       //     continue;
        IPHeader_t *ip = ( IPHeader_t *)(buf + ETHER_LEN);
        if(IPFilter(ip)==1)
            continue;
        //pass 14(ETHER_LEN)+20(IP_LEN)+8(UDP_LEN)+8(GTP_LEN)
        size_t iplen =  (ip->Ver_IHLen&0x0f)*4;
        IPHeader_t *payload = (IPHeader_t *)(buf + ETHER_LEN + iplen + UDP_LEN + GTP_LEN);
        //generate ether header
        int datalen=analyseIP(payload);//modify ip in header and get length
        printf("length %d\n",datalen);
        if (datalen>1500)
            continue;
        // Find interface index from interface name and store index in
        // struct sockaddr_ll device, which will be used as an argument of sendto().
        struct sockaddr_ll device;
        memset (&device, 0, sizeof (device));
        if ((device.sll_ifindex = if_nametoindex (config[7])) == 0)
        {
            perror ("if_nametoindex() failed to obtain interface index ");
            exit (EXIT_FAILURE);
        } 
        // Fill out sockaddr_ll.
        device.sll_family = AF_PACKET;
        memcpy (device.sll_addr, parseMac(config[5],srcmac), 6 * sizeof (uint8_t));
        device.sll_halen = htons (6);
        uint8_t *ether_frame;
        ether_frame=(uint8_t *) malloc (1500 * sizeof (uint8_t));
        frame_length = 6 + 6 + 2 + datalen;
        memcpy (ether_frame, parseMac(config[6],dstmac), 6 * sizeof (uint8_t));
        //printf("new dstmac %x:%x:%x:%x:%x:%x\n",dstmac[0],dstmac[1],dstmac[2],dstmac[3],dstmac[4],dstmac[5]);
        memcpy (ether_frame + 6, parseMac(config[5],srcmac), 6 * sizeof (uint8_t));
        //printf("new srcmac %x:%x:%x:%x:%x:%x\n",srcmac[0],srcmac[1],srcmac[2],srcmac[3],srcmac[4],srcmac[5]);
        ether_frame[12] = ETH_P_IP / 256;
        ether_frame[13] = ETH_P_IP % 256;
        //!!!POINT
        memcpy(ether_frame+ETHER_LEN,payload,datalen * sizeof (uint8_t));


//        int n=0;
//        for(n=0;n<frame_length;n++)
//        {
//            printf("%02x ",ether_frame[n]);
//        }




        if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) 
        {
            perror ("socket() failed ");
            exit (EXIT_FAILURE);
        }
        // Send ethernet frame to socket.
        if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
        {
            perror ("sendto() failed");
            exit (EXIT_FAILURE);
        }
        close(sd);
        free (ether_frame);
        printf("done\n");
    }
    return 0;
}
