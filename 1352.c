void ConstantFolding::ReplaceSubtractionFromZeroByNegation(NodeDef* node,
                                                           GraphDef* graph) {
  node->set_op("Neg");
  node->mutable_input()->SwapElements(0, 1);
  const string ctrl_dep =
      AddControlDependency(node->input(1), graph, node_map_.get());
  node_map_->UpdateInput(node->name(), node->input(1), ctrl_dep);
  node->set_input(1, ctrl_dep);
  graph_modified_ = true;
}