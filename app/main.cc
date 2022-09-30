// main.cc
// Synopsis: Entry point for the executable

#include <ilang/ilang++.h>
#include <ilang/target-json/interface.h>
#include <iostream>
#include "z3++.h"
#include <helper/lib.h>
#include <ilang/vtarget-out/vtarget_gen.h>
#include <sys/resource.h>

using namespace ilang;
using std::cout;
using std::endl;


void add_input(InstrLvlAbsPtr src, InstrLvlAbsPtr dest) {
  for (size_t i = 0; i != src->input_num(); i++) {
    if ((dest->find_input(src->input(i)->name()).get()) ==nullptr)
      dest->AddInput(src->input(i));
  }
}

void add_input(InstrLvlAbsPtr dest, CustExprSet& inputs) {
  for (auto input : inputs) {
    dest->AddInput(input);
  }
}

void add_state(InstrLvlAbsPtr src, InstrLvlAbsPtr dest) {
  for (size_t i = 0; i != src->state_num(); i++) {
    if ((dest->find_state(src->state(i)->name()).get()) == nullptr)
      dest->AddState(src->state(i));
  }
}
 
int main() {
  SetUnsignedComparison(true);
  auto proc_file = "./oc8051.json";
  auto proc_ila = IlaSerDesMngr::DesFromFile(proc_file);

  std::vector<ExprPtr> tmp_iram_elements;
  ModifyIramInterface(proc_ila, tmp_iram_elements);
  
  AddIromInputPort(proc_ila);
  AddXramOutPort(proc_ila);
  for (int i = 0; i < 255; i++) {
    Model8051RemapMemInterface(proc_ila, tmp_iram_elements, i);
  }

  VerilogGeneratorBase::VlgGenConfig vlg_cfg;
  VerilogVerificationTargetGenerator::vtg_config_t vtg_cfg;
  // vtg_cfg.ForceInstCheckReset = true;
  vlg_cfg.pass_node_name = true;
  std::string RootPath = "..";
  std::string VerilogPath = RootPath + "/verilog/";
  std::string RefPath = RootPath + "/refinement/";
  std::string OutputPath = RootPath + "/verification/";
  SetUnsignedComparison(true);

  VerilogVerificationTargetGenerator vg({}, {VerilogPath + "oc8051_gm_top.v"}, "oc8051_top",
  RefPath + "ref-rel-var-map.json", RefPath + "ref-rel-inst-cond.json",
  OutputPath,
  proc_ila,
  VerilogVerificationTargetGenerator::backend_selector::JASPERGOLD,
  vtg_cfg,
  vlg_cfg);
  vg.GenerateTargets();
  
 
  return 0;
}
