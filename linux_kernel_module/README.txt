compile with 'make all'
Insert the module using "sudo insmod PIDLKM.ko inp_pid=1" without the quotes.
The PID can be any PID running on the system, leaving off the inp_pid parameter will default to displaying all processes.

NOTE: Tested on Ubuntu 18.04. Changes to task_struct may break this on newer kernels
