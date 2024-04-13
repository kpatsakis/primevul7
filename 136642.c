XML_SetCommentHandler(XML_Parser parser,
                      XML_CommentHandler handler)
{
  commentHandler = handler;
}