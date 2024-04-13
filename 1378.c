bool ConstantFolding::IsZeros(const NodeDef& node) const {
  if (feed_nodes_.find(node.name()) != feed_nodes_.end()) {
    return false;
  }
  if (IsOnesLike(node)) return false;
  if (IsZerosLike(node)) return true;
  if (node.op() == "Fill") {
    NodeDef* values = node_map_->GetNode(NodeName(node.input(1)));
    return values != nullptr && IsZeros(*values);
  }
  if (!IsConstant(node)) return false;
  if (node.attr().count("dtype") == 0) return false;
  const auto dtype = node.attr().at("dtype").type();
  switch (dtype) {
    IS_ZEROS_CASE(DT_BOOL);
    IS_ZEROS_CASE(DT_HALF);
    IS_ZEROS_CASE(DT_BFLOAT16);
    IS_ZEROS_CASE(DT_FLOAT);
    IS_ZEROS_CASE(DT_DOUBLE);
    IS_ZEROS_CASE(DT_COMPLEX64);
    IS_ZEROS_CASE(DT_COMPLEX128);
    IS_ZEROS_CASE(DT_UINT8);
    IS_ZEROS_CASE(DT_INT8);
    IS_ZEROS_CASE(DT_UINT16);
    IS_ZEROS_CASE(DT_INT16);
    IS_ZEROS_CASE(DT_INT32);
    IS_ZEROS_CASE(DT_INT64);
    IS_ZEROS_CASE(DT_QINT32);
    IS_ZEROS_CASE(DT_QINT16);
    IS_ZEROS_CASE(DT_QUINT16);
    IS_ZEROS_CASE(DT_QINT8);
    IS_ZEROS_CASE(DT_QUINT8);
    default:
      VLOG(1) << "Unsupported type " << DataTypeString(dtype);
      return false;
  }
  return false;
}