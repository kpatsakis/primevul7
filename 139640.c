mail_parser_move_security_before_headers (GQueue *part_queue)
{
	GList *link, *last_headers = NULL;
	GSList *headers_stack = NULL;

	link = g_queue_peek_head_link (part_queue);
	while (link) {
		EMailPart *part = link->data;
		const gchar *id;

		if (!part) {
			link = g_list_next (link);
			continue;
		}

		id = e_mail_part_get_id (part);
		if (!id) {
			link = g_list_next (link);
			continue;
		}

		if (g_str_has_suffix (id, ".rfc822")) {
			headers_stack = g_slist_prepend (headers_stack, last_headers);
			last_headers = NULL;
		} else if (g_str_has_suffix (id, ".rfc822.end")) {
			g_warn_if_fail (headers_stack != NULL);

			if (headers_stack) {
				last_headers = headers_stack->data;
				headers_stack = g_slist_remove (headers_stack, last_headers);
			} else {
				last_headers = NULL;
			}
		}

		if (g_strcmp0 (e_mail_part_get_mime_type (part), "application/vnd.evolution.headers") == 0) {
			last_headers = link;
			link = g_list_next (link);
		} else if (g_strcmp0 (e_mail_part_get_mime_type (part), "application/vnd.evolution.secure-button") == 0) {
			g_warn_if_fail (last_headers != NULL);

			if (last_headers) {
				GList *next = g_list_next (link);

				g_warn_if_fail (g_queue_remove (part_queue, part));
				g_queue_insert_before (part_queue, last_headers, part);

				link = next;
			} else {
				link = g_list_next (link);
			}
		} else {
			link = g_list_next (link);
		}
	}

	g_warn_if_fail (headers_stack == NULL);
	g_slist_free (headers_stack);
}