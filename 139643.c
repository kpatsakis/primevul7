e_mail_parser_base_init (EMailParserClass *class)
{
	EShell *shell;

	/* Register internal extensions. */
	g_type_ensure (e_mail_parser_application_mbox_get_type ());
	/* This is currently disabled, because the WebKit player requires javascript,
	   which is disabled in Evolution. */
	/* g_type_ensure (e_mail_parser_audio_get_type ()); */
	g_type_ensure (e_mail_parser_headers_get_type ());
	g_type_ensure (e_mail_parser_message_get_type ());
	g_type_ensure (e_mail_parser_secure_button_get_type ());
	g_type_ensure (e_mail_parser_source_get_type ());
	g_type_ensure (e_mail_parser_image_get_type ());
	g_type_ensure (e_mail_parser_inline_pgp_encrypted_get_type ());
	g_type_ensure (e_mail_parser_inline_pgp_signed_get_type ());
	g_type_ensure (e_mail_parser_message_delivery_status_get_type ());
	g_type_ensure (e_mail_parser_message_external_get_type ());
	g_type_ensure (e_mail_parser_message_rfc822_get_type ());
	g_type_ensure (e_mail_parser_multipart_alternative_get_type ());
	g_type_ensure (e_mail_parser_multipart_apple_double_get_type ());
	g_type_ensure (e_mail_parser_multipart_digest_get_type ());
	g_type_ensure (e_mail_parser_multipart_encrypted_get_type ());
	g_type_ensure (e_mail_parser_multipart_mixed_get_type ());
	g_type_ensure (e_mail_parser_multipart_related_get_type ());
	g_type_ensure (e_mail_parser_multipart_signed_get_type ());
	g_type_ensure (e_mail_parser_text_enriched_get_type ());
	g_type_ensure (e_mail_parser_text_html_get_type ());
	g_type_ensure (e_mail_parser_text_plain_get_type ());
#ifdef ENABLE_SMIME
	g_type_ensure (e_mail_parser_application_smime_get_type ());
#endif

	class->extension_registry = g_object_new (
		E_TYPE_MAIL_PARSER_EXTENSION_REGISTRY, NULL);

	e_mail_parser_extension_registry_load (class->extension_registry);

	e_extensible_load_extensions (E_EXTENSIBLE (class->extension_registry));

	shell = e_shell_get_default ();
	g_object_weak_ref (G_OBJECT (shell), shell_gone_cb, class);
}