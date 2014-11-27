
#ifndef TESTING
#pragma optimize(0)
#pragma registers(4)
#pragma rc
#include <mmlib.h>
#else
#include <aliases.h>
#endif

#include <ip.h>
#include <snmp.h>
#include <mib.h>
#include <error.h>
#include <eeprom.h>

#include <nmib.h>

#define H2NS(x)	hosttonetshort(x)

#ifdef TESTING
#	define dprintf(x)	printf x
#else
#	define dprintf(x)
#endif

#pragma xref

extern const struct _Mib mib[ N_MIB ];

extern u_short hosttonetshort( u_short );
extern int putlen( u_char *, int, int);
extern int putint( u_char *, long, int, int);
extern int putstr( u_char *, char *, int, int, int);
extern SNMPObject get_next_object( u_char *, int);
extern void ip_output(u_char *, int);

#ifdef TESTING
void dmp(u_char*p, int l){
	int i;

	for(i=0; i<l; i++){
		/* if( (i%16)== 0 )
			printf("%0.4X: ", p + i);
		*/
		printf("%0.2x ", p[i]);
		if( (i%16)==15 )
			printf("\n");
	}
	printf("\n");
}
#endif


void snmp_output(u_char *pkt, u_char *ebuf, int j){
	int k;
	struct ip *ipi, *ipo;
	struct udphdr *udpi, *udpo;
	
	/* word align */
	if( j&1 ){
		for(k=0; k<j; k++)
			ebuf[k-j-1] = ebuf[k-j];

		udpo = (struct udphdr *) (ebuf -j - sizeof(struct udphdr));
	}else{
		udpo = (struct udphdr *) (ebuf - j - sizeof(struct udphdr) + 1);
	}

	ipo = (struct ip*) (((char*)udpo) - sizeof(struct ip));

	ipi = (struct ip*)pkt;
	udpi = (struct udphdr*) (pkt + sizeof(struct ip));

	/* fill in udp and ip headers */
	udpo->uh_sport = udpi->uh_dport;
	udpo->uh_dport = udpi->uh_sport;
	udpo->uh_len   = H2NS(j);
	udpo->uh_sum   = 0;

	ipo->ip_p = IPPROTO_UDP;
	bcopy( (char*)&ipi->ip_src, (char*)&ipo->ip_dst, 4);
	bcopy( (char*)&eeprom.myipaddr, (char*)&ipo->ip_src, 4);
	
#ifdef TESTING
	dmp( (u_char*)ipo, sizeof(struct ip)+sizeof(struct udphdr)+j);
#else
	ip_output( (u_char*)ipo, sizeof(struct ip)+sizeof(struct udphdr)+j);
#endif
	return;
}

u_char resbuf[2000];
void snmp_input(u_char *pkt, int len){
	u_char *snmppacket;
	int snmplen;
	SNMPObject so, ver, comm, pdu;
	SNMPObject reqid, errst, errin, vl, vb, oid, vo;
	SNMPObject result;
	u_char *ebuf;
	char *strp;
	int rlen, vlen, i, j=0, k;
	int errorstat=0, errorindex=0;
	u_long val;

	ebuf = resbuf + sizeof(resbuf) - 1;
	
	snmppacket = pkt + sizeof(struct ip) + sizeof(struct udphdr);
	snmplen = len - sizeof(struct ip) - sizeof(struct udphdr);

	DEBUG3( "snmp snmppkt = %lx, len = %lu, snmplen = %lu\n", snmppacket, len, snmplen);
	so = get_next_object(snmppacket, snmplen);

	if( so.type != (TAG_SEQ | TYPE_COMPOUND)){
		DEBUG1( "so.type = %x\n", so.type);
		goto error_discard;
	}

	ver  = get_next_object( so.p, so.len );
	if( ver.type != TAG_INTEGER || ver.i != 0 ){
		DEBUG1( "ver.type = %x\n", ver.type);
		goto error_discard;
	}
	
	comm = get_next_object( ver.next, so.len - ver.len );
	if( comm.type != TAG_OCTETSTRING ){
		DEBUG1( "comm.type = %x\n", comm.type);
		goto error_discard;
	}
	
	/* verify  .... */

	
	

	pdu  = get_next_object( comm.next, so.len - ver.len - comm.len );
	switch (pdu.type) {
	  case PDU_GETREQUEST:
	  case PDU_GETNEXT:
	  case PDU_SETREQUEST:
		break;
	  default:
		DEBUG1( "pdu.type = %x\n", pdu.type);
		goto error_discard;
	}

	rlen = pdu.len;
	reqid = get_next_object(pdu.p, rlen);
	if( reqid.type != TAG_INTEGER ){
		DEBUG1( "reqid.type = %x\n", reqid.type);
		goto error_discard;
	}
	
	rlen -= reqid.tlen;
	errst = get_next_object( reqid.next, rlen );
	if( errst.type != TAG_INTEGER ){
		DEBUG1( "errst.type = %x\n", errst.type);
		goto error_discard;
	}
	
	rlen -= errst.tlen;
	errin = get_next_object( errst.next, rlen );
	if( errin.type != TAG_INTEGER ){
		DEBUG1( "errin.type = %x\n", errin.type);
		goto error_discard;
	}
	
	rlen -= errin.tlen;
	vl = get_next_object( errin.next, rlen );
	if( vl.type != (TAG_SEQ | TYPE_COMPOUND)){
		DEBUG1( "vl.type = %x\n", vl.type);
		goto error_discard;
	}
	
	vb = get_next_object( vl.p, vl.len );
	if( vb.type != (TAG_SEQ | TYPE_COMPOUND)){
		DEBUG1( "vb.type = %x\n", vb.type);
		goto error_discard;
	}

	do {
		oid = get_next_object( vb.p, vb.len );
		if( oid.type != TAG_OID ){
			DEBUG1( "oid.type = %x\n", oid.type);
			goto error_discard;
		}
		
		vo = get_next_object( oid.next, vb.len - oid.len );
		if( vo.type == TAG_ERROR ){
			DEBUG1( "vo.type = %x\n", vo.type);
			goto error_discard;
		}
	
		/* look for oid in our table */
		for(i=0; mib[i].len; i++){
			if( oid.len != mib[i].len )
				continue;
			for(k=0; k<oid.len; k++){
				if( oid.p[k] != mib[i].oid[k] )
					break;
			}
			if( k == oid.len )
				break;
		}
		if( !mib[i].len ){
			/* not found report the error */
			errorstat = ERRSTAT_NOSUCHNAME;
		}else{
			if( pdu.type == PDU_GETNEXT ){
				/* go to the next item */
				i++;
				if( !mib[i].len )
				/* there is nothing next - report an error */
					errorstat = ERRSTAT_NOSUCHNAME;
			}
		}

		if( pdu.type == PDU_SETREQUEST ){
			if( !( mib[i].tag & WRITABLE) ){
				/* read-only variable */
				errorstat = ERRSTAT_READONLY;
			}

			/* XXX set it */
		

		}else{
			/* get request */
		
			if( !( mib[i].tag & READABLE) ){
				/* write-only variable */
				errorstat = ERRSTAT_GENERR;
			}
	
			/* get and encode our result */
			/* note: we fill our buffer from the right and work back */
			vlen = 0;
			switch (mib[i].tag & ~WRITABLE ) {
			  case IT_STRING|READABLE:
				strp = (char*)mib[i].val;
				vlen = strlen( strp );
				goto str_comn;
			  case IT_OID|READABLE:
				/* val is ptr to {len, encoded_oid} */
				strp = (char*)mib[i].val;
				vlen = *strp++;
			  str_comn:
				/* copy data */
				j += putstr(ebuf-j, strp, vlen, mib[i].otag, sizeof(resbuf)-j);
				break;
			  case IT_CONST|READABLE:
				val = *(u_long*)&mib[i].val;
				goto int_comn;
			  case IT_INT|READABLE:
				val = *(u_short*)(mib[i].val);
				goto int_comn;
			  case IT_INT4|READABLE:
				val = *(u_long*)(mib[i].val);
				goto int_comn;
			  case IT_INT1|READABLE:
				val = *(u_char*)(mib[i].val);
			  int_comn:
				j += putint(ebuf-j, val, mib[i].otag, sizeof(resbuf)-j);
				break;
			  default:
				/* if we have an error of some sort */
				j += putint(ebuf-j, 0, TAG_INTEGER, sizeof(resbuf)-j);
				break;
			}
		}

		/* copy oid */
		j += putstr(ebuf-j, (char*)mib[i].oid, mib[i].len, TAG_OID, sizeof(resbuf)-j);

		/* output the vb seq */
		j += putlen( ebuf-j, j, sizeof(resbuf)-j);
		ebuf[-j++] = TYPE_COMPOUND | TAG_SEQ;

		/* handle multi-item requests */
		/* get the next request */
		rlen -= vb.tlen;
		vb = get_next_object( vb.next, rlen );
	}while( rlen > 0 && vb.type == (TAG_SEQ | TYPE_COMPOUND));

	
	/* output the vl seq */
	j += putlen( ebuf-j, j, sizeof(resbuf)-j);
	ebuf[-j++] = TYPE_COMPOUND | TAG_SEQ;

	/* errstat, errindex, reqid */
	j += putint(ebuf-j, errorindex, TAG_INTEGER, sizeof(resbuf)-j);
	j += putint(ebuf-j, errorstat,  TAG_INTEGER, sizeof(resbuf)-j);
	j += putint(ebuf-j, reqid.i,    TAG_INTEGER, sizeof(resbuf)-j);
	
	/* length and pdu_response */
	j += putlen( ebuf-j, j, sizeof(resbuf)-j);
	ebuf[-j++] = PDU_GETRESPONSE;

	/* community */
	bcopy((char*)ver.next, (char*)(ebuf-j-comm.tlen+1), comm.tlen);
	j += comm.tlen;
	/* version */
	j += putint( ebuf-j, 0, TAG_INTEGER, sizeof(resbuf)-j);

	/* seq */
	j += putlen( ebuf-j, j, sizeof(resbuf)-j);
	ebuf[-j++] = TYPE_COMPOUND | TAG_SEQ;

	/* whewwww!!! */

	snmp_output(pkt, ebuf, j);
	return;
	
  error_discard:
	/* the SNMP packet was corrupt/invalid/wrong version/wrong community/... */
	/* as per RFC1157, 4.1, we discard and perform no further action */

	return;
}


	
/*
  SNMP packet ::

  ip_hdr, udp_hdr
    SEQ, len
     INT, len, ver
     STR, len, community
     PDU, len
      INT, len, reqid
      INT, len, errst
      INT, len, errin
      SEQ, len
       SEQ, len
        OID, len, oid
	TAG, len, value

*/

