#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>           // close()  
#include <string.h>           // strcpy, memset(), and memcpy()  
  
#include <netdb.h>            // struct addrinfo  
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t  
#include <sys/socket.h>       // needed for socket()  
#include <netinet/in.h>       // IPPROTO_UDP, INET_ADDRSTRLEN  
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)  
#include <netinet/udp.h>      // struct udphdr  
#include <arpa/inet.h>        // inet_pton() and inet_ntop()  
#include <sys/ioctl.h>        // macro ioctl is defined  
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.  
#include <net/if.h>           // struct ifreq  
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD  
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)  
#include <net/ethernet.h>  
  
#include <errno.h>            // errno, perror()  
  
// Define some constants.  
#define ETH_HDRLEN 14  // Ethernet header length  
#define IP4_HDRLEN 20  // IPv4 header length  
#define UDP_HDRLEN  8  // UDP header length, excludes data  
  
#define SEV_PORT 6666  
  
// Function prototypes  
uint16_t checksum (uint16_t *, int);  
uint16_t udp4_checksum (struct ip, struct udphdr, uint8_t *, int);  
char *allocate_strmem (int);  
uint8_t *allocate_ustrmem (int);  
int *allocate_intmem (int);  
  
int  
main (int argc, char **argv)  
{  
int temp_i=0;  
  
for(temp_i=0;temp_i<40;temp_i++)  
{  
  int i, status, datalen, frame_length, sd, bytes, *ip_flags; char *interface, *target, *src_ip, *dst_ip;  
  struct ip iphdr;  
  struct udphdr udphdr;  
  uint8_t *data, *src_mac, *dst_mac, *ether_frame;  
  struct addrinfo hints, *res;  
  struct sockaddr_in *ipv4;  
  struct sockaddr_ll device;  
  struct ifreq ifr;  
  void *tmp;  
  
  // Allocate memory for various arrays.  
  src_mac = allocate_ustrmem (6);  
  dst_mac = allocate_ustrmem (6);  
  data = allocate_ustrmem (IP_MAXPACKET);  
  ether_frame = allocate_ustrmem (IP_MAXPACKET);  
  interface = allocate_strmem (40);  
  target = allocate_strmem (40);  
  src_ip = allocate_strmem (INET_ADDRSTRLEN);  
  dst_ip = allocate_strmem (INET_ADDRSTRLEN);  
  ip_flags = allocate_intmem (4);  
  
  // Interface to send packet through.  
  strcpy (interface, "eth0");  
  
  // Submit request for a socket descriptor to look up interface.  
  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {  
    perror ("socket() failed to get socket descriptor for using ioctl() ");  
    exit (EXIT_FAILURE);  
  }  
  
  // Use ioctl() to look up interface name and get its MAC address.  
  memset (&ifr, 0, sizeof (ifr));  
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);  
  if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {  
    perror ("ioctl() failed to get source MAC address ");  
    return (EXIT_FAILURE);  
  }  
  close (sd);  
  
  // Copy source MAC address.  
  memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));  
  
  // Report source MAC address to stdout.  
  printf ("MAC address for interface %s is ", interface);  
  for (i=0; i<5; i++) {  
    printf ("%02x:", src_mac[i]);  
  }  
  printf ("%02x\n", src_mac[5]);  
  
  // Find interface index from interface name and store index in  
  // struct sockaddr_ll device, which will be used as an argument of sendto().  
  memset (&device, 0, sizeof (device));  
  if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {  
    perror ("if_nametoindex() failed to obtain interface index ");  
    exit (EXIT_FAILURE);  
  }  
  printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);  
  
  // Set destination MAC address: you need to fill these out  
  dst_mac[0] = 0xff;  
  dst_mac[1] = 0xff;  
  dst_mac[2] = 0xff;  
  dst_mac[3] = 0xff;  
  dst_mac[4] = 0xff;  
  dst_mac[5] = 0xff;  
  
  // Source IPv4 address: you need to fill this out  
  strcpy (src_ip, "192.168.3.209");  
  
  // Destination URL or IPv4 address: you need to fill this out  
  strcpy (target, "255.255.255.255");  
  
  // Fill out hints for getaddrinfo().  
  memset (&hints, 0, sizeof (struct addrinfo));  
  hints.ai_family = AF_INET;  
  hints.ai_socktype = SOCK_STREAM;  
  hints.ai_flags = hints.ai_flags | AI_CANONNAME;  
  
  // Resolve target using getaddrinfo().  
  if ((status = getaddrinfo (target, NULL, &hints, &res)) != 0) {  
    fprintf (stderr, "getaddrinfo() failed: %s\n", gai_strerror (status));  
    exit (EXIT_FAILURE);  
  }  
  ipv4 = (struct sockaddr_in *) res->ai_addr;  
  tmp = &(ipv4->sin_addr);  
  if (inet_ntop (AF_INET, tmp, dst_ip, INET_ADDRSTRLEN) == NULL) {  
    status = errno;  
    fprintf (stderr, "inet_ntop() failed.\nError message: %s", strerror (status));  
    exit (EXIT_FAILURE);  
  }  
  freeaddrinfo (res);  
  
  // Fill out sockaddr_ll.  
  device.sll_family = AF_PACKET;  
  memcpy (device.sll_addr, src_mac, 6 * sizeof (uint8_t));  
  device.sll_halen = htons (6);  
  
  // UDP data  
  datalen = 9;  
  data[0] = '1';  
  data[1] = '2';  
  data[2] = '3';  
  data[3] = '4';  
  data[4] = '5';  
  data[5] = '6';  
  data[6] = '7';  
  data[7] = '8';  
  data[8] = '9';  
  
  // IPv4 header  
  
  // IPv4 header length (4 bits): Number of 32-bit words in header = 5  
  iphdr.ip_hl = IP4_HDRLEN / sizeof (uint32_t);  
  
  // Internet Protocol version (4 bits): IPv4  
  iphdr.ip_v = 4;  
  
  // Type of service (8 bits)  
  iphdr.ip_tos = 0;  
  
  // Total length of datagram (16 bits): IP header + UDP header + datalen  
  iphdr.ip_len = htons (IP4_HDRLEN + UDP_HDRLEN + datalen);  
  
  // ID sequence number (16 bits): unused, since single datagram  
  //////iphdr.ip_id = htons (0x2e20+temp_i);  
  iphdr.ip_id = htons(0);   //the ip_id can be constant or variable   
  
  // Flags, and Fragmentation offset (3, 13 bits): 0 since single datagram  
  
  // Zero (1 bit)  
  ip_flags[0] = 0;  
  
  // Do not fragment flag (1 bit)  
  ip_flags[1] = 1;  
  
  // More fragments following flag (1 bit)  
  ip_flags[2] = 0;  
  
  // Fragmentation offset (13 bits)  
  ip_flags[3] = 0;  
  
  iphdr.ip_off = htons ((ip_flags[0] << 15)  
                      + (ip_flags[1] << 14)  
                      + (ip_flags[2] << 13)  
                      +  ip_flags[3]);  
  
  // Time-to-Live (8 bits): default to maximum value  
  iphdr.ip_ttl = 64;  
  
  // Transport layer protocol (8 bits): 17 for UDP  
  iphdr.ip_p = IPPROTO_UDP;  
  
  // Source IPv4 address (32 bits)  
  if ((status = inet_pton (AF_INET, src_ip, &(iphdr.ip_src))) != 1) {  
    fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));  
    exit (EXIT_FAILURE);  
  }  
  
  // Destination IPv4 address (32 bits)  
  if ((status = inet_pton (AF_INET, dst_ip, &(iphdr.ip_dst))) != 1) {  
    fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));  
    exit (EXIT_FAILURE);  
  }  
  
  // IPv4 header checksum (16 bits): set to 0 when calculating checksum  
  iphdr.ip_sum = 0;  
  iphdr.ip_sum = checksum ((uint16_t *) &iphdr, IP4_HDRLEN);  
  
  // UDP header  
  
  // Source port number (16 bits): pick a number  
  udphdr.source = htons (40622);  
  
  // Destination port number (16 bits): pick a number  
  udphdr.dest = htons (SEV_PORT);  
  
  // Length of UDP datagram (16 bits): UDP header + UDP data  
  udphdr.len = htons (UDP_HDRLEN + datalen);  
  
  // UDP checksum (16 bits)  
  udphdr.check = udp4_checksum (iphdr, udphdr, data, datalen);  
  
  // Fill out ethernet frame header.  
  
  // Ethernet frame length = ethernet header (MAC + MAC + ethernet type) + ethernet data (IP header + UDP header + UDP data)  
  frame_length = 6 + 6 + 2 + IP4_HDRLEN + UDP_HDRLEN + datalen;  
  
  // Destination and Source MAC addresses  
  memcpy (ether_frame, dst_mac, 6 * sizeof (uint8_t));  
  memcpy (ether_frame + 6, src_mac, 6 * sizeof (uint8_t));  
  
  // Next is ethernet type code (ETH_P_IP for IPv4).  
  // http://www.iana.org/assignments/ethernet-numbers  
  ether_frame[12] = ETH_P_IP / 256;  
  ether_frame[13] = ETH_P_IP % 256;  
  
  // Next is ethernet frame data (IPv4 header + UDP header + UDP data).  
  
  // IPv4 header  
  memcpy (ether_frame + ETH_HDRLEN, &iphdr, IP4_HDRLEN * sizeof (uint8_t));  
  
  // UDP header  
  memcpy (ether_frame + ETH_HDRLEN + IP4_HDRLEN, &udphdr, UDP_HDRLEN * sizeof (uint8_t));  
  
  // UDP data  
  memcpy (ether_frame + ETH_HDRLEN + IP4_HDRLEN + UDP_HDRLEN, data, datalen * sizeof (uint8_t));  
  
  // Submit request for a raw socket descriptor.  
  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {  
    perror ("socket() failed ");  
    exit (EXIT_FAILURE);  
  }  
  
  
  
      // Send ethernet frame to socket.  
      if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {  
        perror ("sendto() failed");  
        exit (EXIT_FAILURE);  
      }  
  
  // Close socket descriptor.  
  close (sd);  
  
  // Free allocated memory.  
  free (src_mac);  
  free (dst_mac);  
  free (data);  
  free (ether_frame);  
  free (interface);  
  free (target);  
  free (src_ip);  
  free (dst_ip);  
  free (ip_flags);  
  sleep(1);  
  printf("%d\n",temp_i);  
}  
  
  return (EXIT_SUCCESS);  
}  
  
// Checksum function  
uint16_t  
checksum (uint16_t *addr, int len)  
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
  
// Build IPv4 UDP pseudo-header and call checksum function.  
uint16_t  
udp4_checksum (struct ip iphdr, struct udphdr udphdr, uint8_t *payload, int payloadlen)  
{  
  char buf[IP_MAXPACKET];  
  char *ptr;  
  int chksumlen = 0;  
  int i;  
  
  ptr = &buf[0];  // ptr points to beginning of buffer buf  
  
  // Copy source IP address into buf (32 bits)  
  memcpy (ptr, &iphdr.ip_src.s_addr, sizeof (iphdr.ip_src.s_addr));  
  ptr += sizeof (iphdr.ip_src.s_addr);  
  chksumlen += sizeof (iphdr.ip_src.s_addr);  
  
  // Copy destination IP address into buf (32 bits)  
  memcpy (ptr, &iphdr.ip_dst.s_addr, sizeof (iphdr.ip_dst.s_addr));  
  ptr += sizeof (iphdr.ip_dst.s_addr);  
  chksumlen += sizeof (iphdr.ip_dst.s_addr);  
  
  // Copy zero field to buf (8 bits)  
  *ptr = 0; ptr++;  
  chksumlen += 1;  
  
  // Copy transport layer protocol to buf (8 bits)  
  memcpy (ptr, &iphdr.ip_p, sizeof (iphdr.ip_p));  
  ptr += sizeof (iphdr.ip_p);  
  chksumlen += sizeof (iphdr.ip_p);  
  
  // Copy UDP length to buf (16 bits)  
  memcpy (ptr, &udphdr.len, sizeof (udphdr.len));  
  ptr += sizeof (udphdr.len);  
  chksumlen += sizeof (udphdr.len);  
  
  // Copy UDP source port to buf (16 bits)  
  memcpy (ptr, &udphdr.source, sizeof (udphdr.source));  
  ptr += sizeof (udphdr.source);  
  chksumlen += sizeof (udphdr.source);  
  
  // Copy UDP destination port to buf (16 bits)  
  memcpy (ptr, &udphdr.dest, sizeof (udphdr.dest));  
  ptr += sizeof (udphdr.dest);  
  chksumlen += sizeof (udphdr.dest);  
  
  // Copy UDP length again to buf (16 bits)  
  memcpy (ptr, &udphdr.len, sizeof (udphdr.len));  
  ptr += sizeof (udphdr.len);  
  chksumlen += sizeof (udphdr.len);  
  
  // Copy UDP checksum to buf (16 bits)  
  // Zero, since we don't know it yet  
  *ptr = 0; ptr++;  
  *ptr = 0; ptr++;  
  chksumlen += 2;  
  
  // Copy payload to buf  
  memcpy (ptr, payload, payloadlen);  
  ptr += payloadlen;  
  chksumlen += payloadlen;  
  
  // Pad to the next 16-bit boundary  
  for (i=0; i<payloadlen%2; i++, ptr++) {  
    *ptr = 0;  
    ptr++;  
    chksumlen++;  
  }  
  
  return checksum ((uint16_t *) buf, chksumlen);  
}  
  
// Allocate memory for an array of chars.  
char *  
allocate_strmem (int len)  
{  
  void *tmp;  
  
  if (len <= 0) {  
    fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);  
    exit (EXIT_FAILURE);  
  }  
  
  tmp = (char *) malloc (len * sizeof (char));  
  if (tmp != NULL) {  
    memset (tmp, 0, len * sizeof (char));  
    return (tmp);  
  } else {  
    fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_strmem().\n");  
    exit (EXIT_FAILURE);  
  }  
}  
  
// Allocate memory for an array of unsigned chars.  
uint8_t *  
allocate_ustrmem (int len)  
{  
  void *tmp;  
  
  if (len <= 0) {  
    fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", len);  
    exit (EXIT_FAILURE);  
  }  
  
  tmp = (uint8_t *) malloc (len * sizeof (uint8_t));  
  if (tmp != NULL) {  
    memset (tmp, 0, len * sizeof (uint8_t));  
    return (tmp);  
  } else {  
    fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_ustrmem().\n");  
    exit (EXIT_FAILURE);  
  }  
}  
  
// Allocate memory for an array of ints.  
int *  
allocate_intmem (int len)  
{  
  void *tmp;  
  
  if (len <= 0) {  
    fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_intmem().\n", len);  
    exit (EXIT_FAILURE);  
  }  
  
  tmp = (int *) malloc (len * sizeof (int));  
  if (tmp != NULL) {  
    memset (tmp, 0, len * sizeof (int));  
    return (tmp);  
  } else {  
    fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_intmem().\n");  
    exit (EXIT_FAILURE);  
  }  
}  