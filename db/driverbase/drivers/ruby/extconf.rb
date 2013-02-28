require 'mkmf'

dir_config('djondb')

$libs = append_library($libs, "supc++")
$libs = append_library($libs, "djon-client")

create_makefile("djondb")
