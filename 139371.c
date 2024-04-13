void gg_event_free(struct gg_event *e)
{
	gg_debug(GG_DEBUG_FUNCTION, "** gg_event_free(%p);\n", e);

	if (!e)
		return;

	switch (e->type) {
		case GG_EVENT_MSG:
		case GG_EVENT_MULTILOGON_MSG:
			free(e->event.msg.message);
			free(e->event.msg.formats);
			free(e->event.msg.recipients);
			free(e->event.msg.xhtml_message);
			break;

		case GG_EVENT_NOTIFY:
			free(e->event.notify);
			break;

		case GG_EVENT_NOTIFY60:
		{
			int i;

			for (i = 0; e->event.notify60[i].uin; i++)
				free(e->event.notify60[i].descr);

			free(e->event.notify60);

			break;
		}

		case GG_EVENT_STATUS60:
			free(e->event.status60.descr);
			break;

		case GG_EVENT_STATUS:
			free(e->event.status.descr);
			break;

		case GG_EVENT_NOTIFY_DESCR:
			free(e->event.notify_descr.notify);
			free(e->event.notify_descr.descr);
			break;

		case GG_EVENT_DCC_VOICE_DATA:
			free(e->event.dcc_voice_data.data);
			break;

		case GG_EVENT_PUBDIR50_SEARCH_REPLY:
		case GG_EVENT_PUBDIR50_READ:
		case GG_EVENT_PUBDIR50_WRITE:
			gg_pubdir50_free(e->event.pubdir50);
			break;

		case GG_EVENT_USERLIST:
			free(e->event.userlist.reply);
			break;

		case GG_EVENT_IMAGE_REPLY:
			free(e->event.image_reply.filename);
			free(e->event.image_reply.image);
			break;

		case GG_EVENT_XML_EVENT:
			free(e->event.xml_event.data);
			break;

		case GG_EVENT_JSON_EVENT:
			free(e->event.json_event.data);
			free(e->event.json_event.type);
			break;

		case GG_EVENT_USER_DATA:
		{
			unsigned int i, j;

			for (i = 0; i < e->event.user_data.user_count; i++) {
				for (j = 0; j < e->event.user_data.users[i].attr_count; j++) {
					free(e->event.user_data.users[i].attrs[j].key);
					free(e->event.user_data.users[i].attrs[j].value);
				}

				free(e->event.user_data.users[i].attrs);
			}

			free(e->event.user_data.users);

			break;
		}
	
		case GG_EVENT_MULTILOGON_INFO:
		{
			int i;

			for (i = 0; i < e->event.multilogon_info.count; i++)
				free(e->event.multilogon_info.sessions[i].name);

			free(e->event.multilogon_info.sessions);

			break;
		}

		case GG_EVENT_USERLIST100_REPLY:
			free(e->event.userlist100_reply.reply);
			break;

		case GG_EVENT_IMTOKEN:
			free(e->event.imtoken.imtoken);
			break;

		case GG_EVENT_CHAT_INFO:
			free(e->event.chat_info.participants);
			break;
	}

	free(e);
}