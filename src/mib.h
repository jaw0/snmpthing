
struct _Mib {
    int len;
    const u_char *oid;
    int otag;
    int tag;
    void *val;
};

#define IT_STRING	1
#define IT_INT		2
#define IT_INT1		3
#define IT_INT4		4
#define IT_OID		5
#define IT_CONST	6


#define READABLE	0x80
#define WRITABLE	0x40

