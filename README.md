# Byteshell

Byteshell is a simple Unix-like shell implemented in C, inspired by the Bash shell. It provides basic shell functionalities and few built-in commands like alias, unalias, enable, echo, pwd, exit, help, cd and history.

## Prerequisites

To compile and run the Byteshell, you'll need the following:

- C compiler (e.g., GCC)
- Windows Subsystem for Linux (WSL) installed (for Windows users)

## Compilation

1. Clone the repository:

```
git clone <repository_url>
```

2. Change to the project directory:

```
cd <project_dir>
```

3. Compile the source code:

```
gcc -o beau code.c
```

## Usage

1. Run the Byteshell:

```
./beau
```

2. You can now start entering commands and interacting with the shell.

## Built-in Commands

- ### alias

    Without arguments, alias prints the list of aliases on the standard output in a form that allows them to be reused as input. If arguments are supplied, an alias is defined for each name whose value is given. If no value is given, the name and value of the alias is printed. 

    Usage:

    - List all aliases: `alias`
    - List a specific alias `alias <name>`
    - Create an alias: `alias <name>=<command>`
    - Now alias can be used as the command it was assigned to.
    - Allowed multiple space seperated entries `alias <name1>=<command1> <name2>=<command2> <name3>`. This assigns 'name1' and 'name2' aliases and prints the alias 'name3' if it exists.
    
    Example:
    ```
    >> alias 
    >> alias apple=history
    >> apple
    ```
    Hitting enter after 'apple' prints the history.
  
- ### unalias

    Remove command aliases.
    
    Usage:
    - Removes alias `unalias <name>`
    - Allowed removal of multiple aliases
    
    Example:
    ```
    >> unalias apple 
    >> unalias rm
    ```

 - ### enable

    Enable and disable builtin shell commands. Disabling a builtin allows a disk command which has the same name as a shell builtin to be executed without specifying a full pathname, even though the shell normally searches for builtins before disk commands. If -n is used, the names become disabled. Otherwise names are enabled.
    
    - List all enabled commands: `enable`
    - Enable a command: `enable <command>`
    - Disable a command: `enable -n <command>`
    - Not works for mutliple entries
    
    Example:
    ```
    >> enable
    >> enable ls
    >> enable -n cd
    ```

- ### history

    Display command history.
    
    Usage: `history`

- ### cd
  
    Changes the current working directory.
    
    Usage: `cd <directory>`
    
    Example:
  
    ```
    >> cd Documents
    ```

- ### help
  
    Display information about the shell and available commands.
    
    Usage: `help`

- ### exit

    Terminate the shell.
    
    Usage: `exit`

- ### pwd

    Print the current working directory.
    
    Usage: `pwd`

- ### echo

    Print arguments to the standard output.
    
    Usage: `echo <arguments>`

    Example:
  ```
  >> echo Hello, Beau!
  Hello, Beau!
  ```

## Additional Notes

- Remember, this is not a fully functional bash shell, it only provide some limited functionality.
- For some commands, byteshell may not work exactly like bash. Example mutliple space seperated inputs after enable will not enable all the commands, rather only the first command.
- Please ensure that you have the Windows Subsystem for Linux (WSL) installed if you are using Windows.

Made with 💙 by Ayush Trivedi
