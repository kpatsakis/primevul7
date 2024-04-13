soup_server_class_init (SoupServerClass *server_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (server_class);

	g_type_class_add_private (server_class, sizeof (SoupServerPrivate));

	/* virtual method override */
	object_class->constructor = constructor;
	object_class->finalize = finalize;
	object_class->set_property = set_property;
	object_class->get_property = get_property;

	/* signals */

	/**
	 * SoupServer::request-started
	 * @server: the server
	 * @message: the new message
	 * @client: the client context
	 *
	 * Emitted when the server has started reading a new request.
	 * @message will be completely blank; not even the
	 * Request-Line will have been read yet. About the only thing
	 * you can usefully do with it is connect to its signals.
	 *
	 * If the request is read successfully, this will eventually
	 * be followed by a #SoupServer::request_read signal. If a
	 * response is then sent, the request processing will end with
	 * a #SoupServer::request_finished signal. If a network error
	 * occurs, the processing will instead end with
	 * #SoupServer::request_aborted.
	 **/
	signals[REQUEST_STARTED] =
		g_signal_new ("request-started",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (SoupServerClass, request_started),
			      NULL, NULL,
			      soup_marshal_NONE__OBJECT_POINTER,
			      G_TYPE_NONE, 2, 
			      SOUP_TYPE_MESSAGE,
			      SOUP_TYPE_CLIENT_CONTEXT);

	/**
	 * SoupServer::request-read
	 * @server: the server
	 * @message: the message
	 * @client: the client context
	 *
	 * Emitted when the server has successfully read a request.
	 * @message will have all of its request-side information
	 * filled in, and if the message was authenticated, @client
	 * will have information about that. This signal is emitted
	 * before any handlers are called for the message, and if it
	 * sets the message's #status_code, then normal handler
	 * processing will be skipped.
	 **/
	signals[REQUEST_READ] =
		g_signal_new ("request-read",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (SoupServerClass, request_read),
			      NULL, NULL,
			      soup_marshal_NONE__OBJECT_POINTER,
			      G_TYPE_NONE, 2,
			      SOUP_TYPE_MESSAGE,
			      SOUP_TYPE_CLIENT_CONTEXT);

	/**
	 * SoupServer::request-finished
	 * @server: the server
	 * @message: the message
	 * @client: the client context
	 *
	 * Emitted when the server has finished writing a response to
	 * a request.
	 **/
	signals[REQUEST_FINISHED] =
		g_signal_new ("request-finished",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (SoupServerClass, request_finished),
			      NULL, NULL,
			      soup_marshal_NONE__OBJECT_POINTER,
			      G_TYPE_NONE, 2,
			      SOUP_TYPE_MESSAGE,
			      SOUP_TYPE_CLIENT_CONTEXT);

	/**
	 * SoupServer::request-aborted
	 * @server: the server
	 * @message: the message
	 * @client: the client context
	 *
	 * Emitted when processing has failed for a message; this
	 * could mean either that it could not be read (if
	 * #SoupServer::request_read has not been emitted for it yet),
	 * or that the response could not be written back (if
	 * #SoupServer::request_read has been emitted but
	 * #SoupServer::request_finished has not been).
	 *
	 * @message is in an undefined state when this signal is
	 * emitted; the signal exists primarily to allow the server to
	 * free any state that it may have allocated in
	 * #SoupServer::request_started.
	 **/
	signals[REQUEST_ABORTED] =
		g_signal_new ("request-aborted",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (SoupServerClass, request_aborted),
			      NULL, NULL,
			      soup_marshal_NONE__OBJECT_POINTER,
			      G_TYPE_NONE, 2,
			      SOUP_TYPE_MESSAGE,
			      SOUP_TYPE_CLIENT_CONTEXT);

	/* properties */
	/**
	 * SOUP_SERVER_PORT:
	 *
	 * Alias for the #SoupServer:port property. (The port the
	 * server listens on.)
	 **/
	g_object_class_install_property (
		object_class, PROP_PORT,
		g_param_spec_uint (SOUP_SERVER_PORT,
				   "Port",
				   "Port to listen on",
				   0, 65536, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	/**
	 * SOUP_SERVER_INTERFACE:
	 *
	 * Alias for the #SoupServer:interface property. (The address
	 * of the network interface the server listens on.)
	 **/
	g_object_class_install_property (
		object_class, PROP_INTERFACE,
		g_param_spec_object (SOUP_SERVER_INTERFACE,
				     "Interface",
				     "Address of interface to listen on",
				     SOUP_TYPE_ADDRESS,
				     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	/**
	 * SOUP_SERVER_SSL_CERT_FILE:
	 *
	 * Alias for the #SoupServer:ssl-cert-file property. (The file
	 * containing the SSL certificate for the server.)
	 **/
	g_object_class_install_property (
		object_class, PROP_SSL_CERT_FILE,
		g_param_spec_string (SOUP_SERVER_SSL_CERT_FILE,
				     "SSL certificate file",
				     "File containing server SSL certificate",
				     NULL,
				     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	/**
	 * SOUP_SERVER_SSL_KEY_FILE:
	 *
	 * Alias for the #SoupServer:ssl-key-file property. (The file
	 * containing the SSL certificate key for the server.)
	 **/
	g_object_class_install_property (
		object_class, PROP_SSL_KEY_FILE,
		g_param_spec_string (SOUP_SERVER_SSL_KEY_FILE,
				     "SSL key file",
				     "File containing server SSL key",
				     NULL,
				     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	/**
	 * SOUP_SERVER_ASYNC_CONTEXT:
	 *
	 * Alias for the #SoupServer:async-context property. (The
	 * server's #GMainContext.)
	 **/
	g_object_class_install_property (
		object_class, PROP_ASYNC_CONTEXT,
		g_param_spec_pointer (SOUP_SERVER_ASYNC_CONTEXT,
				      "Async GMainContext",
				      "The GMainContext to dispatch async I/O in",
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	/**
	 * SOUP_SERVER_RAW_PATHS:
	 *
	 * Alias for the #SoupServer:raw-paths property. (If %TRUE,
	 * percent-encoding in the Request-URI path will not be
	 * automatically decoded.)
	 **/
	g_object_class_install_property (
		object_class, PROP_RAW_PATHS,
		g_param_spec_boolean (SOUP_SERVER_RAW_PATHS,
				      "Raw paths",
				      "If %TRUE, percent-encoding in the Request-URI path will not be automatically decoded.",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * SoupServer:server-header:
	 *
	 * If non-%NULL, the value to use for the "Server" header on
	 * #SoupMessage<!-- -->s processed by this server.
	 *
	 * The Server header is the server equivalent of the
	 * User-Agent header, and provides information about the
	 * server and its components. It contains a list of one or
	 * more product tokens, separated by whitespace, with the most
	 * significant product token coming first. The tokens must be
	 * brief, ASCII, and mostly alphanumeric (although "-", "_",
	 * and "." are also allowed), and may optionally include a "/"
	 * followed by a version string. You may also put comments,
	 * enclosed in parentheses, between or after the tokens.
	 *
	 * Some HTTP server implementations intentionally do not use
	 * version numbers in their Server header, so that
	 * installations running older versions of the server don't
	 * end up advertising their vulnerability to specific security
	 * holes.
	 *
	 * As with #SoupSession:user_agent, if you set a
	 * %server_header property that has trailing whitespace,
	 * #SoupServer will append its own product token (eg,
	 * "<literal>libsoup/2.3.2</literal>") to the end of the
	 * header for you.
	 **/
	/**
	 * SOUP_SERVER_SERVER_HEADER:
	 *
	 * Alias for the #SoupServer:server-header property, qv.
	 **/
	g_object_class_install_property (
		object_class, PROP_SERVER_HEADER,
		g_param_spec_string (SOUP_SERVER_SERVER_HEADER,
				     "Server header",
				     "Server header",
				     NULL,
				     G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}