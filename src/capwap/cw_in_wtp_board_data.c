
#include <stdlib.h>
#include <string.h>


#include "wtpinfo.h"

#include "capwap.h"
#include "capwap_items.h"

#include "itemstore.h"

#include "cw_util.h"
#include "cw_log.h"



static void readsubelems_wtp_board_data(cw_itemstore_t itemstore, uint8_t * msgelem,
						int len)
{

	int i = 0;
	uint32_t val;
	do {
		val = ntohl(*((uint32_t *) (msgelem + i)));
		int subtype = (val >> 16) & 0xffff;
		int sublen = val & 0xffff;
		i += 4;
		if (sublen + i > len) {
			cw_dbg(DBG_ELEM, "WTP Board data sub-element too long, type=%d,len=%d",
			       subtype, sublen);
			return;
		}

		cw_dbg(DBG_ELEM, "Reading WTP board data sub-element, type=%d, len=%d", subtype,
		       sublen);

		switch (subtype) {
			case CWBOARDDATA_MODELNO:
				cw_itemstore_set_bstrn(itemstore, CW_ITEM_WTP_BOARD_MODELNO,
						       msgelem + i, sublen);
				break;
			case CWBOARDDATA_SERIALNO:
				cw_itemstore_set_bstrn(itemstore, CW_ITEM_WTP_BOARD_SERIALNO,
						       msgelem + i, sublen);
				
				break;
			case CWBOARDDATA_MACADDRESS:
				cw_itemstore_set_bstrn(itemstore, CW_ITEM_WTP_BOARD_MACADDRESS,
						       msgelem + i, sublen);
				
				break;
			case CWBOARDDATA_BOARDID:
				cw_itemstore_set_bstrn(itemstore, CW_ITEM_WTP_BOARD_ID,
						       msgelem + i, sublen);
				break;
			case CWBOARDDATA_REVISION:
				cw_itemstore_set_bstrn(itemstore, CW_ITEM_WTP_BOARD_REVISION,
						       msgelem + i, sublen);
			default:
				break;
		}
		i += sublen;

	} while (i < len);
}


/**
 * Parse a WTP Board Data messag element an put results to itemstore.
 */ 
int cw_in_wtp_board_data(struct conn *conn, struct cw_action *a, uint8_t * data, int len)
{
	printf("Jau Board Data\n");
	if (len < 4) {
		cw_dbg(DBG_ELEM_ERR,
		       "Discarding WTP_BOARD_DATA msgelem, wrong size, type=%d, len=%d", a->elem_id,
		       len);
		return 1;
	}

	cw_itemstore_t itemstore = conn->itemstore;	
	cw_itemstore_set_dword(itemstore, CW_ITEM_WTP_BOARD_VENDOR,cw_get_dword(data));

	readsubelems_wtp_board_data(itemstore,data+4,len-4);
	return 1;
}
