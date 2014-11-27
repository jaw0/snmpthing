

struct Netstat {
	u_long inbytes;
	u_long outbytes;

	u_long inpackets;
	u_long outpackets;

	u_long bad_cksum;
	u_long bad_proto;
	u_long bad_input;
	
	struct {
		u_long total;
		u_long echo;
		u_long bad_type;
	} icmp;
	struct {
		u_long total;
		u_long bad_port;
		struct {
			u_long total;
			u_long bad;
		} snmp;
	} udp;
	struct {
		u_long total;
		u_long bad_port;
	} tcp;
	
	

};

extern struct Netstat netstat;

