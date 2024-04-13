bool ConstantFolding::GetTensorFromConstNode(const string& node_name_or_input,
                                             Tensor* tensor) {
  const NodeDef* node = node_map_->GetNode(node_name_or_input);
  return node != nullptr && IsReallyConstant(*node) &&
         CheckAttrExists(*node, "value").ok() &&
         tensor->FromProto(node->attr().at("value").tensor());
}