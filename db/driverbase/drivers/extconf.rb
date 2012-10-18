require 'mkmf'

dir_config('djondb')

$libs = append_library($libs, "supc++")

create_makefile("djonwrapper")
