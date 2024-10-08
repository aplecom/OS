#include <stdio.h> // perror , fgets
#include <stdlib.h> // waitpid
#include <string.h> // strcspn, strtok, strcmp
#include <unistd.h> // системные вызовы: getcwd,fork,execvp,chdir
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

// Функция для вывода приглашения командной строки
void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd); // Вывод текущего рабочего каталога в приглашении
    } else {
        perror("getcwd() error"); // выводит на экран текст ошибки, связанный с последним системным вызовом, который завершился ошибкой
        exit(EXIT_FAILURE);  // выход из программы с кодом ошибки
    }
}

// Функция для выполнения внешней команды
int execute_command(char *args[],char* shell_path) {
    pid_t pid = fork(); // Создание нового процесса через fork(), а pid идентификатор
    if (pid < 0) {
        perror("fork() failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Дочерний процесс
        setenv("parent", shell_path, 1);
        if (execvp(args[0], args) == -1) { // исполняем если можем
            perror("execvp() failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Родительский процесс
        int status;
        waitpid(pid, &status, 0); // Ожидание завершения дочернего процесса,с остановкой родительского
    }
    return 1;
}

int main() {
    char input[MAX_COMMAND_LENGTH]; // строка для хранения ввода пользователя
    char *args[MAX_ARGUMENTS]; //  массив указателей на строки для хранения аргументов команды
    char *token; // указатель на текущий токен при токенизации ввода
    char *shell_path = "/home/user/CLionProjects/myShell/myshell";

    setenv("shell", shell_path, 1); // добавляет новое значение в массиве переменных среды с именем name и значением value, или заменяет старое значение если name уже есть и replace = 1

    while (1) { // неприрывное ожидание от пользователя с помощью бесконечного цикла
        print_prompt(); // Вывод приглашения командной строки
        fgets(input, sizeof(input), stdin); // Получение ввода пользователя
        input[strcspn(input, "\n")] = '\0'; // подсчитали сколько символов без \n и добавили в конце \0

        // Токенизация ввода
        token = strtok(input, " "); // токенизирует ввод пользователя по пробелам.Вернет NULL - если нет строки  или вернет указатель на первый символ выделенной части строки.
        int i = 0;
        while (token != NULL && i < MAX_ARGUMENTS - 1) {
            args[i++] = token; // Заполнение массива аргументов
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        if (args[0] == NULL) {
            continue; // Пропуск пустых команд
        }

        // Внутренние команды
        if (strcmp(args[0], "cd") == 0) { // если индентичны
            if (args[1] == NULL) {
                printf("Usage: cd <directory>\n");
            } else {
                if (chdir(args[1]) != 0) { // не получилось вставить путь
                //perror("chdir() failed");
                printf("chdir() failed\n");
                }
            }
        } else if (strcmp(args[0], "clr") == 0) {
            system("clear");
        } else if (strcmp(args[0], "dir") == 0) {
            if (args[1] == NULL) {
                system("ls");
            } else {
                char command[1024];
                snprintf(command, sizeof(command), "ls %s", args[1]);
                system(command);
            }
        } else if (strcmp(args[0], "environ") == 0) {
            extern char **environ; // глобальная переменная, которая хранит указатели на строки, представляющие переменные окружения
            for (char **env = environ; *env != NULL; env++) {
                printf("%s\n", *env);
            }
        } else if (strcmp(args[0], "echo") == 0) {
            for (int j = 1; args[j] != NULL; j++) {
                printf("%s ", args[j]);
            }
            printf("\n");
        } else if (strcmp(args[0], "help") == 0) {
            printf("Built-in commands:\n");
            printf("cd <directory>    Change directory\n");
            printf("clr               Clear the screen\n");
            printf("dir <directory>   List contents of directory\n");
            printf("environ           Display environment variables\n");
            printf("echo <comment>    Display comment\n");
            printf("help              Display this help message\n");
            printf("pause             Pause until Enter is pressed\n");
            printf("quit              Exit the shell\n");
        } else if (strcmp(args[0], "pause") == 0) {
            printf("Press Enter to continue...");
            while (getchar() != '\n');
        } else if (strcmp(args[0], "quit") == 0) {
            break;
        } else {
            // Внешняя команда
            execute_command(args,shell_path);
        }
    }
    return EXIT_SUCCESS;
}
