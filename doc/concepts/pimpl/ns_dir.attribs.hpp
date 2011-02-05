
// from some global include
struct attrib_info
{
  std::string name;
  std::string type;
  std::string desc;
  std::string mode;
  std::string value;
  std::string notes;
} attrib_info_t;

static std::vector <attrib_info> replica_attributes;

// name:  ThreadsPerProcess 
// desc:  number of threads to start per process 
// mode:  ReadWrite, optional 
// type:  Int 
// value: ’1’ 
// notes: - semantics as defined in JSDL 
//        - available in JSDL, SPMD extension 

attribute_info_t a_1 = {
  "ThreadsPerProcess                     ",
  "number of threads to start per process",
  "ReadWrite, optional                   ",
  "Int                                   ",
  "’1’                                   ",
  "- semantics as defined in JSDL      \n"
  "- available in JSDL, SPMD extension   "
};

replica_attributes.push_back (a_1);

// in ns_dir.hpp, where this file is included


