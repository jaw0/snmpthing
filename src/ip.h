

struct ip {
	u_char  ip_hlv;        /* header len, version */
	u_char  ip_tos;
	u_short ip_len;
	u_short ip_id;
	u_short ip_off;
	u_char  ip_ttl;
	u_char  ip_p;
	u_short ip_sum;
	u_long  ip_src;
	u_long  ip_dst;
};

struct udphdr {
	u_short uh_sport;
	u_short uh_dport;
	u_short uh_len;
	u_short uh_sum;
};

struct icmp {
	u_char  icmp_type;
	u_char  icmp_code;
	u_short icmp_cksum;
};

struct pseudohdr {
	u_long  ph_src;
	u_long  ph_dst;
	u_char  ph_zero;
	u_char  ph_proto;
	u_short ph_len;
};

#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_UDP             17              /* user datagram protocol */

#define UDPPORT_SNMP		161		/* SNMP */
#define UDPPORT_SNMPTRAP	162		/* SNMP Traps */
#define UDPPORT_SYSLOG		514		/* syslog */

#define ICMP_ECHOREPLY          0
#define ICMP_ECHO               8


#define INET_DEFAULT_TTL	30

extern u_short ipid;

extern char *inet_ntoa(u_long, char*, int);
extern u_long inet_aton(const char *);
extern u_short hosttonetshort(u_short);


