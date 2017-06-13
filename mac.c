#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#define FRAME_LEN 14
#define MAX_BUF_LEN 1024
#define MAX_KEY_LEN 64
#define MAX_VAL_LEN 256
#define chartonumber(x)(x-'0')
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

int analyseFrame(FrameHeader_t *frame,u_int8 *mac)
{
    u_int8* p = (u_int8*)&frame->DstMAC;
    p = (u_int8*)&frame->SrcMAC; 
    int i=0;
    for(i=0;i<6;i++)
        if(mac[i]!=p[i]) return 1;
    return 0;    
}

int analyseIP(IPHeader_t *ip)
{
    u_int8* p = (u_int8*)&ip->SrcIP;
    char str[16]="";
    sprintf(str,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);
    if(strcmp(config[2],str)==0)
    return 0;
    else
    return 1;
}

void analyseUDP(UDPHeader_t *udp)
{
    printf("UDP -----\n");
    printf("Source port: %u\n", ntohs(udp->SrcPort));
    printf("Dest port: %u\n", ntohs(udp->DstPort));
}
int main()  
{  
    loadConfigDemo("./setting.conf");
    struct ifreq m_ifreq;
    int sock = 0;
    sock = socket(AF_INET,SOCK_STREAM,0);
    strcpy(m_ifreq.ifr_name,config[7]);
    ioctl(sock,SIOCGIFHWADDR,&m_ifreq);
    u_int8* mac=(u_int8*)m_ifreq.ifr_hwaddr.sa_data;
    FrameHeader_t *frame;
    char buf[12000];//1500 MTU
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
        frame = (FrameHeader_t *)(buf);
        if(analyseFrame(frame,mac)==1)
            continue;
        IPHeader_t *ip = ( IPHeader_t *)(buf + FRAME_LEN);
        if(analyseIP(ip)==1)
            continue;
        size_t iplen =  (ip->Ver_IHLen&0x0f)*4;
        if (ip->Protocol == IPPROTO_UDP)
        {
            UDPHeader_t *udp = (UDPHeader_t *)(buf + FRAME_LEN + iplen);
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
    close(sock);
    return 0;
}
