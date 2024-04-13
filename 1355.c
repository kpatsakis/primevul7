bool ConstantFolding::GetConcatAxis(const NodeDef& node, int* axis) {
  if (node.op() != "ConcatV2") {
    return false;
  }
  int axis_idx = node.input_size() - 1;
  while (axis_idx > 0 && IsControlInput(node.input(axis_idx))) {
    --axis_idx;
  }
  if (axis_idx <= 0) {
    return false;
  }
  Tensor axis_tensor;
  if (!GetTensorFromConstNode(node.input(axis_idx), &axis_tensor)) {
    return false;
  }
  *axis = axis_tensor.dtype() == DT_INT64
              ? static_cast<int>(axis_tensor.scalar<int64_t>()())
              : axis_tensor.scalar<int32>()();
  return true;
}