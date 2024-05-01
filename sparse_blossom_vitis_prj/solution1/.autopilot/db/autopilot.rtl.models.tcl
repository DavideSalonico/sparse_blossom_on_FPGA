set SynModuleInfo {
  {SRCNAME compute_corr_Pipeline_compute MODELNAME compute_corr_Pipeline_compute RTLNAME compute_corr_compute_corr_Pipeline_compute
    SUBMODULES {
      {MODELNAME compute_corr_flow_control_loop_pipe_sequential_init RTLNAME compute_corr_flow_control_loop_pipe_sequential_init BINDTYPE interface TYPE internal_upc_flow_control INSTNAME compute_corr_flow_control_loop_pipe_sequential_init_U}
    }
  }
  {SRCNAME compute_corr_Pipeline_write_and_reset MODELNAME compute_corr_Pipeline_write_and_reset RTLNAME compute_corr_compute_corr_Pipeline_write_and_reset}
  {SRCNAME compute_corr MODELNAME compute_corr RTLNAME compute_corr IS_TOP 1
    SUBMODULES {
      {MODELNAME compute_corr_corr_internal_RAM_AUTO_1R1W RTLNAME compute_corr_corr_internal_RAM_AUTO_1R1W BINDTYPE storage TYPE ram IMPL auto LATENCY 2 ALLOW_PRAGMA 1}
    }
  }
}
