
static metric_info_t replica_dir_metrics[] =

// name:  job.state 
// desc:  fires on state changes of the job, and has 
//        the literal value of the job state enum. 
// mode:  ReadOnly 
// unit:  1 
// type:  Enum 
// value: New 
// notes: - the state metric is inherited from 
//          saga::task, but has a different set 
//          of possible values 
//        - see description of job states above 

{ {
  "job.state",
  "fires on state changes of the job, and has"
  "the literal value of the job state enum.  ",
  "ReadOnly",
  "1",
  "Enum",
  "New",
  "- the state metric is inherited from"
  "  saga::task, but has a different set"
  "  of possible values"
  "- see description of job states above"
  }, 
{}
};

