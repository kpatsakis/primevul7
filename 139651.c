e_mail_parser_error (EMailParser *parser,
                     GQueue *out_mail_parts,
                     const gchar *format,
                     ...)
{
	const gchar *mime_type = "application/vnd.evolution.error";
	EMailPart *mail_part;
	CamelMimePart *part;
	gchar *errmsg;
	gchar *uri;
	va_list ap;

	g_return_if_fail (E_IS_MAIL_PARSER (parser));
	g_return_if_fail (out_mail_parts != NULL);
	g_return_if_fail (format != NULL);

	va_start (ap, format);
	errmsg = g_strdup_vprintf (format, ap);

	part = camel_mime_part_new ();
	camel_mime_part_set_content (
		part, errmsg, strlen (errmsg), mime_type);
	g_free (errmsg);
	va_end (ap);

	g_mutex_lock (&parser->priv->mutex);
	parser->priv->last_error++;
	uri = g_strdup_printf (".error.%d", parser->priv->last_error);
	g_mutex_unlock (&parser->priv->mutex);

	mail_part = e_mail_part_new (part, uri);
	e_mail_part_set_mime_type (mail_part, mime_type);
	mail_part->is_error = TRUE;

	g_free (uri);
	g_object_unref (part);

	g_queue_push_tail (out_mail_parts, mail_part);
}