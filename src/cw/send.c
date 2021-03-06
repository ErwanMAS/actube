#include <errno.h>


#include "conn.h"
#include "cw.h"
#include "log.h"
#include "dbg.h"
#include "sock.h"
#include "timer.h"




int conn_send_data_msg(struct conn * conn, uint8_t *rawmsg,int len)
{
	int packetlen = len;
	int fragoffset;
	int hlen;
	
	uint8_t * ptr;
	int mtu;
	
	cw_dbg_msg(DBG_MSG_OUT, conn,rawmsg, packetlen,(struct sockaddr*)&conn->addr);


	ptr = rawmsg;

	fragoffset = 0;

	hlen = cw_get_hdr_hlen(rawmsg)*4;

	mtu = conn->mtu;

	while (packetlen>mtu){
		cw_set_hdr_flags(rawmsg,CAPWAP_FLAG_HDR_F,1);
		cw_set_dword(ptr+4, conn->fragid<<16 | fragoffset<<3 );

		cw_dbg_pkt(DBG_PKT_OUT,conn,ptr,mtu,(struct sockaddr*)&conn->addr);

/*		if (conn->write_data(conn,ptr,mtu)<0)*/
			return -1;

/*		// XXX Fragmentation stuff..*/
		ptr +=mtu-hlen;
		fragoffset+=(mtu-hlen)/8;

		packetlen-=mtu-hlen;

	}


	if (fragoffset)
		cw_set_hdr_flags(rawmsg,CAPWAP_FLAG_HDR_F | CAPWAP_FLAG_HDR_L,1);
	else
		cw_set_hdr_flags(rawmsg,CAPWAP_FLAG_HDR_F,0);

	cw_set_dword(ptr+4, conn->fragid<<16 | fragoffset<<3 );


	cw_dbg_pkt(DBG_PKT_OUT,conn,ptr,packetlen,(struct sockaddr*)&conn->addr);

/*//	return conn->write_data(conn,ptr,packetlen-0);*/

	return 0;
}







int cw_send_request(struct conn *conn,int msg_id)
{
	time_t timer;
	int i;
	int rc;
	char sock_buf[SOCK_ADDR_BUFSIZE];
	cw_init_request(conn, msg_id);
	if ( cw_put_msg(conn, conn->req_buffer) == -1 ){
		errno=ENOMSG;
		return -1;
	}
	conn_send_msg(conn, conn->req_buffer);

	

	rc=-1;
	for (i=0; i<conn->max_retransmit && rc<0; i++){
		if ( i>0 ){
			cw_log(LOG_WARNING,"Retransmitting request ... %d",i);
		}
		timer = cw_timer_start(conn->retransmit_interval);
		while (!cw_timer_timeout(timer) && rc<0){
	
        	        rc =cw_read_messages(conn);
			if(rc<0){
				if (errno!=EAGAIN)
					break;
			}

		}
		if (rc<0){
			if(errno!=EAGAIN)
				break;
			
		}

	}

	if ( rc <0 && errno != EAGAIN) {
		cw_log(LOG_ERR,"Can't read from %s: %s",sock_addr2str(&conn->addr,sock_buf),strerror(errno));
	}
	if ( rc <0 && errno == EAGAIN) {
		errno=ETIMEDOUT;
		rc=-1;
	}

	return rc;
}


/*// XXX find a better name for this function */
int cw_send_custom_request_2(struct conn *conn,int msg_id)
{




	return 0;
}

