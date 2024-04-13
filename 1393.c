bool ConstantFolding::SimplifyReshape(const GraphProperties& properties,
                                      bool use_shape_info, NodeDef* node) {
  if (!use_shape_info || node->attr().count("T") == 0 ||
      !IsSimplifiableReshape(*node, properties).ok()) {
    return false;
  }
  DataType output_type = node->attr().at("T").type();
  node->set_op("Identity");
  EraseRegularNodeAttributes(node);
  (*node->mutable_attr())["T"].set_type(output_type);
  *node->mutable_input(1) = AsControlDependency(node->input(1));
  return true;
}