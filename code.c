#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int BEAU_cd(char **args);
int BEAU_help(char **args);
int BEAU_exit(char **args);
int BEAU_pwd(char **args);
int BEAU_echo(char **args);
int BEAU_history(char **args);
int BEAU_alias(char **args);
int BEAU_unalias(char **args);
int BEAU_enable(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "echo",
    "history",
    "alias",
    "unalias",
    "enable"};

int (*builtin_func[])(char **) = {
    &BEAU_cd,
    &BEAU_help,
    &BEAU_exit,
    &BEAU_pwd,
    &BEAU_echo,
    &BEAU_history,
    &BEAU_alias,
    &BEAU_unalias,
    &BEAU_enable};

int BEAU_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

// verifies if a command is a builtin or not
int BEAU_verify(char *command)
{
    int i;
    for (i = 0; i < BEAU_num_builtins(); i++)
    {
        if (strcmp(builtin_str[i], command) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Builtin cd
int BEAU_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "BEAU: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("BEAU");
        }
    }
    return 1;
}

// Builtin help
int BEAU_help(char **args)
{
    int i;
    printf("Welcome to BEAU, version 1.0\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("BEAU allows following builtin commands:\n");

    for (i = 0; i < BEAU_num_builtins(); i++)
    {
        printf("%s\n", builtin_str[i]);
    }
    printf("For detailed usage of these commands, visit documentation: https://github.com/PvNagh/Byteshell\n");
    return 1;
}

// Builtin exit
int BEAU_exit(char **args)
{
    return 0;
}

// Builtin echo
int BEAU_echo(char **args)
{
    int i = 1;
    while (args[i] != NULL)
    {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
    return 1;
}

// Builtin pwd
int BEAU_pwd(char **args)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n", cwd);
    }
    else
    {
        perror("BEAU");
    }
    return 1;
}

// Builtin history

// linked list for storing history

struct Node
{
    char *str;
    struct Node *next;
};

struct Node *head = NULL;
struct Node *cur = NULL;

char *strAppend(char *str1, char *str2)
{
    char *str3 = (char *)malloc(sizeof(char *) * (strlen(str1) + strlen(str2)));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

void add_to_history(char **args)
{
    if (head == NULL)
    {
        head = (struct Node *)malloc(sizeof(struct Node));
        head->str = (char *)malloc(0x1000);
        char *str1 = " ";
        if (args[1] == NULL)
        {
            strcpy(head->str, strAppend(args[0], str1));
        }

        else
        {
            strcpy(head->str, strAppend(args[0], str1));
            strcpy(head->str, strAppend(head->str, args[1]));
        }

        head->next = NULL;
        cur = head;
    }
    else
    {
        struct Node *ptr = (struct Node *)malloc(sizeof(struct Node));
        cur->next = ptr;
        ptr->str = (char *)malloc(0x1000);
        char *str1 = " ";
        if (args[1] == NULL)
            strcpy(ptr->str, strAppend(args[0], str1));
        else
        {
            strcpy(ptr->str, strAppend(args[0], str1));
            strcpy(ptr->str, strAppend(ptr->str, args[1]));
        }
        ptr->next = NULL;
        cur = ptr;
    }
}

int BEAU_history(char **args)
{
    struct Node *ptr = head;
    int i = 1;
    while (ptr != NULL)
    {
        printf("%d %s\n", i++, ptr->str);
        ptr = ptr->next;
    }
    return 1;
}

// builtin alias

struct AliasNode
{
    char *name;
    char *command;
    struct AliasNode *next;
};

struct AliasNode *alias_list = NULL;

void add_alias(const char *name, const char *command)
{
    struct AliasNode *new_alias = (struct AliasNode *)malloc(sizeof(struct AliasNode));
    new_alias->name = strdup(name);
    new_alias->command = strdup(command);
    new_alias->next = alias_list;
    alias_list = new_alias;
}

int BEAU_alias(char **args)
{
    if (args[1] == NULL)
    {
        struct AliasNode *curr = alias_list;
        while (curr != NULL)
        {
            printf("alias %s=%s\n", curr->name, curr->command);
            curr = curr->next;
        }
    }
    else
    {
        int i = 1;
        while (args[i] != NULL)
        {
            char *equal_sign = strchr(args[i], '=');
            if (equal_sign - args[i] == 0 || equal_sign == NULL)
            {
                struct AliasNode *curr = alias_list;
                while (curr != NULL)
                {
                    if (strcmp(curr->name, args[i]) == 0)
                    {
                        printf("%s=%s\n", curr->name, curr->command);
                        break;
                    }
                    curr = curr->next;
                }
                if (curr == NULL)
                {
                    printf("BEAU: alias not found: %s\n", args[i]);
                }
            }
            else
            {
                *equal_sign = '\0';
                char *alias_name = args[i];
                char *alias_command = equal_sign + 1;

                add_alias(alias_name, alias_command);
            }
            i++;
        }
    }
    return 1;
}

// builtin unalias

void remove_alias(const char *name)
{
    struct AliasNode *prev = NULL;
    struct AliasNode *curr = alias_list;

    while (curr != NULL)
    {
        if (strcmp(curr->name, name) == 0)
        {
            if (prev == NULL)
            {
                // name is in the head of the list
                alias_list = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }
            free(curr->name);
            free(curr->command);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("BEAU: unalias: '%s' not found.\n", name);
}

int BEAU_unalias(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "BEAU: usage: unalias <name>\n");
    }
    else
    {
        int i = 1;
        while (args[i] != NULL)
        {
            remove_alias(args[i]);
            i++;
        }
    }
    return 1;
}

// builtin enable

struct CommandNode
{
    char *command;
    struct CommandNode *cnext;
};

struct CommandNode *enabled_commands = NULL; // list to store all the commands that are enabled

int is_enabled(char *command)
{
    struct CommandNode *current = enabled_commands;
    while (current != NULL)
    {
        if (strcmp(current->command, command) == 0)
        {
            return 1;
        }
        current = current->cnext;
    }
    return 0;
}

void enable_command(char *command)
{
    if (!is_enabled(command))
    {
        struct CommandNode *new_node = (struct CommandNode *)malloc(sizeof(struct CommandNode));
        new_node->command = strdup(command);
        new_node->cnext = enabled_commands;
        enabled_commands = new_node;
    }
}

void disable_command(char *command)
{
    struct CommandNode *current = enabled_commands;
    struct CommandNode *previous = NULL;
    while (current != NULL)
    {
        if (strcmp(current->command, command) == 0)
        {
            if (previous == NULL)
            {
                enabled_commands = current->cnext; // Command is in the head of the list
            }
            else
            {
                previous->cnext = current->cnext;
            }
            free(current->command);
            free(current);
            return;
        }
        previous = current;
        current = current->cnext;
    }
}

int BEAU_enable(char **args)
{
    if (args[1] == NULL)
    {
        struct CommandNode *curr = enabled_commands;
        while (curr != NULL)
        {
            printf("enable '%s' \n", curr->command);
            curr = curr->cnext;
        }
    }
    else
    {
        if (strcmp(args[1], "-n") == 0 && args[2] != NULL)
        {
            if (BEAU_verify(args[2]))
            {
                if (is_enabled(args[2]))
                {
                    disable_command(args[2]);
                }
            }
            else
            {
                printf("BEAU: '%s': is not recognized as a BEAU buitlin.\n", args[2]);
            }
        }

        else if (args[1] != NULL)
        {
            struct CommandNode *curr = enabled_commands;
            while (curr != NULL)
            {
                if (BEAU_verify(args[1]))
                {
                    enable_command(args[1]);
                    break;
                }
                else
                {
                    printf("BEAU: '%s': is not recognized as BEAU buitlin\n", args[1]);
                    break;
                }
            }
            curr = curr->cnext;
        }
    }
    return 1;
}

// Launch
int BEAU_launch(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("BEAU");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("BEAU");
    }
    else
    {
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Execute function
int BEAU_execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1;
    }

    struct AliasNode *alias = alias_list;
    while (alias != NULL)
    {
        if (strcmp(args[0], alias->name) == 0)
        {
            args[0] = alias->command;
            break;
        }
        alias = alias->next;
    }

    int i;
    for (i = 0; i < BEAU_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0 && is_enabled(args[0]))
        {
            return (*builtin_func[i])(args);
        }
    }

    return BEAU_launch(args);
}

#define BEAU_RL_BUFSIZE 1024

char *BEAU_read_line(void)
{
    int bufsize = BEAU_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "BEAU: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        if (c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize)
        {
            bufsize += BEAU_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer)
            {
                fprintf(stderr, "BEAU: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define BEAU_TOK_BUFSIZE 64
#define BEAU_TOK_DELIM " \t\r\n\a"

char **BEAU_split_line(char *line)
{
    int bufsize = BEAU_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "BEAU: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, BEAU_TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += BEAU_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "BEAU: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, BEAU_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void BEAU_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf(">> ");
        line = BEAU_read_line();      // read a line from the user
        args = BEAU_split_line(line); // split the line into tokens
        status = BEAU_execute(args);  // check if the command entered is external or buitlin and execute accordingly
        if (args[0] != NULL)
        {
            add_to_history(args);
        }
        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    enable_command("cd");
    enable_command("help");
    enable_command("exit");
    enable_command("pwd");
    enable_command("echo");
    enable_command("history");
    enable_command("alias");
    enable_command("unalias");
    enable_command("enable");

    BEAU_loop();

    return EXIT_SUCCESS;
}