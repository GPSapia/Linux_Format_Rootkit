A simple Linux rootkit, developed mainly to study internals of the Kernel and have fun with it.

In the Kernel there is a list of every executable format supported for the execution, represented by a struct linux_binfmt. Each one of this formats will have to define a function with which he will try to run the executable that we (from User Space) wish to run. When we launch a program, this list is scanned and all the handling functions will be executed until the right one is found.

In particular, upon a mismatch between the format of the executable and the format that the specific linux_binfmt struct implements, this function will return an error code and the scanning of the list will continue.

This module inserts a fake linux_binfmt in front of the list, whose handling function will perform every malicious (or not) actions that we want, but it will return an error in order to make the executable be handled by the proper binfmt.

In the case of this rootkit, our handling function will check if the user executed the command './IWantToBeRoot.sh'. In such a case, the next command launched from the shell will be runned as root.
