/**
* LKM to print some information about all process with a PID greater than 
* the one that was entered. Defaults to PID = 1 if none entered.
*
* Completion time: 15 Hours
*
* @author Greg Mooney
* @version 1.0
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>

#define PROC_NAME "MooneyLKM"

// PID defaults to 1 if none is entered
static long inp_pid = 1;
static char task_com[TASK_COMM_LEN];
static char parent_task_com[TASK_COMM_LEN];  
static char child_task_com[TASK_COMM_LEN];   
static struct task_struct *tsk;
static struct task_struct *parent;
static struct task_struct *child;
static struct list_head *list;
static int child_count;

static void print_info(void);

// Read input
module_param(inp_pid, long, 0);

//Called when module is loaded 
static int mod_init(void)
{      
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        
        print_info();

	return 0;
}

//Called when module removed 
static void mod_exit(void) 
{
        printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static void print_info() {
  for_each_process(tsk) {
            if (tsk->pid >= inp_pid) {     
                // Get name for pid
                get_task_comm(task_com, tsk);                
                
                // Print name, pid, state, and priorities to kernel log 
                printk(KERN_INFO "PROCESS\t\t\tPID\tState\tPrio\tSt_Prio\tNorm_Prio\n");
                printk(KERN_INFO "%-16s    \t%d \t%ld \t%d \t%d \t%d\n", task_com, tsk->pid, tsk->__state, tsk->prio, tsk->static_prio, tsk->normal_prio);
                
                // Get parent info
                parent = tsk->real_parent;
                get_task_comm(parent_task_com, parent);
                
                //print parent info 
                printk(KERN_INFO "PARENT\n");
                printk(KERN_INFO "%-16s    \t%d \t%ld \t%d \t%d \t%d\n", parent_task_com, parent->pid, parent->__state, parent->prio, parent->static_prio, parent->normal_prio);
                
                // Check if there's children
                if (!list_empty(&tsk->children)) {  
                    // Child count for display purposes
                    child_count = 0;
                    // Get the task_struct of the children and print the info
                    list_for_each(list, &tsk->children) {                       
                        child = list_entry(list, struct task_struct, sibling);
                        get_task_comm(child_task_com, child);
                        printk(KERN_INFO "CHILD%d\n", child_count);
                        printk(KERN_INFO "%-16s    \t%d \t%ld \t%d \t%d \t%d\n", child_task_com, child->pid, child->__state, child->prio, child->static_prio, child->normal_prio);
                        
                        child_count++;
                    }
                }
                 
                printk(KERN_INFO "\n");   
            } 
        } 
    
}


// Registration macros 
module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
 
