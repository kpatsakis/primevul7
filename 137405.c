static void call_nt_transact_ioctl(connection_struct *conn,
				   struct smb_request *req,
				   uint16 **ppsetup, uint32 setup_count,
				   char **ppparams, uint32 parameter_count,
				   char **ppdata, uint32 data_count,
				   uint32 max_data_count)
{
	uint32 function;
	uint16 fidnum;
	files_struct *fsp;
	uint8 isFSctl;
	uint8 compfilter;
	char *pdata = *ppdata;

	if (setup_count != 8) {
		DEBUG(3,("call_nt_transact_ioctl: invalid setup count %d\n", setup_count));
		reply_nterror(req, NT_STATUS_NOT_SUPPORTED);
		return;
	}

	function = IVAL(*ppsetup, 0);
	fidnum = SVAL(*ppsetup, 4);
	isFSctl = CVAL(*ppsetup, 6);
	compfilter = CVAL(*ppsetup, 7);

	DEBUG(10,("call_nt_transact_ioctl: function[0x%08X] FID[0x%04X] isFSctl[0x%02X] compfilter[0x%02X]\n", 
		 function, fidnum, isFSctl, compfilter));

	fsp=file_fsp(req, fidnum);
	/* this check is done in each implemented function case for now
	   because I don't want to break anything... --metze
	FSP_BELONGS_CONN(fsp,conn);*/

	SMB_PERFCOUNT_SET_IOCTL(&req->pcd, function);

	switch (function) {
	case FSCTL_SET_SPARSE:
		/* pretend this succeeded - tho strictly we should
		   mark the file sparse (if the local fs supports it)
		   so we can know if we need to pre-allocate or not */

		DEBUG(10,("FSCTL_SET_SPARSE: called on FID[0x%04X](but not implemented)\n", fidnum));
		send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0, NULL, 0);
		return;

	case FSCTL_CREATE_OR_GET_OBJECT_ID:
	{
		unsigned char objid[16];

		/* This should return the object-id on this file.
		 * I think I'll make this be the inode+dev. JRA.
		 */

		DEBUG(10,("FSCTL_CREATE_OR_GET_OBJECT_ID: called on FID[0x%04X]\n",fidnum));

		if (!fsp_belongs_conn(conn, req, fsp)) {
			return;
		}

		data_count = 64;
		pdata = nttrans_realloc(ppdata, data_count);
		if (pdata == NULL) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}

		/* For backwards compatibility only store the dev/inode. */
		push_file_id_16(pdata, &fsp->file_id);
		memcpy(pdata+16,create_volume_objectid(conn,objid),16);
		push_file_id_16(pdata+32, &fsp->file_id);
		send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0,
				pdata, data_count);
		return;
	}

	case FSCTL_GET_REPARSE_POINT:
		/* pretend this fail - my winXP does it like this
		 * --metze
		 */

		DEBUG(10,("FSCTL_GET_REPARSE_POINT: called on FID[0x%04X](but not implemented)\n",fidnum));
		reply_nterror(req, NT_STATUS_NOT_A_REPARSE_POINT);
		return;

	case FSCTL_SET_REPARSE_POINT:
		/* pretend this fail - I'm assuming this because of the FSCTL_GET_REPARSE_POINT case.
		 * --metze
		 */

		DEBUG(10,("FSCTL_SET_REPARSE_POINT: called on FID[0x%04X](but not implemented)\n",fidnum));
		reply_nterror(req, NT_STATUS_NOT_A_REPARSE_POINT);
		return;

	case FSCTL_GET_SHADOW_COPY_DATA: /* don't know if this name is right...*/
	{
		/*
		 * This is called to retrieve the number of Shadow Copies (a.k.a. snapshots)
		 * and return their volume names.  If max_data_count is 16, then it is just
		 * asking for the number of volumes and length of the combined names.
		 *
		 * pdata is the data allocated by our caller, but that uses
		 * total_data_count (which is 0 in our case) rather than max_data_count.
		 * Allocate the correct amount and return the pointer to let
		 * it be deallocated when we return.
		 */
		SHADOW_COPY_DATA *shadow_data = NULL;
		TALLOC_CTX *shadow_mem_ctx = NULL;
		bool labels = False;
		uint32 labels_data_count = 0;
		uint32 i;
		char *cur_pdata;

		if (!fsp_belongs_conn(conn, req, fsp)) {
			return;
		}

		if (max_data_count < 16) {
			DEBUG(0,("FSCTL_GET_SHADOW_COPY_DATA: max_data_count(%u) < 16 is invalid!\n",
				max_data_count));
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		if (max_data_count > 16) {
			labels = True;
		}

		shadow_mem_ctx = talloc_init("SHADOW_COPY_DATA");
		if (shadow_mem_ctx == NULL) {
			DEBUG(0,("talloc_init(SHADOW_COPY_DATA) failed!\n"));
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}

		shadow_data = TALLOC_ZERO_P(shadow_mem_ctx,SHADOW_COPY_DATA);
		if (shadow_data == NULL) {
			DEBUG(0,("TALLOC_ZERO() failed!\n"));
			talloc_destroy(shadow_mem_ctx);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}

		shadow_data->mem_ctx = shadow_mem_ctx;

		/*
		 * Call the VFS routine to actually do the work.
		 */
		if (SMB_VFS_GET_SHADOW_COPY_DATA(fsp, shadow_data, labels)!=0) {
			talloc_destroy(shadow_data->mem_ctx);
			if (errno == ENOSYS) {
				DEBUG(5,("FSCTL_GET_SHADOW_COPY_DATA: connectpath %s, not supported.\n", 
					conn->connectpath));
				reply_nterror(req, NT_STATUS_NOT_SUPPORTED);
				return;
			} else {
				DEBUG(0,("FSCTL_GET_SHADOW_COPY_DATA: connectpath %s, failed.\n", 
					conn->connectpath));
				reply_nterror(req, NT_STATUS_UNSUCCESSFUL);
				return;
			}
		}

		labels_data_count = (shadow_data->num_volumes*2*sizeof(SHADOW_COPY_LABEL))+2;

		if (!labels) {
			data_count = 16;
		} else {
			data_count = 12+labels_data_count+4;
		}

		if (max_data_count<data_count) {
			DEBUG(0,("FSCTL_GET_SHADOW_COPY_DATA: max_data_count(%u) too small (%u) bytes needed!\n",
				max_data_count,data_count));
			talloc_destroy(shadow_data->mem_ctx);
			reply_nterror(req, NT_STATUS_BUFFER_TOO_SMALL);
			return;
		}

		pdata = nttrans_realloc(ppdata, data_count);
		if (pdata == NULL) {
			talloc_destroy(shadow_data->mem_ctx);
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}

		cur_pdata = pdata;

		/* num_volumes 4 bytes */
		SIVAL(pdata,0,shadow_data->num_volumes);

		if (labels) {
			/* num_labels 4 bytes */
			SIVAL(pdata,4,shadow_data->num_volumes);
		}

		/* needed_data_count 4 bytes */
		SIVAL(pdata, 8, labels_data_count+4);

		cur_pdata+=12;

		DEBUG(10,("FSCTL_GET_SHADOW_COPY_DATA: %u volumes for path[%s].\n",
			  shadow_data->num_volumes, fsp_str_dbg(fsp)));
		if (labels && shadow_data->labels) {
			for (i=0;i<shadow_data->num_volumes;i++) {
				srvstr_push(pdata, req->flags2,
					    cur_pdata, shadow_data->labels[i],
					    2*sizeof(SHADOW_COPY_LABEL),
					    STR_UNICODE|STR_TERMINATE);
				cur_pdata+=2*sizeof(SHADOW_COPY_LABEL);
				DEBUGADD(10,("Label[%u]: '%s'\n",i,shadow_data->labels[i]));
			}
		}

		talloc_destroy(shadow_data->mem_ctx);

		send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0,
				pdata, data_count);

		return;
        }

	case FSCTL_FIND_FILES_BY_SID: /* I hope this name is right */
	{
		/* pretend this succeeded -
		 *
		 * we have to send back a list with all files owned by this SID
		 *
		 * but I have to check that --metze
		 */
		DOM_SID sid;
		uid_t uid;
		size_t sid_len = MIN(data_count-4,SID_MAX_SIZE);

		DEBUG(10,("FSCTL_FIND_FILES_BY_SID: called on FID[0x%04X]\n",fidnum));

		if (!fsp_belongs_conn(conn, req, fsp)) {
			return;
		}

		/* unknown 4 bytes: this is not the length of the sid :-(  */
		/*unknown = IVAL(pdata,0);*/

		if (!sid_parse(pdata+4,sid_len,&sid)) {
			reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return;
		}

		DEBUGADD(10, ("for SID: %s\n", sid_string_dbg(&sid)));

		if (!sid_to_uid(&sid, &uid)) {
			DEBUG(0,("sid_to_uid: failed, sid[%s] sid_len[%lu]\n",
				 sid_string_dbg(&sid),
				 (unsigned long)sid_len));
			uid = (-1);
		}

		/* we can take a look at the find source :-)
		 *
		 * find ./ -uid $uid  -name '*'   is what we need here
		 *
		 *
		 * and send 4bytes len and then NULL terminated unicode strings
		 * for each file
		 *
		 * but I don't know how to deal with the paged results
		 * (maybe we can hang the result anywhere in the fsp struct)
		 *
		 * we don't send all files at once
		 * and at the next we should *not* start from the beginning,
		 * so we have to cache the result
		 *
		 * --metze
		 */

		/* this works for now... */
		send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0, NULL, 0);
		return;
	}
	default:
		if (!logged_ioctl_message) {
			logged_ioctl_message = true; /* Only print this once... */
			DEBUG(0,("call_nt_transact_ioctl(0x%x): Currently not implemented.\n",
				 function));
		}
	}

	reply_nterror(req, NT_STATUS_NOT_SUPPORTED);
}