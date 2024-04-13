e_mail_parser_wrap_as_non_expandable_attachment (EMailParser *parser,
						 CamelMimePart *part,
						 GString *part_id,
						 GQueue *out_parts_queue)
{
	GQueue work_queue = G_QUEUE_INIT;
	GList *head, *link;

	g_return_if_fail (E_IS_MAIL_PARSER (parser));
	g_return_if_fail (CAMEL_IS_MIME_PART (part));
	g_return_if_fail (part_id != NULL);
	g_return_if_fail (out_parts_queue != NULL);

	e_mail_parser_wrap_as_attachment (parser, part, part_id, &work_queue);

	head = g_queue_peek_head_link (&work_queue);

	for (link = head; link; link = g_list_next (link)) {
		EMailPartAttachment *empa = link->data;

		if (E_IS_MAIL_PART_ATTACHMENT (empa)) {
			EAttachment *attachment;
			CamelMimePart *att_part;

			empa->shown = FALSE;
			e_mail_part_attachment_set_expandable (empa, FALSE);

			attachment = e_mail_part_attachment_ref_attachment (empa);
			e_attachment_set_initially_shown (attachment, FALSE);
			e_attachment_set_can_show (attachment, FALSE);

			att_part = e_attachment_ref_mime_part (attachment);
			if (att_part)
				camel_mime_part_set_disposition (att_part, NULL);

			g_clear_object (&att_part);
			g_clear_object (&attachment);
		}
	}

	e_queue_transfer (&work_queue, out_parts_queue);
}