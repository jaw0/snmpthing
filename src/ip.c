

#ifndef TESTING
#pragma optimize(0)
#pragma registers(4)
#pragma rc
#include <mmlib.h>
#else
#include <aliases.h>
#endif

#include <ip.h>
#include <netstat.h>
#include <eeprom.h>
#include <error.h>

#define UDPOF(p)	((struct udphdr*)((char*)p + sizeof(struct ip)))
#define UDPDATAOF(p)	((char*)p + sizeof(struct ip) + sizeof(struct udphdr))


u_short ipid;           /* ip id */
struct Netstat netstat;

extern void snmp_input(u_char *, int);


void initNet(void){

	bzero((char*)&netstat, sizeof(netstat));
	ipid = 0;

}

/* these will work on both big and little indians! */
u_short hosttonetshort( u_short x ){
	u_short y;

	*((u_char*)&y) = x>>8;
	*((u_char*)&y + 1) = x & 0xFF;

	return y;
}

u_short nettohostshort( u_short x ){
	u_short y;

	*((u_char*)&y) = x>>8;
	*((u_char*)&y + 1) = x & 0xFF;

	return y;
}

/* from ping.c */
u_short in_cksum(u_short *addr, int len){
	int nleft = len;
        u_short *w = addr;
        long sum = 0;
        u_short answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
         * sequential 16 bit words to it, and at the end, fold back all the
         * carry bits from the top 16 bits into the lower 16 bits.
         */
        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

	/* mop up an odd byte, if necessary */
        if (nleft == 1) {
                /* *(u_char *)(&answer) = *(u_char *)w ; */
		answer = *(u_char *)w;
		answer <<= 8;
                sum += answer;
        }

	/* add back carry outs from top 16 bits to low 16 bits */
        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
        return(answer);

}

extern int serial_putchar(FILE *, char);

void slip_output(u_char *pkt, int l){
	int i;
	
#if 0
	/* dump packet to serial port */
	for(i=0; i< l; i++){
		if(!(i%16)){
			snprintf(buf, sizeof(buf), "\n%04.4lx :", i);
			puts(buf);
		}
		snprintf(buf, sizeof(buf), " %02.2x", pkt[i] );
		puts(buf);
	}
	puts("\n");
#endif

	serial_putchar(serial_port, SLIP_END);
	for(i=0; i<l; i++){
		switch(pkt[i]){
		  case SLIP_ESC:
			serial_putchar(serial_port, SLIP_ESC);
			serial_putchar(serial_port, SLIP_ESC_ESC);
			break;
		  case SLIP_END:
			serial_putchar(serial_port, SLIP_ESC);
			serial_putchar(serial_port, SLIP_ESC_END);
			break;
		  default:
			serial_putchar(serial_port, pkt[i]);
			break;
		}
	}
	serial_putchar(serial_port, SLIP_END);
}

void ip_output(u_char *pkt, int l){
	struct ip *ip;

	/* fill in ip header (dst should already be filled) */
	
	ip = (struct ip*)pkt;
	
	ip->ip_hlv = 0x45;
	ip->ip_tos = 0;
	ip->ip_id  = hosttonetshort(ipid++);
	ip->ip_off = 0;
	ip->ip_ttl = INET_DEFAULT_TTL;
	ip->ip_src = eeprom.myipaddr;
	ip->ip_len = hosttonetshort(l);

	ip->ip_sum = 0;
	ip->ip_sum = in_cksum((u_short*)pkt, sizeof(struct ip));

	netstat.outpackets ++;
	
	slip_output(pkt, l);
}

void syslog(int code, const char *message){
	u_char packet[1024];
	struct ip *ip;
	struct udphdr *udp;
	char *p;
	int i, l, ml;
	
	ip  = (struct ip*)packet;
	udp = UDPOF(packet);
	p   = UDPDATAOF(packet);

	l = sizeof(packet)-sizeof(struct ip)-sizeof(struct udphdr);
	i = snprintf(p, l, "<%ld>%s", code, message);

	udp->uh_len = hosttonetshort(i + sizeof(struct udphdr));
	udp->uh_sport = 0x1234;
	udp->uh_dport = hosttonetshort(UDPPORT_SYSLOG);
	udp->uh_sum = 0;
	
	ip->ip_p   = IPPROTO_UDP;
	ip->ip_dst = eeprom.sysloghost;

	l = i + sizeof(struct ip) + sizeof(struct udphdr);
	ip_output(packet, l);
	
}

void udp_input(const u_char *pkt, int len){
	struct udphdr *udp;
	int port;

	udp = UDPOF(pkt);
	
	port = nettohostshort(udp->uh_dport);

	switch (port){
	  case UDPPORT_SNMP:
		netstat.udp.snmp.total ++;
		snmp_input((u_char*)pkt, len);
		break;
	  default:
		netstat.udp.bad_port ++;
		ERROR1(LOG_INET, LOG_ERR, "UDP %ld NYI\n", port);
		break;
		/* error */
	}
}

void icmp_input(const u_char *pkt, int len){
	struct icmp *ic;
	struct ip *ip;
	u_short sum;
	int type;
	int il, ih;
	
	ip = (struct ip*)pkt;
	ih = (ip->ip_hlv & 0xF) << 2;
	
	ic = (struct icmp*)(pkt + ih);
	il = nettohostshort(ip->ip_len) - ih;
	
	type = ic->icmp_type;
	switch(type){

	  case ICMP_ECHO:
		netstat.icmp.echo ++;
		ip->ip_dst = ip->ip_src;
		ic->icmp_type = ICMP_ECHOREPLY;
		sum = ic->icmp_cksum;
		ic->icmp_cksum = 0;
		ic->icmp_cksum = in_cksum((u_short*)ic, il);
		ip_output((u_char*)pkt, len);

		/* DEBUG1("ping from %I", ip->ip_dst); */
		break;
	  default:
		netstat.icmp.bad_type ++;
		ERROR1(LOG_INET, LOG_ERR, "ICMP %ld NYI", type);
		break;
	}

}

void tcp_input(const u_char *pkt, int len){

	netstat.tcp.bad_port ++;
	ERROR(LOG_INET, LOG_ERR, "TCP NYI\n");
}


void ip_input(const u_char *pkt, int len){
	u_short cksum;
	int hl, proto, plen;
	struct ip *ip;
	u_long srcip;

	netstat.inpackets ++;
	
	if( len < 20 ){
		netstat.bad_input ++;
		ERROR1(LOG_INET, LOG_ERR, "tiny read: %ld", len);
		return;
	}

	ip = (struct ip*)pkt;
	srcip = ip->ip_src;
	
	hl = ip->ip_hlv & 0xF;
	if( hl < 5 ){
		netstat.bad_input ++;
		ERROR2(LOG_INET, LOG_ERR, "short head from %I %ld:", srcip, hl);
		return;
	}
	
	cksum = in_cksum((u_short*)pkt, hl<<2);
	if( cksum ){
		netstat.bad_cksum ++;
		ERROR2(LOG_INET, LOG_ERR, "bad cksum from %I %lx", srcip, cksum);
		return;
	}

	plen = nettohostshort( ip->ip_len );
	if( len < plen ){
		netstat.bad_input ++;
		ERROR3(LOG_INET, LOG_ERR, "short read from %I %ld < %ld", srcip, len, plen);
		return;
	}
	
	proto = ip->ip_p;
	switch (proto){

	  case IPPROTO_UDP:
		netstat.udp.total ++;
		udp_input(pkt, plen);
		break;
	  case IPPROTO_ICMP:
		netstat.icmp.total ++;
		icmp_input(pkt, plen);
		break;
	  case IPPROTO_TCP:
		netstat.tcp.total ++;
		tcp_input(pkt, plen);
		break;
	  default:
		/* error */
		ERROR2(LOG_INET, LOG_ERR, "unknown proto: %ld from %I", proto, srcip);
		netstat.bad_proto ++;
	}
}

void sliploop(void){
	char buf[64];
	int i;

	while(1){

		/* DEBUG("waiting for packet"); */
		
		rcv_allow();
		while( !BitTest(sp_flags, SPF_SLIP_DONE) )
			;

#if 0		
		puts("pkt done.\n");
	
		for(i=0; i< sp_index; i++){
			if(!(i%16)){
				snprintf(buf, sizeof(buf), "\n%04.4lX :", i);
				puts(buf);
			}
			snprintf(buf, sizeof(buf), " %02.2x", packet_in[i] );
			puts(buf);
		}
		puts("\n");
#endif
		ip_input((u_char*)packet_in, sp_index);


	}
}
