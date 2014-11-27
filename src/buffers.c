

#include <mmlib.h>
#include <buffers.h>

struct {
	u_char big[1][2048];
	u_char med[4][512];
	u_char small[8][64];
} bufs;

void initBuf(void){

	bzero((char*)&bufs, sizeof(bufs));
}

Buf *allocbuf(int size, int waitp, int pause){
	int i;
	u_long i0;
	Buf *b;
	
	while(1){

		i1 = spl0();

		switch( size ){

		  case BUF_SMALL:
			for(i=0; i<sizeof(bufs.small)/sizeof(bufs.small[0]); i++){
				b = (struct Buf*)&bufs.small[i];
				if(! b->data ){
					b->size = sizeof(bufs.small[0]) - sizeof(struct Buf);
					b->data = (char*)b + b->size;
					splx(i1);
					return b;
				}
			}
			break;
			
		  case BUF_MED:
			for(i=0; i<sizeof(bufs.med)/sizeof(bufs.med[0]); i++){
				b = (struct Buf*)&bufs.med[i];
				if(! b->data ){
					b->size = sizeof(bufs.med[0]) - sizeof(struct Buf);
					b->data = (char*)b + b->size;
					splx(i1);
					return b;
				}
			}
			break;

		  case BUF_BIG:
			for(i=0; i<sizeof(bufs.big)/sizeof(bufs.big[0]); i++){
				b = (struct Buf*)&bufs.big[i];
				if(! b->data ){
					b->size = sizeof(bufs.big[0]) - sizeof(struct Buf);
					b->data = (char*)b + b->size;
					splx(i1);
					return b;
				}
			}
			break;
		}
		splx(i1);
		
		if( !waitp )
			return 0;

		msleep(pause);
		pause += pause >> 1;
	}
}

void freebuf(Buf *b){

	b->data = 0;
}


			
	
