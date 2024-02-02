#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<unistd.h>
#include<time.h>

//402105892

// reset mibare be commit ghabli
// u not in repo
// emtiazi depth
// be repo address vorudi bede adresi ke mikhai azash shuru koni ro
// emtiazi undo ta 10
// relative kardane last add
// laptop haye dige
// unfunction current_time

//tag



int args;
FILE *last_add;


char *repository_address(){
    char *return_value = (char *)malloc(1000);
    char repo_address[1000];
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char address[1000];
    strcpy(address, cwd);
    struct dirent *entry;
    while(1){
        DIR *dir = opendir(".");
        while((entry = readdir(dir)) != NULL){
            if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".teibur") == 0){
                closedir(dir);
                chdir(".teibur");
                getcwd(repo_address, sizeof(repo_address));
                strcpy(return_value, repo_address);
                chdir(cwd);
                return return_value;
            }
        }
        closedir(dir);
        if(strcmp(address, "/") == 0){
            chdir(cwd);
            return NULL;
        }
        chdir("..");
        getcwd(address, sizeof(address));
    }
}


char *relative_path(char path[], char repo[]){
    chdir(repo);
    chdir("staging area");
    char relative[500];
    repo[strlen(repo) - 8] = '\0';
    memmove(relative, path + strlen(repo) + 1, strlen(path + strlen(repo) + 1));
    relative[strlen(path + strlen(repo)) - 1] = '\0';
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, relative);
    return return_value;
}


char *current_time(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}


char *last_commit(){
    char id_txt[1000];
    strcpy(id_txt, repository_address());
    strcat(id_txt, "/master/id.txt");
    FILE *id_file_ = fopen(id_txt, "r");
    char last_id[10];
    int last_id_d;
    fgets(last_id, 5, id_file_);
    sscanf(last_id, "%d", &last_id_d);
    last_id_d--;
    sprintf(last_id, "%d", last_id_d);
    char last[1000];
    strcpy(last, repository_address());
    strcat(last, "/master/");
    strcat(last, last_id);
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, last);
    return return_value;
}


int is_same_file(char first[], char second[]){
    FILE *file1 = fopen(first, "r");
    FILE *file2 = fopen(second, "r");
    if(file1 == NULL || file2 == NULL) perror("Cant open file");
    char char1;
    char char2;
    do{
        char1 = fgetc(file1);
        char2 = fgetc(file2);
        if (char1 != char2)
            return 0;
    }while(char1 != EOF && char2 != EOF);
    if (char1 == EOF && char2 == EOF)
        return 1;
    return 0;
}


int is_same_dir(char first[], char second[]){
    DIR *dir1 = opendir(first);
    DIR *dir2 = opendir(second);
    struct dirent *entry1;
    struct dirent *entry2;
    while(1){
        entry1 = readdir(dir1);
        entry2 = readdir(dir2);
        if(entry1 == NULL && entry2 == NULL)
            return 1;
        if(entry1 == NULL || entry2 == NULL)
            return 0;
        if(entry1->d_name[0] == '.')
            continue;
        char entry1dir[1000];
        strcpy(entry1dir, first);
        strcat(entry1dir, "/");
        strcat(entry1dir, entry1->d_name);
        char entry2dir[1000];
        strcpy(entry2dir, second);
        strcat(entry2dir, "/");
        strcat(entry2dir, entry2->d_name);
        if(strcmp(entry1->d_name, entry2->d_name) != 0)
            return 0;
        if(entry1->d_type != entry2->d_type)
            return 0;
        if(entry1->d_type == DT_DIR){
            if(!is_same_dir(entry1dir, entry2dir))
                return 0;
        }else{
            if(!is_same_file(entry1dir, entry2dir))
                return 0;
        }
    }
}


int config(int global, char arg[], char str[]){
    char config_address[1000];
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char address[1000];
    strcpy(address, cwd);
    if(global){
        strcpy(config_address, "/home/mohammad/teibur files");
    }else{
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        strcpy(config_address, repository_address());
    }
    chdir(config_address);
    mkdir("config", 0755);
    chdir("config");
    if(strcmp(arg, "user.name") == 0){
        FILE *username = fopen("username.txt", "w");
        fputs(str, username);
        fclose(username);
    }else if(strcmp(arg, "user.email") == 0){
        FILE *email = fopen("email.txt", "w");
        fputs(str, email);
        fclose(email);
    }else if(strncmp(arg, "alias.", 6) == 0){
        mkdir("aliases", 0755);
        chdir("aliases");
        char alias[100];
        sscanf(arg, "alias.%s", alias);
        strcat(alias, ".txt");
        FILE *aliasfile = fopen(alias, "w");
        fputs(str, aliasfile);
        fclose(aliasfile);
    }else{
        perror("invallid command");
    }
}


int init(){
    if(repository_address() != NULL){
        perror("repository already exists");
        return 1;
    }
    mkdir(".teibur", 0755);
    chdir(".teibur");
    mkdir("staging area", 0755);
    mkdir("shortcuts", 0755);
    mkdir("master", 0755);
    chdir("master");
    mkdir("3426", 0755);
    FILE *id = fopen("id.txt", "w");
    fprintf(id, "3427");
    return 0;
}


int add_file(char filename[], char copy_from[]){
    char current[1000];
    getcwd(current, 1000);
    strcat(current, "/");
    strcat(current, filename);
    fputs(current, last_add);
    fprintf(last_add, "\n");
    char command[2000];
    sprintf(command, "cp \"%s\" \"%s\"", copy_from, current);
    system(command);
}


int add_directory(char token[], char repo_r[]){
    mkdir(token, 0755);
    chdir(token);
    struct stat entry;
    char prev_token[100];
    strcpy(prev_token, token);
    token = strtok(NULL, "/");
    
    if(token == NULL){
        char current[1000];
        getcwd(current, 1000);
        fputs(current, last_add);
        fprintf(last_add, "\n");
        current[strlen(current) - strlen(prev_token) - 1] = '\0';
        char command[2000];
        sprintf(command, "cp -r \"%s\" \"%s\"", repo_r, current);
        system(command);
        return 0;
    }
    strcat(repo_r, "/");
    strcat(repo_r, token);
    if(stat(repo_r, &entry) == 0){
        if(S_ISDIR(entry.st_mode)){
            add_directory(token, repo_r);
        }else{
            add_file(token, repo_r);
        }
    }else{
        chdir(repository_address());
        chdir("staging area");
        FILE *deleted_file = fopen(relative_path(repo_r, repository_address()), "r");
        if(deleted_file == NULL){
            perror("File not found");
            return 1;
        }
        char command[1000];
        sprintf(command, "rm \"%s\"", relative_path(repo_r, repository_address()));
        system(command);
    }
}


int add(char file_or_directory[], int _n, char repo[]){
    char temp[1000];
    getcwd(temp, sizeof(temp));
    char relative[1000];
    strcpy(relative, relative_path(file_or_directory, repo));
    if(_n){
        chdir("staging area");
        chdir(relative);
        char direc1[1000];
        getcwd(direc1, sizeof(direc1));
        chdir(temp);
        struct dirent *entry2;
        DIR *dir2 = opendir(".");
        char direc2[1000];
        getcwd(direc2, sizeof(direc2));
        while((entry2 = readdir(dir2)) != NULL){
            if(entry2->d_name[0] == '.')
                continue;
            char file2path[1000];
            strcpy(file2path, direc2);
            strcat(file2path, "/");
            strcat(file2path, entry2->d_name);
            int staged = 0;
            struct dirent *entry1;
            DIR *dir1 = opendir(direc1);
            while((entry1 = readdir(dir1)) != NULL){
                char file1path[1000];
                strcpy(file1path, direc1);
                strcat(file1path, "/");
                strcat(file1path, entry1->d_name);
                if(strcmp(entry1->d_name, entry2->d_name) == 0){
                    if(entry2->d_type == DT_DIR){
                        if(is_same_file(file1path, file2path)){
                            staged = 1;
                        }
                    }else{
                        if(is_same_file(file1path, file2path)){
                            staged = 1;
                        }
                    }
                }
            }
            if(staged){
                printf("%s : staged\n", entry2->d_name);
            }else{
                printf("%s : not staged\n", entry2->d_name);
            }
        }
        return 0;
    }
    chdir(temp);
    char *token = strtok(relative, "/");
    chdir(repo);
    strcat(repo, "/");
    strcat(repo, token);
    struct stat entry;
    if(stat(token, &entry) == 0){
        if(S_ISDIR(entry.st_mode)){
            chdir(".teibur/staging area");
            add_directory(token, repo);
        }else{
            add_file(token, repo);
        }
    }else perror("File not found");
}


int reset(char path[], char repo[]){
    char command[1000];
    sprintf(command, "rm -r \"%s\"", relative_path(path, repo));
    system(command);
}


int status(){

}


int commit(char message[]){
    if(strlen(message) > 72){
        perror("message is too long");
        return 1;
    }
    
    char staging_area[1000];
    strcpy(staging_area, repository_address());
    strcat(staging_area, "/staging area");
    if(is_same_dir(last_commit(), staging_area)){
        perror("Nothing to commit");
        return 1;
    }
    char commit_time[100];
    strcpy(commit_time, current_time());
    char local_username[1000];
    strcpy(local_username, repository_address());
    strcat(local_username, "/config/username.txt");
    char local_email[1000];
    strcpy(local_email, repository_address());
    strcat(local_email, "/config/email.txt");
    FILE *username_file = fopen("/home/mohammad/teibur files/config/username.txt", "r");
    FILE *email_file = fopen("/home/mohammad/teibur files/config/email.txt", "r");
    if(username_file == NULL || email_file == NULL){
        username_file = fopen(local_username, "r");
        email_file = fopen(local_email, "r");
        if(username_file == NULL || email_file == NULL){
            perror("Please config username and email");
            return 1;
        }
    }
    char username[100];
    char email[100];
    fgets(username, 100 ,username_file);
    fgets(email, 100, email_file);
    char command[3000];
    chdir(repository_address());
    chdir("master");
    int commit_id;
    char commit_id_s[10];
    FILE *id_file = fopen("id.txt", "r");
    fgets(commit_id_s, 10, id_file);
    fclose(id_file);
    id_file = fopen("id.txt", "w");
    char dest[1000];
    getcwd(dest, sizeof(dest));
    strcat(dest, "/");
    strcat(dest, commit_id_s);
    sscanf(commit_id_s, "%d", &commit_id);
    char src[1000];
    strcpy(src, repository_address());
    strcat(src, "/staging area");
    sprintf(command, "cp -r \"%s\" \"%s\"", src, dest);
    system(command);
    printf("%s\ntime : %sid : %s\n", message, commit_time, commit_id_s);
    fprintf(id_file, "%d", commit_id + 1);
    strcat(commit_id_s, ".txt");
    FILE *commit_info = fopen(commit_id_s, "w");
    fprintf(commit_info, "%s\n%s\n%s\n%s", username, email, message, commit_time);
}


int set_shortcut(char message[], char shortcut_name[]){
    char shortcut_path[1000];
    strcpy(shortcut_path, repository_address());
    strcat(shortcut_path, "/shortcuts/");
    strcat(shortcut_path, shortcut_name);
    FILE *shortcut = fopen(shortcut_path, "r");
    if(shortcut != NULL){
        perror("Shortcut already exists");
        return 1;
    }
    shortcut = fopen(shortcut_path, "w");
    fputs(message, shortcut);
}


int replace_shortcut(char message[], char shortcut_name[]){
    char shortcut_path[1000];
    strcpy(shortcut_path, repository_address());
    strcat(shortcut_path, "/shortcuts/");
    strcat(shortcut_path, shortcut_name);
    FILE *shortcut = fopen(shortcut_path, "r");
    if(shortcut == NULL){
        perror("Shortcut doesn't exists");
        return 1;
    }
    shortcut = fopen(shortcut_path, "w");
    fputs(message, shortcut);
}


int remove_shortcut(char shortcut_name[]){
    char shortcut_path[1000];
    strcpy(shortcut_path, repository_address());
    strcat(shortcut_path, "/shortcuts/");
    strcat(shortcut_path, shortcut_name);
    FILE *shortcut = fopen(shortcut_path, "r");
    if(shortcut == NULL){
        perror("Shortcut doesn't exists");
        return 1;
    }
    char command[2000];
    sprintf(command, "rm \"%s\"", shortcut_path);
    system(command);
}


int main(int argc, char *argv[]){
    if(argc == 1)
        return 0;
    if(strcmp(argv[1], "config") == 0){
        if(argc == 5){
            if(strcmp(argv[2], "-global") == 0){
                config(1, argv[3], argv[4]);
            }else{
                perror("invalid command");
                return 1;
            }
        }else if(argc == 4){
            config(0, argv[2], argv[3]);
        }else{
            perror("invalid command");
            return 1;
        }
    }else if(strcmp(argv[1], "init") == 0){
        if(argc == 2){
            init();
        }else{
            perror("invalid command");
        }
    }else if(strcmp(argv[1], "add") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        char file_path[1000];
        strcpy(file_path, repository_address());
        strcat(file_path, "/last add.txt");
        last_add = fopen(file_path, "w");
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        if(strcmp(argv[2], "-f") == 0){
            for(int i = 3; i < argc; i++){
                chdir(cwd);
                char real_arg[1000];
                realpath(argv[i], real_arg);
                add(real_arg, 0, repository_address());
                args = argc - 3;
            }
        }if(strcmp(argv[2], "-n") == 0){
            char real_arg[1000];
            getcwd(real_arg, sizeof(real_arg));
            add(real_arg, 1, repository_address());
        }else{
            for(int i = 2; i < argc; i++){
                chdir(cwd);
                char real_arg[1000];
                realpath(argv[i], real_arg);
                add(real_arg, 0, repository_address());
                args = argc - 2;
            }
        }
    }else if(strcmp(argv[1], "reset") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        if(strcmp(argv[2], "-undo")== 0){
            char saved_path[1000];
            char file_path[1000];
            strcpy(file_path, repository_address());
            strcat(file_path, "/last add.txt");
            last_add = fopen(file_path, "r");
            while(fgets(saved_path, 1000, last_add) != NULL){
                saved_path[strlen(saved_path) - 1] = '\0';
                char command[2000];
                sprintf(command, "rm -r \"%s\"", saved_path);
                system(command);
            }
            return 0;
        }
        char real_arg[1000];
        realpath(argv[2], real_arg);
        reset(real_arg, repository_address());
    }else if(strcmp(argv[1], "status") == 0){
        if(argc != 2){
            perror("invalid command");
            return 1;
        }
        status();
    }else if(strcmp(argv[1], "commit") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        if(argc == 2){
            perror("Your commit should have a message");
            return 1;
        }
        if(argc == 4){
            if(strcmp(argv[2], "-m") == 0){
                commit(argv[3]);
                return 0;
            }
            if(strcmp(argv[2], "-s") == 0){
                char shortcut_path[1000];
                strcpy(shortcut_path, repository_address());
                strcat(shortcut_path, "/shortcuts/");
                strcat(shortcut_path, argv[3]);
                FILE *shortcut = fopen(shortcut_path, "r");
                if(shortcut == NULL){
                    perror("Shortcut doesn't exist");
                    return 1;
                }
                char message[1000];
                fgets(message, 1000, shortcut);
                commit(message);
                return 0;
            }
        }
        perror("invalid command");
        return 1;
    }else if(strcmp(argv[1], "set") == 0 && strcmp(argv[2], "-m") == 0 && strcmp(argv[4], "-s") == 0 && argc == 6){
        set_shortcut(argv[3], argv[5]);
    }else if(strcmp(argv[1], "replace") == 0 && strcmp(argv[2], "-m") == 0 && strcmp(argv[4], "-s") == 0 && argc == 6){
        replace_shortcut(argv[3], argv[5]);
    }else if(strcmp(argv[1], "remove") == 0 && strcmp(argv[2], "-s") == 0 && argc == 4){
        remove_shortcut(argv[3]);
    }else{
        char *arg_txt = strcat(argv[1], ".txt");
        chdir(repository_address());
        if(chdir("config") == 0){
            if(chdir("aliases") == 0){
                DIR *dir = opendir(".");
                struct dirent *entry;
                if(dir == NULL){
                    perror("Can't open directory");
                    return 1;
                }
                while((entry = readdir(dir)) != NULL){
                    if(strcmp(entry->d_name, arg_txt) == 0){
                        FILE *aliasfile = fopen(entry->d_name, "r");
                        char command[100];
                        fgets(command, 100, aliasfile);
                        system(command);
                        return 0;
                    }
                }
            }
        }
        chdir("/home/mohammad/teibur files/");
        mkdir("config", sizeof("config"));
        chdir("config");
        if(chdir("aliases") == 0){
            DIR *dir = opendir(".");
            struct dirent *entry;
            if(dir == NULL){
                perror("Can't open directory");
                return 1;
            }
            while((entry = readdir(dir)) != NULL){
                if(strcmp(entry->d_name, arg_txt) == 0){
                    FILE *aliasfile = fopen(entry->d_name, "r");
                    char command[100];
                    fgets(command, 100, aliasfile);
                    system(command);
                    return 0;
                }
            }
        }
        perror("invalid command");
    }
    return 0;
}