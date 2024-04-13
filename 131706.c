RGWOp *RGWHandler_REST_Service_S3::op_post()
{
  const auto max_size = s->cct->_conf->rgw_max_put_param_size;

  int ret;
  bufferlist data;
  std::tie(ret, data) = rgw_rest_read_all_input(s, max_size, false);
  if (ret < 0) {
      return nullptr;
  }

  const auto post_body = data.to_str();

  if (isSTSEnabled) {
    RGWHandler_REST_STS sts_handler(auth_registry, post_body);
    sts_handler.init(store, s, s->cio);
    auto op = sts_handler.get_op();
    if (op) {
      return op;
    }
  }

  if (isIAMEnabled) {
    RGWHandler_REST_IAM iam_handler(auth_registry, post_body);
    iam_handler.init(store, s, s->cio);
    auto op = iam_handler.get_op();
    if (op) {
      return op;
    }
  }

  if (isPSEnabled) {
    RGWHandler_REST_PSTopic_AWS topic_handler(auth_registry, post_body);
    topic_handler.init(store, s, s->cio);
    auto op = topic_handler.get_op();
    if (op) {
      return op;
    }
  }

  return nullptr;
}