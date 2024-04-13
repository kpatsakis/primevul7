bool ConstantFolding::IsReductionWithConstantIndices(
    const NodeDef& node, bool* indices_is_empty) const {
  // Ensure its an appropriate Reduce node.
  if (!IsReduction(node) || node.input_size() < 2) {
    return false;
  }
  // Ensure that the axes to reduce by are constant.
  NodeDef* reductions_indices = node_map_->GetNode(node.input(1));
  if (!IsReallyConstant(*reductions_indices) ||
      !reductions_indices->attr().count("value")) {
    return false;
  }
  const TensorShapeProto& reduction_indices_shape =
      reductions_indices->attr().at("value").tensor().tensor_shape();
  *indices_is_empty = TensorShape(reduction_indices_shape).num_elements() == 0;
  return true;
}