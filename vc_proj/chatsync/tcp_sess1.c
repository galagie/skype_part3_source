/*  
*
* Direct TCP connect to skype client
* cmd 109 session
*
*/

// for rc4
#include "rc4/Expand_IV.h"

// for aes
#include "crypto/rijndael.h"

// for global variables
#include "global_vars.h"

// for types
#include "short_types.h"

// for 41
#include "decode41.h"

// for mip miracl
#include "crypto/miracl.h"

// rc4 obfuscation
extern int Skype_RC4_Expand_IV (RC4_context * const rc4, const u32 iv, const u32 flags);
extern void RC4_crypt (u8 * buffer, u32 bytes, RC4_context * const rc4, const u32 test);

extern int Calculate_CRC32_For41(char *a2, int a3);
extern unsigned int Calculate_CRC32(char *crc32, int bytes);

// socket comm
extern int udp_talk(char *remoteip, unsigned short remoteport, char *buf, int len, char *result);
extern int tcp_talk(char *remoteip, unsigned short remoteport, char *buf, int len, char *result,int need_close);

// sha1 and rsa crypto function
extern int _get_sha1_data(char *buf, int len, char *outbuf, int need_convert);

extern int _get_sign_data(_MIPD_ char *globalptr, char *buf, int len, char *outbuf);

// utils
extern int process_tcp_packet(char *globalptr, char *resp, int resp_len);
extern int get_blkseq(char *data, int datalen);
extern int process_aes_crypt(char *globalptr, char *data, int datalen, int usekey, int blkseq, int need_xor);
extern int show_memory(char *mem, int len, char *text);
extern int get_packet_size(char *data,int len);
extern int decode41(char *data, int len, char *text);
extern int set_packet_size(char *a1, int c);
extern int process_aes(char *globalptr, char *buf, int buf_len, int usekey, int blkseq, int need_xor);
extern int first_bytes_correction(char *globalptr, char *header, int header_len, char *buf, int buf_len);

//blobs encode 
int encode41_sesspkt_ack(char *globalptr, char *buf, int buf_limit_len, uint cmd);


int encode41_sess1pkt1_new(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_sess1pkt1(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess1pkt2(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_sess2pkt2(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess2pkt3(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_sess3pkt2(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess3pkt4(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess3pkt5(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess3pkt7(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess3pkt8(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_sess4pkt3(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess4pkt4(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_newblk1(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_newblk2(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_newblk3(char *globalptr, char *buf, int buf_limit_len);


extern int encode41_sess1pkt1_new14(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess1pkt1_new13(char *globalptr, char *buf, int buf_limit_len);
extern int encode41_sess1pkt1_new12(char *globalptr, char *buf, int buf_limit_len);

extern int encode41_sess1pkt1_old(char *globalptr, char *buf, int buf_limit_len);
	
// global data


extern uint DEBUG_LEVEL;






int make_tcp_client_sess1_pkt4o(char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	/*
	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;
	
	u8 buf3[0x1000];
	int buf3_len;
	u8 buf3header[0x10];
	int buf3header_len;

	u8 buf4[0x1000];
	int buf4_len;
	u8 buf4header[0x10];
	int buf4header_len;
	*/

	unsigned int chatrnd;

	struct global_s *global;
	global=(struct global_s *)globalptr;


	tmplen=strlen(global->CHAT_STRING)-4;
	
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	global->CHAT_STRING[tmplen]=0;
	tmplen++;

	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);

	if(DEBUG_LEVEL>=100) printf("CHAT ID:%s\n",global->CHAT_STRING);

	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	if(DEBUG_LEVEL>=100) printf("Sending four TCP packet, session cmd 109, pkt 1\n");
	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	
	//////////////////////////////////////////////////
	// modify challenge response blob, in aes data
	//////////////////////////////////////////////////


	//!!!
	//strcpy(global->CHAT_STRING,"#zelda.livero/$ave.gaft;4fea66013cdd3865");
	strcpy(global->CHAT_STRING,"#zelda.thegame1/$ave.gaft;c71aa72be96d4da2");
	//!!!



	///////////////////////////////
	// first 41
	///////////////////////////////


	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sess1pkt1_old(globalptr, buf1, sizeof(buf1));
	if (buf1_len<0) {
		printf("buf size failed\n");
		return -1;
	};
	show_memory(buf1, buf1_len, "sess1pkt1");

	blkseq=0x02;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	//


	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess1pkt1header");



	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//aes 1
	memcpy(pkt+len,buf1header,buf1header_len);
	len=len+buf1header_len;
	
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	

	
	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");

	*pkt_len=len;

	return 0;

};


///////////////////////////////
//tcp four(4) packet
///////////////////////////////
unsigned int make_tcp_client_sess1_pkt4n(char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;
	
	u8 buf3[0x1000];
	int buf3_len;
	u8 buf3header[0x10];
	int buf3header_len;

	u8 buf4[0x1000];
	int buf4_len;
	u8 buf4header[0x10];
	int buf4header_len;

	unsigned int chatrnd;

	struct global_s *global;
	global=(struct global_s *)globalptr;


	tmplen=strlen(global->CHAT_STRING)-4;
	
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	global->CHAT_STRING[tmplen]=0;
	tmplen++;

	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);

	if(DEBUG_LEVEL>=100) printf("CHAT ID:%s\n",global->CHAT_STRING);

	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	if(DEBUG_LEVEL>=100) printf("Sending four TCP packet, session cmd 109, pkt 1\n");
	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	
	//////////////////////////////////////////////////
	// modify challenge response blob, in aes data
	//////////////////////////////////////////////////


	//!!!
	//strcpy(global->CHAT_STRING,"#zelda.livero/$ave.gaft;4fea66013cdd3865");
	//strcpy(global->CHAT_STRING,"#zelda.livero/$ave.gaft;4fea66013cdd7082");
	//!!!



	///////////////////////////////
	// first 41
	///////////////////////////////


	memset(buf1,0,sizeof(buf1));
  	//buf1_len=encode41_sess1pkt1_new12(globalptr, buf1, sizeof(buf1));
  	buf1_len=encode41_sesspkt_ack(globalptr, buf1, sizeof(buf1), global->confirm[0]);
	if (buf1_len<0) {
		printf("buf size failed\n");
		return -1;
	};
	show_memory(buf1, buf1_len, "sess1pkt1");

	blkseq=0x02;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	//

	memset(buf2,0,sizeof(buf2));
  	buf2_len=encode41_sess1pkt1_new12(globalptr, buf2, sizeof(buf2));
	if (buf2_len<0) {
		printf("buf size failed\n");
		return -1;
	};
	show_memory(buf2, buf2_len, "sess1pkt1");

	blkseq=0x03;
	buf2_len=process_aes(globalptr, buf2, buf2_len, 1, blkseq, 0);



	//

	memset(buf3,0,sizeof(buf3));
  	buf3_len=encode41_sess1pkt1_new13(globalptr, buf3, sizeof(buf3));
	if (buf3_len<0) {
		printf("buf size failed\n");
		return -1;
	};
	show_memory(buf3, buf3_len, "sess1pkt1");

	blkseq=0x04;
	buf3_len=process_aes(globalptr, buf3, buf3_len, 1, blkseq, 0);


	//

	memset(buf4,0,sizeof(buf4));
  	buf4_len=encode41_sess1pkt1_new14(globalptr, buf4, sizeof(buf4));
	if (buf4_len<0) {
		printf("buf size failed\n");
		return -1;
	};
	show_memory(buf4, buf4_len, "sess1pkt1");

	blkseq=0x05;
	buf4_len=process_aes(globalptr, buf4, buf4_len, 1, blkseq, 0);


	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess1pkt1header");

	buf2header_len=first_bytes_correction(globalptr, buf2header, sizeof(buf2header)-1, buf2, buf2_len);
	show_memory(buf2header, buf2header_len, "sess1pkt1header");

	buf3header_len=first_bytes_correction(globalptr, buf3header, sizeof(buf3header)-1, buf3, buf3_len);
	show_memory(buf3header, buf3header_len, "sess1pkt1header");

	buf4header_len=first_bytes_correction(globalptr, buf4header, sizeof(buf4header)-1, buf4, buf4_len);
	show_memory(buf4header, buf4header_len, "sess1pkt1header");


	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//aes 1
	memcpy(pkt+len,buf1header,buf1header_len);
	len=len+buf1header_len;
	
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	

	//
	memcpy(pkt+len,buf2header,buf2header_len);
	len=len+buf2header_len;
	
	memcpy(pkt+len,buf2,buf2_len);
	len=len+buf2_len;

	//
	memcpy(pkt+len,buf3header,buf3header_len);
	len=len+buf3header_len;
	
	memcpy(pkt+len,buf3,buf3_len);
	len=len+buf3_len;

	//
	memcpy(pkt+len,buf4header,buf4header_len);
	len=len+buf4header_len;
	
	memcpy(pkt+len,buf4,buf4_len);
	len=len+buf4_len;

	
	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");

	*pkt_len=len;

	return 0;

};

/*
///////////////////////////////
//tcp four(4) packet
///////////////////////////////
unsigned int make_tcp_client_sess1_pkt4(_MIPD_ char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;
	
	unsigned int chatrnd;

	struct global_s *global;
	global=(struct global_s *)globalptr;


	tmplen=strlen(global->CHAT_STRING)-4;
	
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);
	chatrnd=(rand() % 0x9);
	global->CHAT_STRING[tmplen]=global->CHAT_STRING[tmplen]+chatrnd;
	tmplen++;
	if(DEBUG_LEVEL>=100) printf("chatrnd=%d\n",chatrnd);

	if(DEBUG_LEVEL>=100) printf("CHAT ID:%s\n",global->CHAT_STRING);

	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	if(DEBUG_LEVEL>=100) printf("Sending four TCP packet, session cmd 109, pkt 1\n");
	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	
	//////////////////////////////////////////////////
	// modify challenge response blob, in aes data
	//////////////////////////////////////////////////


	///////////////////////////////
	// first 41
	///////////////////////////////

	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sess1pkt1(globalptr, buf1, sizeof(buf1));
	show_memory(buf1, buf1_len, "sess1pkt1");

	//aes encrypt block 1
	blkseq=0x02;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	///////////////////////////////
	// second 41
	///////////////////////////////

	memset(buf2,0,sizeof(buf2));
  	buf2_len=encode41_sess1pkt2(globalptr, buf2, sizeof(buf2));
	show_memory(buf2, buf2_len, "sess1pkt2");

	//aes encrypt block 2
	blkseq=0x03;
	buf2_len=process_aes(globalptr, buf2, buf2_len, 1, blkseq, 0);


	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess1pkt1header");


	/////////////////////////////////////
	// first bytes correction, block 2
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf2header_len=first_bytes_correction(globalptr, buf2header, sizeof(buf2header)-1, buf2, buf2_len);
	show_memory(buf2header, buf2header_len, "sess1pkt2header");

	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//aes 1
	memcpy(pkt,buf1header,buf1header_len);
	len=len+buf1header_len;
	
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	
	// aes 2
	memcpy(pkt+len,buf2header,buf2header_len);
	len=len+buf2header_len;
	
	memcpy(pkt+len,buf2,buf2_len);
	len=len+buf2_len;
	

	
	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");

	*pkt_len=len;

	return 0;

};
*/







///////////////////////////////
//tcp fifth(5) packet
///////////////////////////////
unsigned int make_tcp_client_sess1_pkt5(_MIPD_ char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;
	
	u8 buf3[0x1000];
	int buf3_len;
	u8 buf3header[0x10];
	int buf3header_len;
	
	u8 buf4[0x1000];
	int buf4_len;
	u8 buf4header[0x10];
	int buf4header_len;
	

	u8 buf_newblk1[0x1000];
	int buf_newblk1_len;

	struct global_s *global;
	global=(struct global_s *)globalptr;



	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	if(DEBUG_LEVEL>=100) printf("Sending fifth(5) TCP packet, session cmd 109, pkt 2\n");
	if(DEBUG_LEVEL>=100) printf("::::::::::::::::::::::::::::::::::::::::::::::::::\n");

	
	///////////////////////////////
	// first 41
	///////////////////////////////

	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sesspkt_ack(globalptr, buf1, sizeof(buf1), global->confirm[0]);
	show_memory(buf1, buf1_len, "sess2pkt1");

	// aes encrypt block 1
	blkseq=0x04;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	///////////////////////////////
	// second 41
	///////////////////////////////


	memset(global->NEWBLK,0,sizeof(global->NEWBLK));
	global->NEWBLK_LEN=0;

	global->NEWBLK[0]=0x6A;
    global->NEWBLK_LEN++;

	/////////////////////////////
	// SHA1 digest 0
	/////////////////////////////
	// for make digest at the _start_ of newbkl
	// crypted credentials(0x100) + chatid(0x24)
	if (1) {
		char *buf;
		char *outbuf;
		int tlen;

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		// credentials
		memcpy(buf,global->CREDENTIALS, global->CREDENTIALS_LEN);

		// chatid 
		memcpy(buf+global->CREDENTIALS_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
		tlen=global->CREDENTIALS_LEN+strlen(global->CHAT_STRING);

		// show data for hashing
		show_memory(buf, tlen, "CHATID input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);

		// show 
		show_memory(outbuf, 0x14, "CHATID(hash) OUTPUT");

		// copy sha1 to new blk, at start+1
		memcpy(global->NEWBLK+global->NEWBLK_LEN,outbuf,0x14);
    	global->NEWBLK_LEN+=0x14;

	};




	
	//////////////////////////////////////////////////
	// modify chatid in newblk , in aes data
	//////////////////////////////////////////////////
	memcpy(global->NEWBLK+global->NEWBLK_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
	global->NEWBLK_LEN+=strlen(global->CHAT_STRING);


	memset(buf_newblk1,0,sizeof(buf_newblk1));
  	buf_newblk1_len=encode41_newblk1(globalptr, buf_newblk1, sizeof(buf_newblk1));
	show_memory(buf_newblk1, buf_newblk1_len, "buf_newblk1");
	

	tmplen=buf_newblk1_len;
	if(1){
		int tlen_ost;
		int tlen_need;
		int tlen_first;
		int tlen_second;

		tlen_ost=0x80-global->NEWBLK_LEN-0x15;
		tlen_need=buf_newblk1_len;

		if (tlen_ost < tlen_need){
			tlen_first=tlen_ost;
			tlen_second=tlen_need-tlen_first;
			tmplen=tlen_first;
		};
	
	};


	// middle of newblk .. some 41 data..
    memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk1,tmplen);
	global->NEWBLK_LEN+=tmplen;


	if (global->NEWBLK_LEN+0x15 != 0x80) {
			show_memory(global->NEWBLK,0x80,"newblk:");
			if(DEBUG_LEVEL>=100) printf("NEWBLK LEN encode error, LEN=0x%08X\n",global->NEWBLK_LEN+0x15);
			return -1;
	};

	global->NEWBLK[0x7f]=0xBC;

	/////////////////////////////
	// SHA1 digest 1
	/////////////////////////////
	// for make digest at the end of newblk
	// data under crypto(0x80) + cleartext data after(0x12)
	if (1) {
		char *buf;
		char *outbuf;
		u32 tlen;


		global->NEWBLK_LEN=0x80;

		if ( tmplen!= buf_newblk1_len ){
			// aes41
			tlen=buf_newblk1_len-tmplen;
			memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk1+tmplen,tlen);
			global->NEWBLK_LEN+=tlen;
		};



		/*
	    memcpy(NEWBLK+NEWBLK_LEN,"\x01",1);
		NEWBLK_LEN++;

		tlen=strlen(REMOTE_NAME)+1;
	    memcpy(NEWBLK+NEWBLK_LEN,REMOTE_NAME,tlen);
		NEWBLK_LEN+=tlen;

	    memcpy(NEWBLK+NEWBLK_LEN,"\x00\x0A\xAA\xEE\xF5\x46\x00\x0B\x01", 9);
	    NEWBLK_LEN+=9;

		*/

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		// first char not count 
		// last 0x14 + BC is sha1 hash
		tlen=0x80-0x14-1-1;
		memcpy(buf,global->NEWBLK+1,tlen);
		memcpy(buf+tlen,global->NEWBLK+0x80,global->NEWBLK_LEN-0x80);
		tlen=tlen+global->NEWBLK_LEN-0x80;

		// show data for hashing
		show_memory(buf, tlen, "NEWBLK input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);


		// show 
		show_memory(outbuf, 0x14, "NEWBLK(hash) OUTPUT");

		// copy sha1 to new blk, at end, before BC
		memcpy(global->NEWBLK+0x80-0x14-1,outbuf,0x14);

	};


	show_memory(global->NEWBLK, global->NEWBLK_LEN, "NEWBLK new OUTPUT");
	
	///////////////////////
	//RSA sign
	///////////////////////
	//for sign newblk with our(xoteg) private key
	if (1) {
		char *buf;
		char *outbuf;


		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);


		//copy challenge template
		memcpy(buf,global->NEWBLK,0x80);
		
		//print newblk data
		//before RSA sign-ing
		show_memory(buf, 0x80, "newblk RSA SIGN input");

		//make rsa sign
		_get_sign_data(_MIPP_ globalptr, buf, 0x80, outbuf);

		////copy rsa sign to challenge_response buffer
		////for send this response in next pkt
		memcpy(global->NEWBLK,outbuf,0x80);

		//print rsa signed newblk data
		show_memory(outbuf, 0x80, "newblk RSA SIGN output");

	};
	


	memset(buf2,0,sizeof(buf2));
  	buf2_len=encode41_sess2pkt2(globalptr, buf2, sizeof(buf2));
	show_memory(buf2, buf2_len, "sess2pkt2");

	//aes encrypt block 2
	blkseq=0x05;
	buf2_len=process_aes(globalptr, buf2, buf2_len, 1, blkseq, 0);


	///////////////////////////////
	// third 41
	///////////////////////////////

	memset(buf3,0,sizeof(buf3));
  	buf3_len=encode41_sess2pkt3(globalptr, buf3, sizeof(buf3));
	show_memory(buf3, buf3_len, "sess2pkt3");

	//aes encrypt block 3
	blkseq=0x06;
	buf3_len=process_aes(globalptr, buf3, buf3_len, 1, blkseq, 0);

	///////////////////////////////
	// fourth 41
	///////////////////////////////
	memset(buf4,0,sizeof(buf4));
  	buf4_len=encode41_sesspkt_ack(globalptr, buf4, sizeof(buf4), global->confirm[1]);
	show_memory(buf4, buf4_len, "sess2pkt4");

	//aes encrypt block 4
	blkseq=0x07;
	buf4_len=process_aes_crypt(globalptr, buf4, buf4_len, 1, blkseq, 0);



	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess2pkt1header");


	/////////////////////////////////////
	// first bytes correction, block 2
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf2header_len=first_bytes_correction(globalptr, buf2header, sizeof(buf2header)-1, buf2, buf2_len);
	show_memory(buf2header, buf2header_len, "sess2pkt2header");

	/////////////////////////////////////
	// first bytes correction, block 3
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf3header_len=first_bytes_correction(globalptr, buf3header, sizeof(buf3header)-1, buf3, buf3_len);
	show_memory(buf3header, buf3header_len, "sess2pkt3header");


	/////////////////////////////////////
	// first bytes correction, block 4
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	buf4header_len=first_bytes_correction(globalptr, buf4header, sizeof(buf4header)-1, buf4, buf4_len);
	show_memory(buf4header, buf4header_len, "sess2pkt4header");


	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//block 1
	memcpy(pkt+len,buf1header,buf1header_len);
	len=len+buf1header_len;
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	
	// block 2
	memcpy(pkt+len,buf2header,buf2header_len);
	len=len+buf2header_len;
	memcpy(pkt+len,buf2,buf2_len);
	len=len+buf2_len;

	// block 3
	memcpy(pkt+len,buf3header,buf3header_len);
	len=len+buf3header_len;
	memcpy(pkt+len,buf3,buf3_len);
	len=len+buf3_len;
	
	// block 4
	memcpy(pkt+len,buf4header,buf4header_len);
	len=len+buf4header_len;
	memcpy(pkt+len,buf4,buf4_len);
	len=len+buf4_len;
	

	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");


	*pkt_len=len;

	return 0;

};


int process_tcp_client_sess1_pkt5(_MIPD_ char *globalptr, char *resp, int resp_len){
	u8 recvbuf[0x1000];
	char header41[5];
	int i,j;
	int len;
	int tmplen;
	int recvlen;
	int remote_blkseq;

	struct global_s *global;
	global=(struct global_s *)globalptr;

	
	// recv pkt
	show_memory(resp, resp_len, "Result");	


	// copy pkt
	recvlen=resp_len;
	memcpy(recvbuf,resp,recvlen);


	/////////////////////////////////
	// RC4 decrypt pkt
	/////////////////////////////////
	show_memory(recvbuf, recvlen, "Before RC4 decrypt");	
	RC4_crypt (recvbuf, recvlen, &global->rc4_recv, 0);
	show_memory(recvbuf, recvlen, "After RC4 decrypt");	


	/////////////////////////////////
	// processing response
	/////////////////////////////////
	// by aes blocks with 109 cmd each
	i=0;j=0;
	global->confirm_count=0;
	len=resp_len;
	while(len > 0){
		j++;

		if(DEBUG_LEVEL>=100) printf("PKT BLK: %d\n\n",j);

		tmplen=get_packet_size(recvbuf+i, 4);
		tmplen=tmplen-1;
		if (tmplen > 0x1000){
			if(DEBUG_LEVEL>=100) printf("pkt block size too big, len: 0x%08X\n",tmplen);
			return -1;
		};
		if (tmplen <= 0){
			if(DEBUG_LEVEL>=100) printf("pkt block size too small, len: 0x%08X\n",tmplen);
			return -1;
		};

		show_memory(recvbuf+i, 4, "Header");

		//get blkseq, need full aes len with crc32
		remote_blkseq=get_blkseq(recvbuf+i+4,tmplen-2);

		process_aes_crypt(globalptr, recvbuf+i+4, tmplen-2, 1, remote_blkseq, 1);

		//decode41(recvbuf+i+4, tmplen-2,"PKT 6D reply2");


		//processing headers, need for cmd conformation
		memcpy(header41,recvbuf+i+4,5);

		//remember first byte sess id, for send in confirmation
		if (header41[3]==0x6d){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x6d){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x4D){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[3]==0x4D){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};





		len=len-tmplen-2;
		i=i+tmplen+2;

		if(DEBUG_LEVEL>=100) printf("len(left)=%d i(bytes processed)=%d\n",len,i);
		if(DEBUG_LEVEL>=100) printf("len(current block)=%d\n",tmplen);
		if(DEBUG_LEVEL>=100) printf("confirm_count=%d\n",global->confirm_count);

	};



	return 0;
};









///////////////////////////////
//tcp sixth(6) packet
///////////////////////////////
unsigned int make_tcp_client_sess1_pkt6(_MIPD_ char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;

	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;

	u8 buf3[0x1000];
	int buf3_len;
	u8 buf3header[0x10];
	int buf3header_len;

	u8 buf4[0x1000];
	int buf4_len;
	u8 buf4header[0x10];
	int buf4header_len;

	u8 buf5[0x1000];
	int buf5_len;
	u8 buf5header[0x10];
	int buf5header_len;

	u8 buf6[0x1000];
	int buf6_len;
	u8 buf6header[0x10];
	int buf6header_len;

	u8 buf7[0x1000];
	int buf7_len;
	u8 buf7header[0x10];
	int buf7header_len;

	u8 buf8[0x1000];
	int buf8_len;
	u8 buf8header[0x10];
	int buf8header_len;

	u8 buf9[0x1000];
	int buf9_len;
	u8 buf9header[0x10];
	int buf9header_len;

	u8 buf_newblk2[0x1000];
	int buf_newblk2_len;


	struct global_s *global;
	global=(struct global_s *)globalptr;


	if(DEBUG_LEVEL>=100) printf("Sending sixth(6) TCP packet, session cmd 109, pkt 3\n");
	


	///////////////////////////////
	// first 41
	///////////////////////////////

	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sesspkt_ack(globalptr, buf1, sizeof(buf1), global->confirm[0]);
	show_memory(buf1, buf1_len, "sess3pkt1ack");

	// aes encrypt block 1
	blkseq=0x08;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	///////////////////////////////
	// second 41
	///////////////////////////////

	memset(buf2,0,sizeof(buf2));
  	buf2_len=encode41_sess3pkt2(globalptr, buf2, sizeof(buf2));
	show_memory(buf2, buf2_len, "sess3pkt2");

	//aes encrypt block 2
	blkseq=0x09;
	buf2_len=process_aes(globalptr, buf2, buf2_len, 1, blkseq, 0);



	///////////////////////////////
	// third 41
	///////////////////////////////

	memset(buf3,0,sizeof(buf3));
  	buf3_len=encode41_sesspkt_ack(globalptr, buf3, sizeof(buf3), global->confirm[1]);
	show_memory(buf3, buf3_len, "sess3pkt3ack");

	//aes encrypt block 3
	blkseq=0x0A;
	buf3_len=process_aes(globalptr, buf3, buf3_len, 1, blkseq, 0);


	///////////////////////////////
	// fouth block4 41
	///////////////////////////////



	//////////////////////////////////////////////////
	// modify credentials, in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data4+0xc5,aes_41data4_fix,0x100);


	memset(global->NEWBLK,0,sizeof(global->NEWBLK));
	global->NEWBLK_LEN=0;

	global->NEWBLK[0]=0x6A;
    global->NEWBLK_LEN++;


	/////////////////////////////
	// SHA1 digest 0
	/////////////////////////////
	// for make digest at the _start_ of newbkl
	// crypted credentials(0x100) + chatid(0x24)
	if (1) {
		char *buf;
		char *outbuf;
		int tlen;

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		// credentials
		memcpy(buf,global->CREDENTIALS,global->CREDENTIALS_LEN);

		// + chatid 
		//memcpy(buf+4+0x100,"#xoteg_iam/$xot_iam;4fef7b015cb20ad0",0x24);
		memcpy(buf+global->CREDENTIALS_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
		tlen=global->CREDENTIALS_LEN+strlen(global->CHAT_STRING);

		// show data for hashing
		show_memory(buf, tlen, "CHATID 2 input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);

		// show 
		show_memory(outbuf, 0x14, "CHATID(hash) 2 OUTPUT");

		// copy sha1 to new blk, at start+1
		memcpy(global->NEWBLK+1,outbuf,0x14);
		global->NEWBLK_LEN+=0x14;

	};




	
	//////////////////////////////////////////////////
	// modify chatid in newblk , in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data4_newblk+0x15,"#xoteg_iam/$xot_iam;4fef7b015cb20ad0",0x24);
	//memcpy(aes_41data4_newblk+0x15,CHAT_STRING,0x24);

	memcpy(global->NEWBLK+global->NEWBLK_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
	global->NEWBLK_LEN+=strlen(global->CHAT_STRING);

	memset(buf_newblk2,0,sizeof(buf_newblk2));
  	buf_newblk2_len=encode41_newblk2(globalptr, buf_newblk2, sizeof(buf_newblk2));
	show_memory(buf_newblk2, buf_newblk2_len, "buf_newblk2");


	tmplen=buf_newblk2_len;
	if(1){
		int tlen_ost;
		int tlen_need;
		int tlen_first;
		int tlen_second;

		tlen_ost=0x80-global->NEWBLK_LEN-0x15;
		tlen_need=buf_newblk2_len;

		if (tlen_ost < tlen_need){
			tlen_first=tlen_ost;
			tlen_second=tlen_need-tlen_first;
			tmplen=tlen_first;
		};
	
	};


	// middle of newblk .. some 41 data..
    memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk2,tmplen);
	global->NEWBLK_LEN+=tmplen;


	if (global->NEWBLK_LEN+0x15 != 0x80) {
			show_memory(global->NEWBLK,0x80,"newblk:");
			if(DEBUG_LEVEL>=100) printf("NEWBLK2 LEN encode error, LEN=0x%08X\n",global->NEWBLK_LEN+0x15);
			return -1;
	};

	global->NEWBLK[0x7f]=0xBC;

	

	/////////////////////////////
	// SHA1 digest 1
	/////////////////////////////
	// for make digest at the end of newblk
	// data under crypto(0x80) + cleartext data after(0x0c)
	if (1) {
		char *buf;
		char *outbuf;
		u32 tlen;

		global->NEWBLK_LEN=0x80;

		if ( tmplen!= buf_newblk2_len ){
			// aes41
			tlen=buf_newblk2_len-tmplen;
			memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk2+tmplen,tlen);
			global->NEWBLK_LEN+=tlen;
		};


		/*
		memcpy(NEWBLK+NEWBLK_LEN,"\x0E\x00\x00\x0F\x00\x00\x0A\x9D\xED\xA2\x90\x04", 0x0C);
	    NEWBLK_LEN=NEWBLK_LEN+0x0C;
		*/

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		// first char not count 
		// last 0x14 + BC is sha1 hash
		tlen=0x80-0x14-1-1;
		memcpy(buf,global->NEWBLK+1,tlen);
		memcpy(buf+tlen,global->NEWBLK+0x80,global->NEWBLK_LEN-0x80);
		tlen=tlen+global->NEWBLK_LEN-0x80;

		// show data for hashing
		show_memory(buf, tlen, "NEWBLK 2 input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);


		// show 
		show_memory(outbuf, 0x14, "NEWBLK(hash) 2 OUTPUT");

		// copy sha1 to new blk, at end, before BC
		memcpy(global->NEWBLK+0x80-0x14-1,outbuf,0x14);

	};


	show_memory(global->NEWBLK, global->NEWBLK_LEN, "NEWBLK2 new OUTPUT");


	///////////////////////
	//RSA sign
	///////////////////////
	//for sign newblk with our(xoteg) private key
	if (1) {
		char *buf;
		char *outbuf;


		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);


		//copy challenge template
		memcpy(buf,global->NEWBLK,0x80);
		
		//print newblk data
		//before RSA sign-ing
		show_memory(buf, 0x80, "newblk 2 RSA SIGN input");

		//make rsa sign
		_get_sign_data(_MIPP_ globalptr, buf, 0x80, outbuf);

		////copy rsa sign to challenge_response buffer
		////for send this response in next pkt
		memcpy(global->NEWBLK,outbuf,0x80);

		//print rsa signed newblk data
		show_memory(outbuf, 0x80, "newblk 2 RSA SIGN output");

	};
	


	//////////////////////////////////////////////////
	// modify sign new block with hash on cred+chatid , in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data4+0x31,aes_41data4_newblk,0x80);



	
	memset(buf4,0,sizeof(buf4));
  	buf4_len=encode41_sess3pkt4(globalptr, buf4, sizeof(buf4));
	show_memory(buf4, buf4_len, "sess3pkt4");

	//aes encrypt block 4
	blkseq=0x0B;
	buf4_len=process_aes(globalptr, buf4, buf4_len, 1, blkseq, 0);



	///////////////////////////////
	// fifth block5 41
	///////////////////////////////

	
	memset(buf5,0,sizeof(buf5));
  	buf5_len=encode41_sess3pkt5(globalptr, buf5, sizeof(buf5));
	show_memory(buf5, buf5_len, "sess3pkt5");

	//aes encrypt block 5
	blkseq=0x0C;
	buf5_len=process_aes(globalptr, buf5, buf5_len, 1, blkseq, 0);


	///////////////////////////////
	// block6 41
	///////////////////////////////

	memset(buf6,0,sizeof(buf6));
  	buf6_len=encode41_sesspkt_ack(globalptr, buf6, sizeof(buf6), global->confirm[2]);
	show_memory(buf6, buf6_len, "sess3pkt6ack");

	//aes encrypt block 6
	blkseq=0x0D;
	buf6_len=process_aes(globalptr, buf6, buf6_len, 1, blkseq, 0);




	///////////////////////////////
	// seventh block7 41
	///////////////////////////////


	//////////////////////////////////////////////////
	// modify credentials, in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data7+0x53,REMOTE_NAME,0x07);


	memset(buf7,0,sizeof(buf7));
  	buf7_len=encode41_sess3pkt7(globalptr, buf7, sizeof(buf7));
	show_memory(buf7, buf7_len, "sess3pkt7");


	//aes encrypt block 7
	blkseq=0x0E;
	buf7_len=process_aes(globalptr, buf7, buf7_len, 1, blkseq, 0);


	///////////////////////////////
	// eighth block8 41
	///////////////////////////////

	memset(buf8,0,sizeof(buf8));
  	buf8_len=encode41_sess3pkt8(globalptr, buf8, sizeof(buf8));
	show_memory(buf8, buf8_len, "sess3pkt8");

	//aes encrypt block 8
	blkseq=0x0F;
	buf8_len=process_aes(globalptr, buf8, buf8_len, 1, blkseq, 0);


	///////////////////////////////
	// block9 41
	///////////////////////////////

	memset(buf9,0,sizeof(buf9));
  	buf9_len=encode41_sesspkt_ack(globalptr, buf9, sizeof(buf9), global->confirm[3]);
	show_memory(buf9, buf9_len, "sess3pkt9ack");

	//aes encrypt block 9
	blkseq=0x10;
	buf9_len=process_aes(globalptr, buf9, buf9_len, 1, blkseq, 0);


	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing	

	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess3pkt1header");

	/////////////////////////////////////
	// first bytes correction, block 2
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf2header_len=first_bytes_correction(globalptr, buf2header, sizeof(buf2header)-1, buf2, buf2_len);
	show_memory(buf2header, buf2header_len, "sess3pkt2header");

	/////////////////////////////////////
	// first bytes correction, block 3
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	
	buf3header_len=first_bytes_correction(globalptr, buf3header, sizeof(buf3header)-1, buf3, buf3_len);
	show_memory(buf3header, buf3header_len, "sess3pkt3header");

	/////////////////////////////////////
	// first bytes correction, block 4
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf4header_len=first_bytes_correction(globalptr, buf4header, sizeof(buf4header)-1, buf4, buf4_len);
	show_memory(buf4header, buf4header_len, "sess3pkt4header");

	/////////////////////////////////////
	// first bytes correction, block 5
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf5header_len=first_bytes_correction(globalptr, buf5header, sizeof(buf5header)-1, buf5, buf5_len);
	show_memory(buf5header, buf5header_len, "sess3pkt5header");

	/////////////////////////////////////
	// first bytes correction, block 6
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf6header_len=first_bytes_correction(globalptr, buf6header, sizeof(buf6header)-1, buf6, buf6_len);
	show_memory(buf6header, buf6header_len, "sess3pkt6header");

	/////////////////////////////////////
	// first bytes correction, block 7
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf7header_len=first_bytes_correction(globalptr, buf7header, sizeof(buf7header)-1, buf7, buf7_len);
	show_memory(buf7header, buf7header_len, "sess3pkt7header");

	/////////////////////////////////////
	// first bytes correction, block 8
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf8header_len=first_bytes_correction(globalptr, buf8header, sizeof(buf8header)-1, buf8, buf8_len);
	show_memory(buf8header, buf8header_len, "sess3pkt8header");

	/////////////////////////////////////
	// first bytes correction, block 9
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf9header_len=first_bytes_correction(globalptr, buf9header, sizeof(buf9header)-1, buf9, buf9_len);
	show_memory(buf9header, buf9header_len, "sess3pkt9header");



	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//block 1
	memcpy(pkt+len,buf1header,buf1header_len);
	len=len+buf1header_len;
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	
	// block 2
	memcpy(pkt+len,buf2header,buf2header_len);
	len=len+buf2header_len;
	memcpy(pkt+len,buf2,buf2_len);
	len=len+buf2_len;

	// block 3
	memcpy(pkt+len,buf3header,buf3header_len);
	len=len+buf3header_len;
	memcpy(pkt+len,buf3,buf3_len);
	len=len+buf3_len;
	
	// block 4
	memcpy(pkt+len,buf4header,buf4header_len);
	len=len+buf4header_len;
	memcpy(pkt+len,buf4,buf4_len);
	len=len+buf4_len;
	
	// block 5
	memcpy(pkt+len,buf5header,buf5header_len);
	len=len+buf5header_len;
	memcpy(pkt+len,buf5,buf5_len);
	len=len+buf5_len;
	
	// block 6
	memcpy(pkt+len,buf6header,buf6header_len);
	len=len+buf6header_len;
	memcpy(pkt+len,buf6,buf6_len);
	len=len+buf6_len;
	
	// block 7
	memcpy(pkt+len,buf7header,buf7header_len);
	len=len+buf7header_len;
	memcpy(pkt+len,buf7,buf7_len);
	len=len+buf7_len;
	
	// block 8
	memcpy(pkt+len,buf8header,buf8header_len);
	len=len+buf8header_len;
	memcpy(pkt+len,buf8,buf8_len);
	len=len+buf8_len;
	
	// block 9
	memcpy(pkt+len,buf9header,buf9header_len);
	len=len+buf9header_len;
	memcpy(pkt+len,buf9,buf9_len);
	len=len+buf9_len;
	


	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");

	*pkt_len=len;

	return 0;

};



int process_tcp_client_sess1_pkt6(_MIPD_ char *globalptr, char *resp, int resp_len){
	u8 recvbuf[0x1000];
	char header41[0x100];
	int i,j;
	int len;
	int tmplen;
	int recvlen;
	int remote_blkseq;


	struct global_s *global;
	global=(struct global_s *)globalptr;


	
	// recv pkt
	show_memory(resp, resp_len, "Result");	


	// copy pkt
	recvlen=resp_len;
	memcpy(recvbuf,resp,recvlen);


	/////////////////////////////////
	// RC4 decrypt pkt
	/////////////////////////////////
	show_memory(recvbuf, recvlen, "Before RC4 decrypt");	
	RC4_crypt (recvbuf, recvlen, &global->rc4_recv, 0);
	show_memory(recvbuf, recvlen, "After RC4 decrypt");	


	/////////////////////////////////
	// processing response
	/////////////////////////////////
	// by aes blocks with 109 cmd each
	i=0;j=0;
	global->confirm_count=0;
	len=resp_len;
	while(len > 0){
		j++;

		if(DEBUG_LEVEL>=100) printf("PKT BLK: %d\n\n",j);

		tmplen=get_packet_size(recvbuf+i, 4);
		tmplen=tmplen-1;
		if (tmplen > 0x1000){
			if(DEBUG_LEVEL>=100) printf("pkt block size too big, len: 0x%08X\n",tmplen);
			return -1;
		};
		if (tmplen <= 0){
			if(DEBUG_LEVEL>=100) printf("pkt block size too small, len: 0x%08X\n",tmplen);
			return -1;
		};

		show_memory(recvbuf+i, 4, "Header");//0x0b//0x08

		//get blkseq, need full aes len with crc32
		remote_blkseq=get_blkseq(recvbuf+i+4,tmplen-2);
		
		process_aes_crypt(globalptr, recvbuf+i+4, tmplen-2, 1, remote_blkseq, 1);

		//decode41(recvbuf+i+4, tmplen-2,"PKT 6D reply3");


		//processing headers, need for cmd conformation
		memcpy(header41,recvbuf+i+4,5);

		//remember first byte sess id, for send in confirmation
		if (header41[3]==0x6d){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x6d){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x4D){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[3]==0x4D){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};





		len=len-tmplen-2;
		i=i+tmplen+2;

		if(DEBUG_LEVEL>=100) printf("len(left)=%d i(bytes processed)=%d\n",len,i);
		if(DEBUG_LEVEL>=100) printf("len(current block)=%d\n",tmplen);
		if(DEBUG_LEVEL>=100) printf("confirm_count=%d\n",global->confirm_count);

	};



	return 0;
};






///////////////////////////////
//tcp seventh(7) packet
///////////////////////////////
unsigned int make_tcp_client_sess1_pkt7(_MIPD_ char *globalptr, char *pkt, int *pkt_len){
	int len;
	int tmplen;
	int blkseq;


	u8 buf1[0x1000];
	int buf1_len;
	u8 buf1header[0x10];
	int buf1header_len;

	u8 buf2[0x1000];
	int buf2_len;
	u8 buf2header[0x10];
	int buf2header_len;

	u8 buf3[0x1000];
	int buf3_len;
	u8 buf3header[0x10];
	int buf3header_len;

	u8 buf4[0x1000];
	int buf4_len;
	u8 buf4header[0x10];
	int buf4header_len;

	u8 buf5[0x1000];
	int buf5_len;
	u8 buf5header[0x10];
	int buf5header_len;


	u8 buf_newblk3[0x1000];
	int buf_newblk3_len;

	struct global_s *global;
	global=(struct global_s *)globalptr;


	
	
	if(DEBUG_LEVEL>=100) printf("Sending seventh(7) TCP packet, session cmd 109, pkt 4\n");
	


	///////////////////////////////
	// first 41
	///////////////////////////////

	memset(buf1,0,sizeof(buf1));
  	buf1_len=encode41_sesspkt_ack(globalptr, buf1, sizeof(buf1), global->confirm[0]);
	show_memory(buf1, buf1_len, "sess4pkt1ack");

	// aes encrypt block 1
	blkseq=0x11;
	buf1_len=process_aes(globalptr, buf1, buf1_len, 1, blkseq, 0);


	///////////////////////////////
	// second 41
	///////////////////////////////

	memset(buf2,0,sizeof(buf2));
  	buf2_len=encode41_sesspkt_ack(globalptr, buf2, sizeof(buf2), global->confirm[1]);
	show_memory(buf2, buf2_len, "sess4pkt2ack");

	//aes encrypt block 2
	blkseq=0x12;
	buf2_len=process_aes(globalptr, buf2, buf2_len, 1, blkseq, 0);


	
	///////////////////////////////
	// thirth block3 41
	///////////////////////////////

	// uic crc

	memset(global->NEWBLK,0,sizeof(global->NEWBLK));
	global->NEWBLK_LEN=0;

	global->NEWBLK[0]=0x6A;
    global->NEWBLK_LEN++;


	/////////////////////////////
	// SHA1 digest 0
	/////////////////////////////
	// for make digest at the _start_ of newbkl
	// crypted credentials(0x100) + chatid(0x24)
	if (1) {
		char *buf;
		char *outbuf;
		uint tlen;

		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);

		// credentials
		memcpy(buf, global->CREDENTIALS, global->CREDENTIALS_LEN);

		// + chatid 
		//memcpy(buf+4+0x100,"#xoteg_iam/$xot_iam;4fef7b015cb20ad0",0x24);
		memcpy(buf+global->CREDENTIALS_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
		tlen=global->CREDENTIALS_LEN+strlen(global->CHAT_STRING);

		// show data for hashing
		show_memory(buf, tlen, "CHATID 3 input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);

		// show 
		show_memory(outbuf, 0x14, "CHATID(hash) 3 OUTPUT");

		// copy sha1 to new blk, at start+1
		memcpy(global->NEWBLK+1,outbuf,0x14);
		global->NEWBLK_LEN+=0x14;

	};


	
	//////////////////////////////////////////////////
	// modify chatid in newblk , in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data3_newblk+0x15,"#xoteg_iam/$xot_iam;4fef7b015cb20ad0",0x24);
	//memcpy(aes_41data3_newblk+0x15,CHAT_STRING,0x24);

	memcpy(global->NEWBLK+global->NEWBLK_LEN,global->CHAT_STRING,strlen(global->CHAT_STRING));
	global->NEWBLK_LEN+=strlen(global->CHAT_STRING);

	
	memset(buf_newblk3,0,sizeof(buf_newblk3));
  	buf_newblk3_len=encode41_newblk3(globalptr, buf_newblk3, sizeof(buf_newblk3));
	show_memory(buf_newblk3, buf_newblk3_len, "buf_newblk3");


	tmplen=buf_newblk3_len;
	if(1){
		int tlen_ost;
		int tlen_need;
		int tlen_first;
		int tlen_second;

		tlen_ost=0x80-global->NEWBLK_LEN-0x15;
		tlen_need=buf_newblk3_len;

		if (tlen_ost < tlen_need){
			tlen_first=tlen_ost;
			tlen_second=tlen_need-tlen_first;
			tmplen=tlen_first;
		};
	
	};


	// middle of newblk .. some 41 data..
    memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk3,tmplen);
	global->NEWBLK_LEN+=tmplen;

	if (global->NEWBLK_LEN+0x15 != 0x80) {
			show_memory(global->NEWBLK,0x80,"newblk:");
			if(DEBUG_LEVEL>=100) printf("NEWBLK2 LEN encode error, LEN=0x%08X\n",global->NEWBLK_LEN+0x15);
			return -1;
	};

	global->NEWBLK[0x7f]=0xBC;

	
	/////////////////////////////
	// SHA1 digest 1
	/////////////////////////////
	// for make digest at the end of newblk
	// data under crypto(0x80) + cleartext data after(0x0a)
	if (1) {
		char *buf;
		char *outbuf;
		u32 tlen;


		global->NEWBLK_LEN=0x80;

		if ( tmplen!= buf_newblk3_len ){
			// aes41
			tlen=buf_newblk3_len-tmplen;
			memcpy(global->NEWBLK+global->NEWBLK_LEN,buf_newblk3+tmplen,tlen);
			global->NEWBLK_LEN+=tlen;
		};



		/*
		// message right after newblk
		memcpy(NEWBLK+NEWBLK_LEN,"\x02",1);
		NEWBLK_LEN++;

		memcpy(NEWBLK+NEWBLK_LEN,MSG_TEXT,strlen(MSG_TEXT));
		NEWBLK_LEN+=strlen(MSG_TEXT);

		memcpy(NEWBLK+NEWBLK_LEN,"\x00",1);
		NEWBLK_LEN++;
		*/


		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);


		// first char not count 
		// last 0x14 + BC is sha1 hash
		tlen=0x80-0x14-1-1;
		memcpy(buf,global->NEWBLK+1,tlen);		
		memcpy(buf+tlen,global->NEWBLK+0x80,global->NEWBLK_LEN-0x80);
		tlen=tlen+global->NEWBLK_LEN-0x80;

		// show data for hashing
		show_memory(buf, tlen, "NEWBLK 3 input");

		// making sha1 hash
		_get_sha1_data(buf,tlen,outbuf,1);


		// show 
		show_memory(outbuf, 0x14, "NEWBLK(hash) 3 OUTPUT");

		// copy sha1 to new blk, at end, before BC
		memcpy(global->NEWBLK+0x80-0x14-1,outbuf,0x14);

	};

	show_memory(global->NEWBLK, global->NEWBLK_LEN, "NEWBLK3 new OUTPUT");


	///////////////////////
	//RSA sign
	///////////////////////
	//for sign newblk with our(xoteg) private key
	if (1) {
		char *buf;
		char *outbuf;


		buf=malloc(0x1000);
		outbuf=malloc(0x1000);

		memset(buf,0,0x200);
		memset(outbuf,0,0x200);


		//copy challenge template
		memcpy(buf,global->NEWBLK,0x80);
		
		//print newblk data
		//before RSA sign-ing
		show_memory(buf, 0x80, "newblk 3 RSA SIGN input");

		//make rsa sign
		_get_sign_data(_MIPP_ globalptr, buf, 0x80, outbuf);

		////copy rsa sign to challenge_response buffer
		////for send this response in next pkt
		memcpy(global->NEWBLK,outbuf,0x80);

		//print rsa signed newblk data
		show_memory(outbuf, 0x80, "newblk 3 RSA SIGN output");

	};
	


	//////////////////////////////////////////////////
	// modify sign new block with hash on cred+chatid , in aes data
	//////////////////////////////////////////////////
	//memcpy(aes_41data3+0x35,aes_41data3_newblk,0x80);

	
	memset(buf3,0,sizeof(buf3));
  	buf3_len=encode41_sess4pkt3(globalptr, buf3, sizeof(buf3));
	show_memory(buf3, buf3_len, "sess4pkt3");

	//aes encrypt block 3
	blkseq=0x13;
	buf3_len=process_aes(globalptr, buf3, buf3_len, 1, blkseq, 0);


	///////////////////////////////
	// fourth block4 41
	///////////////////////////////

	memset(buf4,0,sizeof(buf4));
  	buf4_len=encode41_sess4pkt4(globalptr, buf4, sizeof(buf4));
	show_memory(buf4, buf4_len, "sess4pkt4");

	//aes encrypt block 4
	blkseq=0x14;
	buf4_len=process_aes(globalptr, buf4, buf4_len, 1, blkseq, 0);


	///////////////////////////////
	// block5 41
	///////////////////////////////
	
	memset(buf5,0,sizeof(buf5));
  	buf5_len=encode41_sesspkt_ack(globalptr, buf5, sizeof(buf5), global->confirm[2]);
	show_memory(buf5, buf5_len, "sess4pkt5ack");

	//aes encrypt block 4
	blkseq=0x15;
	buf5_len=process_aes(globalptr, buf5, buf5_len, 1, blkseq, 0);



	/////////////////////////////////////
	// first bytes correction, block 1
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing	

	buf1header_len=first_bytes_correction(globalptr, buf1header, sizeof(buf1header)-1, buf1, buf1_len);
	show_memory(buf1header, buf1header_len, "sess4pkt1header");

	/////////////////////////////////////
	// first bytes correction, block 2
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf2header_len=first_bytes_correction(globalptr, buf2header, sizeof(buf2header)-1, buf2, buf2_len);
	show_memory(buf2header, buf2header_len, "sess4pkt2header");

	/////////////////////////////////////
	// first bytes correction, block 3
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing
	
	buf3header_len=first_bytes_correction(globalptr, buf3header, sizeof(buf3header)-1, buf3, buf3_len);
	show_memory(buf3header, buf3header_len, "sess4pkt3header");

	/////////////////////////////////////
	// first bytes correction, block 4
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf4header_len=first_bytes_correction(globalptr, buf4header, sizeof(buf4header)-1, buf4, buf4_len);
	show_memory(buf4header, buf4header_len, "sess4pkt4header");

	/////////////////////////////////////
	// first bytes correction, block 5
	/////////////////////////////////////
	// calculate for 4 and 5 byte fixing

	buf5header_len=first_bytes_correction(globalptr, buf5header, sizeof(buf5header)-1, buf5, buf5_len);
	show_memory(buf5header, buf5header_len, "sess4pkt5header");


	/////////////////////////////////
	// assembling pkt for sending
	/////////////////////////////////
	len=0;

	//block 1
	memcpy(pkt+len,buf1header,buf1header_len);
	len=len+buf1header_len;
	memcpy(pkt+len,buf1,buf1_len);
	len=len+buf1_len;
	
	// block 2
	memcpy(pkt+len,buf2header,buf2header_len);
	len=len+buf2header_len;
	memcpy(pkt+len,buf2,buf2_len);
	len=len+buf2_len;

	// block 3
	memcpy(pkt+len,buf3header,buf3header_len);
	len=len+buf3header_len;
	memcpy(pkt+len,buf3,buf3_len);
	len=len+buf3_len;
	
	// block 4
	memcpy(pkt+len,buf4header,buf4header_len);
	len=len+buf4header_len;
	memcpy(pkt+len,buf4,buf4_len);
	len=len+buf4_len;
	
	// block 5
	memcpy(pkt+len,buf5header,buf5header_len);
	len=len+buf5header_len;
	memcpy(pkt+len,buf5,buf5_len);
	len=len+buf5_len;
	


	/////////////////////////////////
	// RC4 encrypt pkt
	/////////////////////////////////
	show_memory(pkt, len, "Before RC4 encrypt");	
	RC4_crypt (pkt, len, &global->rc4_send, 0);
	show_memory(pkt, len, "After RC4 encrypt");	


	// display pkt
	show_memory(pkt, len, "Send pkt");


	*pkt_len=len;

	return 0;

};



int process_tcp_client_sess1_pkt7(_MIPD_ char *globalptr, char *resp, int resp_len){	
	u8 recvbuf[0x1000];
	char header41[0x100];
	int i;
	int j;
	int len;
	int tmplen;
	int recvlen;
	int remote_blkseq;


	struct global_s *global;
	global=(struct global_s *)globalptr;


	// recv pkt
	show_memory(resp, resp_len, "Result");	


	// copy pkt
	recvlen=resp_len;
	memcpy(recvbuf,resp,recvlen);


	/////////////////////////////////
	// RC4 decrypt pkt
	/////////////////////////////////
	show_memory(recvbuf, recvlen, "Before RC4 decrypt");	
	RC4_crypt (recvbuf, recvlen, &global->rc4_recv, 0);
	show_memory(recvbuf, recvlen, "After RC4 decrypt");	


	/////////////////////////////////
	// processing response
	/////////////////////////////////
	// by aes blocks with 109 cmd each
	i=0;j=0;
	global->confirm_count=0;
	len=resp_len;
	while(len > 0){
		j++;

		if(DEBUG_LEVEL>=100) printf("PKT BLK: %d\n\n",j);

		tmplen=get_packet_size(recvbuf+i, 4);
		tmplen=tmplen-1;
		if (tmplen > 0x1000){
			if(DEBUG_LEVEL>=100) printf("pkt block size too big, len: 0x%08X\n",tmplen);
			return -1;
		};
		if (tmplen <= 0){
			if(DEBUG_LEVEL>=100) printf("pkt block size too small, len: 0x%08X\n",tmplen);
			return -1;
		};

		show_memory(recvbuf+i, 4, "Header");

		//get blkseq, need full aes len with crc32
		remote_blkseq=get_blkseq(recvbuf+i+4,tmplen-2);
		
		//aes decrypt
		process_aes_crypt(globalptr, recvbuf+i+4, tmplen-2, 1, remote_blkseq, 1);

		//decode 41
		//decode41(recvbuf+i+4, tmplen-2,"PKT 6D reply4");

		//processing headers, need for cmd conformation
		memcpy(header41,recvbuf+i+4,5);

		//remember first byte sess id, for send in confirmation
		if (header41[3]==0x6d){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x6d){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[2]==0x4D){
			if (header41[3]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,2);
				global->confirm_count++;
			};
		};
		if (header41[3]==0x4D){
			if (header41[4]==0x41){
				memcpy(&global->confirm[global->confirm_count],header41,3);
				global->confirm_count++;
			};
		};




		len=len-tmplen-2;
		i=i+tmplen+2;

		if(DEBUG_LEVEL>=100) printf("len(left)=%d i(bytes processed)=%d\n",len,i);
		if(DEBUG_LEVEL>=100) printf("len(current block)=%d\n",tmplen);
		if(DEBUG_LEVEL>=100) printf("confirm_count=%d\n",global->confirm_count);

	};



	return 0;
};


