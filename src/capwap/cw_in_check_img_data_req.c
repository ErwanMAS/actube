#include <stdio.h>
#include <errno.h>

#include "capwap.h"
#include "dbg.h"
#include "cw_log.h"
#include "capwap_items.h"


int cw_in_check_img_data_req(struct conn *conn, struct cw_action_in *a, uint8_t * data,
			 int len)
{
	/* Check for mandatory elements */
	cw_action_in_t * mlist[60];
	int n = cw_check_missing_mand(mlist,conn,a);
	if (n) {
		cw_dbg_missing_mand(DBG_ELEM,conn,mlist,n,a);
		conn->capwap_state=CW_STATE_JOIN;
		return CW_RESULT_MISSING_MAND_ELEM;
	}

	
	struct cw_item *i = cw_itemstore_get(conn->remote,CW_ITEM_IMAGE_IDENTIFIER);
	if (i) {
		uint32_t vendor_id = vendorstr_get_vendor_id(i->data);

		const char * image_dir = cw_itemstore_get_str(conn->local,CW_ITEM_AC_IMAGE_DIR);

		char * image_filename = malloc(6+vendorstr_len(i->data)+1+strlen(image_dir));
		if (!image_filename) 
			return CW_RESULT_IMAGE_DATA_OTHER_ERROR;

		sprintf(image_filename,"%s%04X/%s",image_dir,vendor_id,vendorstr_data(i->data));


		FILE *infile = fopen(image_filename,"rb");
		if (!infile){
			cw_log(LOG_WARNING,"Can't open image file: %s - %s - requestet by WTP",
				image_filename,strerror(errno));
			free(image_filename);
			return CW_RESULT_IMAGE_DATA_OTHER_ERROR;
		}

		cw_itemstore_set_str(conn->remote,CW_ITEM_WTP_IMAGE_FILENAME,image_filename);

	

	}

	
	/* set result code to ok and change to configure state */
//	cw_itemstore_set_dword(conn->local,CW_ITEM_RESULT_CODE,0);
//	conn->capwap_state = CW_STATE_CONFIGURE;

	cw_itemstore_set_dword(conn->local,CW_ITEM_RESULT_CODE,0);	
	conn->capwap_state=CW_STATE_IMAGE_DATA;

	return 0;

}
