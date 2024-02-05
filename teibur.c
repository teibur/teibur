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


char *master(){
    char master_path[1000];
    strcpy(master_path, repository_address());
    strcat(master_path, "/master");
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, master_path);
    return return_value;
}


char *staging_area(){
    char staging_path[1000];
    strcpy(staging_path, repository_address());
    strcat(staging_path, "/staging area");
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, staging_path);
    return return_value;
}


char *relative_path(char path[]){
    char repo[1000];
    strcpy(repo, repository_address());
    repo[strlen(repo) - 8] = '\0';
    if(strcmp(repo, path) == 0)
        return ".";
    char relative[500];
    memmove(relative, path + strlen(repo) + 1, strlen(path + strlen(repo) + 1));
    relative[strlen(path + strlen(repo)) - 1] = '\0';
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, relative);
    return return_value;
}


char *relative_path2(char path[], char notrepo[]){
    char relative[500];
    memmove(relative, path + strlen(notrepo) + 1, strlen(path + strlen(notrepo) + 1));
    relative[strlen(path + strlen(notrepo)) - 1] = '\0';
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


char *commit_path(char address[], char hash[]){
    char temp[1000];
    getcwd(temp, 1000);
    chdir(address);
    struct stat entry;
    if(stat(hash, &entry) == 0){
        strcat(address, "/");
        strcat(address, hash);
        chdir(temp);
        return address;
    }
    DIR *dir = opendir(address);
    struct dirent *entryy;
    while((entryy = readdir(dir)) != NULL){
        if(entryy->d_name[0] == '-'){
            char addr[1000];
            strcpy(addr, address);
            strcat(addr, "/");
            strcat(addr, entryy->d_name);
            char addrr[1000];
            strcpy(addrr, addr);
            if(commit_path(addr, hash) == NULL)
                continue;
            chdir(temp);
            char *return_value = (char *)malloc(1000);
            strcpy(return_value, commit_path(addrr, hash));
            return return_value;
            
        }
    }
    chdir(temp);
    return NULL;
}


char *minus_one(char in_string[]){
    int in_int;
    sscanf(in_string, "%d", &in_int);
    in_int--;
    sprintf(in_string, "%d", in_int);
    return in_string;
}


char *last_commit(){
    char id_txt[1000];
    strcpy(id_txt, master());
    strcat(id_txt, "/id.txt");
    FILE *id_file_ = fopen(id_txt, "r");
    char last_id[10];
    fgets(last_id, 5, id_file_);
    return minus_one(last_id);
}


char *last_commit_in_branch(char branch_p[]){
    char temp[1000];
    getcwd(temp, sizeof(temp));
    chdir(branch_p);
    FILE *last_txt = fopen("last.txt", "r");
    char commit_id[10];
    fgets(commit_id, 5, last_txt);
    char *return_value = (char *)malloc(1000);
    if(chdir(commit_id) == 0){
        getcwd(return_value, 1000);
    }else{
        chdir("../");
        getcwd(return_value, 1000);
        strcat(return_value, "/");
        strcat(return_value, commit_id);
    }
    chdir(temp);
    return return_value;
}


int is_same_file(char first[], char second[]){
    FILE *file1 = fopen(first, "rb");
    FILE *file2 = fopen(second, "rb");
    if(file1 == NULL && file2 == NULL) perror("Cant open file");
    if(file1 == NULL || file2 == NULL) return 0;
    void *byte1 = malloc(1);
    void *byte2 = malloc(1);
    fseek(file1, 0, SEEK_END);
    fseek(file2, 0, SEEK_END);
    int size1 = ftell(file1);
    int size2 = ftell(file2);
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);
    if(size1 != size2){
        fclose(file1);
        fclose(file2);
        return 0;
    }
    for(int i = 0; i < size1; i++){
        fread(byte1, 1, 1, file1);
        fread(byte2, 1, 1, file2);
        if(memcmp(byte1, byte2, 1) != 0){
            fclose(file1);
            fclose(file2);
            return 0;
        }
    }
    fclose(file1);
    fclose(file2);
    return 1;
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


char *current_branch(){
    char file_address[1000];
    strcpy(file_address, repository_address());
    strcat(file_address, "/branch.txt");
    FILE *branch = fopen(file_address, "r");
    char branch_address[1000];
    fgets(branch_address, 1000, branch);
    if(branch_address[strlen(branch_address) - 1] == '\n')
        branch_address[strlen(branch_address) - 1] = '\0';
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, branch_address);
    return return_value;
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
    FILE *branch = fopen("branch.txt", "w");
    fputs(master(), branch);
    chdir("master");
    mkdir("3426", 0755);
    FILE *id = fopen("id.txt", "w");
    fprintf(id, "3427");
    FILE *last_c_in_b = fopen("last.txt", "w");
    fprintf(last_c_in_b, "3426");
    return 0;
}


int add_file(char filename[], char copy_from[]){
    char current[1000];
    getcwd(current, 1000);
    strcat(current, "/");
    strcat(current, filename);
    fputs(relative_path2(current, staging_area()), last_add);
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
        fputs(relative_path2(current, staging_area()), last_add);
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
        chdir(staging_area());
        FILE *deleted_file = fopen(relative_path(repo_r), "r");
        if(deleted_file == NULL){
            perror("File not found");
            return 1;
        }
        char command[1000];
        sprintf(command, "rm \"%s\"", relative_path(repo_r));
        system(command);
    }
}


int add_n(char direc[], int depth, int first_depth){
    depth--;
    if(depth < 0)
        return 0;
    chdir(staging_area());
    chdir(relative_path(direc));
    char direc1[1000];
    getcwd(direc1, sizeof(direc1));
    chdir(repository_address());
    chdir("../");
    if(chdir(relative_path(direc)) != 0)
        return 1;
    char direc2[1000];
    getcwd(direc2, sizeof(direc2));
    struct dirent *entry2;
    DIR *dir2 = opendir(".");
    while((entry2 = readdir(dir2)) != NULL){
        if(entry2->d_name[0] == '.')
            continue;
        char file2path[1000];
        strcpy(file2path, direc2);
        strcat(file2path, "/");
        strcat(file2path, entry2->d_name);
        struct dirent *entry1;
        DIR *dir1 = opendir(direc1);
        int found = 0;
        while((entry1 = readdir(dir1)) != NULL){
            char file1path[1000];
            strcpy(file1path, direc1);
            strcat(file1path, "/");
            strcat(file1path, entry1->d_name);
            if(strcmp(entry1->d_name, entry2->d_name) == 0){
                found = 1;
                if(entry2->d_type == DT_DIR){
                    if(is_same_dir(file1path, file2path)){
                        for(int i = depth; i < first_depth - 1; i++)
                            printf("    ");
                        printf("%s : staged\n", entry2->d_name);
                        char new_direc[1000];
                        strcpy(new_direc, direc);
                        strcat(new_direc, "/");
                        strcat(new_direc, entry1->d_name);
                        add_n(new_direc, depth, first_depth);
                        break;
                    }else{
                        for(int i = depth; i < first_depth - 1; i++)
                            printf("    ");
                        printf("%s : not staged\n", entry2->d_name);
                        char new_direc[1000];
                        strcpy(new_direc, direc);
                        strcat(new_direc, "/");
                        strcat(new_direc, entry1->d_name);
                        add_n(new_direc, depth, first_depth);
                        break;
                    }
                }else{
                    if(is_same_file(file1path, file2path)){
                        for(int i = depth; i < first_depth - 1; i++)
                            printf("    ");
                        printf("%s : staged\n", entry2->d_name);
                        break;
                    }else{
                        for(int i = depth; i < first_depth - 1; i++)
                            printf("    ");
                        printf("%s : not staged\n", entry2->d_name);
                        break;
                    }
                }
            }
        }
        if(!found){
            for(int i = depth; i < first_depth - 1; i++)
                printf("    ");
            printf("%s : not staged\n", entry2->d_name);
            char new_direc[1000];
            strcpy(new_direc, direc);
            strcat(new_direc, "/");
            strcat(new_direc, entry2->d_name);
            add_n(new_direc, depth, first_depth);
        }
    }
    return 0;
}


int add(char file_or_directory[], char repo[]){
    repo[strlen(repo) - 8] = '\0';
    char repo2[1000];
    strcpy(repo2, repo);
    char relative[1000];
    strcpy(relative, relative_path(file_or_directory));
    char la_path[1000];
    strcpy(la_path, repository_address());
    strcat(la_path, "/last add.txt");
    last_add = fopen(la_path, "w");
    char *token = strtok(relative, "/");
    int exist = 1;
    FILE *check = fopen(file_or_directory, "r");
    if(chdir(file_or_directory) != 0 && check == NULL)   
        exist = 0;
    chdir(repo);
    if(exist){
        strcat(repo, "/");
        strcat(repo, token);
        struct stat entry;
        stat(repo, &entry);
        if(S_ISDIR(entry.st_mode)){
            chdir(".teibur/staging area");
            add_directory(token, repo);
        }else{
            chdir(".teibur/staging area");
            add_file(token, repo);
        }
    }else{
        fputs(relative_path(file_or_directory), last_add);
        chdir(staging_area());
        char command[1000];
        sprintf(command, "rm -r \"%s\"", relative_path(file_or_directory));
        system(command);
    }
}


int redo(){
    char temp[1000];
    getcwd(temp, 1000);
    char repo[1000];
    chdir(repository_address());
    chdir("../");
    getcwd(repo, 1000);
    struct dirent *entry;
    DIR *dir = opendir(".");
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            char command[2000];
            sprintf(command, "rm -r \"%s\"", entry->d_name);
            system(command);
        }
    }
    closedir(dir);
    chdir(staging_area());
    dir = opendir(".");
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            char command[2000];
            sprintf(command, "cp -r \"%s\" \"%s\"", entry->d_name, repo);
            system(command);
        }
    }
    return 0;
}


int reset(char path[]){
    int exist = 0;
    char in_staging[1000];
    strcpy(in_staging, staging_area());
    strcat(in_staging, "/");
    strcat(in_staging, relative_path(path));
    FILE *check = fopen(in_staging, "r");
    if(chdir(in_staging) == 0 || check != NULL){
        char command[2000];
        sprintf(command, "rm -r \"%s\"", in_staging);
        system(command);
        exist = 1;
    }
    char in_commit[1000];
    strcpy(in_commit, last_commit_in_branch(current_branch()));
    strcat(in_commit, "/");
    strcat(in_commit, relative_path(path));
    check = fopen(in_commit, "r");
    if(chdir(in_commit) == 0 || check != NULL){
        char tokenize1[1000];
        strcpy(tokenize1, relative_path(path));
        char tokenize2[1000];
        strcpy(tokenize2, relative_path(path));
        int toknum = 0;
        strtok(tokenize1, "/");
        while(strtok(NULL, "/") != NULL)
            toknum++;
        if(toknum == 0){
            char command[3000];
            sprintf(command, "cp -r \"%s\" \"%s\"", in_commit, in_staging);
            system(command);
        }else{
            chdir(staging_area());
            char *token = strtok(tokenize2, "/");
            mkdir(token, 0755);
            chdir(token);
            for(int i = 1; i < toknum; i++){
                token = strtok(NULL, "/");
                mkdir(token, 0755);
                chdir(token);
            }
            char command[3000];
            sprintf(command, "cp -r \"%s\" \"%s\"", in_commit, in_staging);
            system(command);

        }
        exist = 1;
    }
    if(!exist){
        perror("File not found");
        return 1;
    }
    return 0;
}


int status(char path[], int tabs[]){
    char in_commit[1000];
    chdir(last_commit_in_branch(current_branch()));
    chdir(path);
    getcwd(in_commit, sizeof(in_commit));
    char in_staging[1000];
    chdir(staging_area());
    chdir(path);
    getcwd(in_staging, sizeof(in_staging));
    char in_repo[1000];
    strcpy(in_repo, repository_address());
    in_repo[strlen(in_repo) - 8] = '\0';
    chdir(in_repo);
    chdir(path);
    getcwd(in_repo, sizeof(in_repo));
    DIR *dir = opendir(in_repo);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] == '.')
            continue;
        if(entry->d_type == DT_DIR){
            char entry_repo[1000];
            strcpy(entry_repo, in_repo);
            strcat(entry_repo, "/");
            strcat(entry_repo, entry->d_name);
            status(relative_path(entry_repo), 0);
        }else{
            char entry_repo[1000];
            strcpy(entry_repo, in_repo);
            strcat(entry_repo, "/");
            strcat(entry_repo, entry->d_name);
            char entry_staging[1000];
            strcpy(entry_staging, in_staging);
            strcat(entry_staging, "/");
            strcat(entry_staging, entry->d_name);
            char entry_commit[1000];
            strcpy(entry_commit, in_commit);
            strcat(entry_commit, "/");
            strcat(entry_commit, entry->d_name);
            FILE *file = fopen(entry_commit, "r");
            if(file == NULL){
                if(is_same_file(entry_repo, entry_staging))
                    printf("%s +A\n", relative_path(entry_repo));
                else printf("%s -A\n", relative_path(entry_repo));
            }else if(!is_same_file(entry_commit, entry_repo)){
                if(is_same_file(entry_repo, entry_staging))
                    printf("%s +M\n", relative_path(entry_repo));
                else printf("%s -M\n", relative_path(entry_repo));
            }
        }
    }
    return 0;
}


int status_deleted(char path[], char just_name[]){
    char in_commit[1000];
    chdir(last_commit_in_branch(current_branch()));
    chdir(path);
    getcwd(in_commit, sizeof(in_commit));
    char in_staging[1000];
    strcpy(in_staging, staging_area());
    if(strcmp(path, ".") == 0){
        chdir(staging_area());
        chdir(path);
        getcwd(in_staging, sizeof(in_staging));
    }else{
        strcat(in_staging, "/");
        strcat(in_staging, path);
    }
    char in_repo[1000];
    strcpy(in_repo, repository_address());
    in_repo[strlen(in_repo) - 8] = '\0';
    if(strcmp(path, ".") == 0){
        chdir(in_repo);
        chdir(path);
        getcwd(in_repo, sizeof(in_repo));
    }else{
        strcat(in_repo, "/");
        strcat(in_repo, path);
    }

    DIR *dir = opendir(in_commit);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] == '.')
            continue;
        if(entry->d_type == DT_DIR){
            char entry_repo[1000];
            strcpy(entry_repo, in_repo);
            strcat(entry_repo, "/");
            strcat(entry_repo, entry->d_name);
            status_deleted(relative_path(entry_repo), 0);
        }else{
            char entry_repo[1000];
            strcpy(entry_repo, in_repo);
            strcat(entry_repo, "/");
            strcat(entry_repo, entry->d_name);
            char entry_staging[1000];
            strcpy(entry_staging, in_staging);
            strcat(entry_staging, "/");
            strcat(entry_staging, entry->d_name);
            FILE *file = fopen(entry_repo, "r");
            if(file != NULL)
                continue;
            file = fopen(entry_staging, "r");
            if(file == NULL)
                printf("%s +D\n", relative_path(entry_repo));
            else printf("%s -D\n", relative_path(entry_repo));
        }
    }
}


int commit(char message[]){
    if(strcmp(current_branch(), "not HEAD") == 0){
        perror("You can't commit here");
        return 1;
    }
    if(strlen(message) > 72){
        perror("message is too long");
        return 1;
    }
    if(is_same_dir(last_commit_in_branch(current_branch()), staging_area())){
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
    chdir(master());
    int commit_id;
    char commit_id_s[10];
    FILE *id_file = fopen("id.txt", "r");
    fgets(commit_id_s, 10, id_file);
    fclose(id_file);
    id_file = fopen("id.txt", "w");
    chdir(current_branch());
    FILE *last_one = fopen("last.txt", "w");
    fputs(commit_id_s, last_one);
    char dest[1000];
    getcwd(dest, sizeof(dest));
    strcat(dest, "/");
    strcat(dest, commit_id_s);
    sscanf(commit_id_s, "%d", &commit_id);
    sprintf(command, "cp -r \"%s\" \"%s\"", staging_area(), dest);
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


int print_log(char commit_p[], char branch_[], char author[], char search[]){
    chdir(commit_p);
    chdir("../");
    char temp1[1000];
    getcwd(temp1, 1000);
    char id[10];
    strcpy(id, relative_path2(commit_p, temp1));
    chdir("../");
    char temp2[1000];
    getcwd(temp2, 1000);
    char branch[100];
    strcpy(branch, relative_path2(temp1, temp2));
    if(strcmp(branch, "master") != 0){
        strcpy(branch, branch + 1);
    }
    if(branch_ != NULL){
        if(strcmp(branch_, branch) != 0)
            return 0;
    }
    strcat(commit_p, ".txt");
    FILE *info = fopen(commit_p, "r");
    char message[100];
    char time[100];
    char author_name[100];
    char author_email[100];
    fgets(author_name, 100, info);
    fgets(author_email, 100, info);
    fgets(message, 100, info);
    fgets(time, 100, info);
    author_name[strlen(author_name) - 1] = '\0';
    if(author != NULL){
        if(strcmp(author, author_name) != 0)
            return 0;
    }
    if(search != NULL){
        if(strstr(message, search) == 0)
            return 0;
    }
    printf("%stime : %sid : %s\nauthor name : %s\nauthor email : %sbranch : %s\n\n", message, time, id, author_name, author_email, branch);
}


int branch_exist(char address[], char name[]){
    chdir(address);
    struct stat entry;
    if(stat(name, &entry) == 0){
        return 1;
    }
    DIR *dir = opendir(address);
    struct dirent *entryy;
    while((entryy = readdir(dir)) != NULL){
        if(entryy->d_name[0] == '-'){
            char addr[1000];
            strcpy(addr, address);
            strcat(addr, "/");
            strcat(addr, entryy->d_name);
            if(branch_exist(addr, name))
                return 1;
        }
    }
    return 0;
}


char *absolute_current_branch(){
    char file_address[1000];
    strcpy(file_address, repository_address());
    strcat(file_address, "/branch.txt");
    FILE *branch = fopen(file_address, "r");
    char branch_address[1000];
    fgets(branch_address, 1000, branch);
    if(branch_address[strlen(branch_address) - 1] == '\n')
        branch_address[strlen(branch_address) - 1] = '\0';
    if(strcmp(branch_address, "not HEAD") == 0){
        fgets(branch_address, 1000, branch);
        if(branch_address[strlen(branch_address) - 1] == '\n')
            branch_address[strlen(branch_address) - 1] = '\0';
    }
    char *return_value = (char *)malloc(1000);
    strcpy(return_value, branch_address);
    return return_value;
}


char *branch_path(char address[], char name[]){
    char temp[1000];
    getcwd(temp, 1000);
    chdir(address);
    struct stat entry;
    if(stat(name, &entry) == 0){
        strcat(address, "/");
        strcat(address, name);
        chdir(temp);
        return address;
    }
    DIR *dir = opendir(address);
    struct dirent *entryy;
    while((entryy = readdir(dir)) != NULL){
        if(entryy->d_name[0] == '-'){
            char addr[1000];
            strcpy(addr, address);
            strcat(addr, "/");
            strcat(addr, entryy->d_name);
            char addrr[1000];
            strcpy(addrr, addr);
            if(branch_path(addr, name) != NULL){
                chdir(temp);
                char *return_value = (char *)malloc(1000);
                strcpy(return_value, branch_path(addrr, name));
                return return_value;
            }
        }
    }
    chdir(temp);
    return NULL;
}


int uncommited_changes(){
    if(strcmp(current_branch(), "not HEAD") == 0);
        return 0;
    char from[1000];
    strcpy(from, repository_address());
    from[strlen(from) - 8] = '\0';
    char command[3000];
    sprintf(command, "cp -r \"%s\" \"%s\"", from, "/home/mohammad/teibur files/temp");
    system(command);
    system("rm -r \"/home/mohammad/teibur files/temp/.teibur\"");
    if(!is_same_dir(last_commit_in_branch(current_branch()), "/home/mohammad/teibur files/temp") || !is_same_dir(last_commit_in_branch(current_branch()), staging_area())){
        perror("You have uncommited changes");
        system("rm -r \"/home/mohammad/teibur files/temp\"");
        return 1;
    }
    system("rm -r \"/home/mohammad/teibur files/temp\"");
    return 0;
}


int go_to_commit(char commit_p[1000]){
    if(uncommited_changes())
        return 1;
    char temp[1000];
    getcwd(temp, 1000);
    char repo[1000];
    chdir(repository_address());
    chdir("../");
    getcwd(repo, 1000);
    // clear working directory
    struct dirent *entry;
    DIR *dir = opendir(".");
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            char command[2000];
            sprintf(command, "rm -r \"%s\"", entry->d_name);
            system(command);
        }
    }
    closedir(dir);
    chdir(staging_area());
    // clear staging area
    dir = opendir(".");
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            char command[2000];
            sprintf(command, "rm -r \"%s\"", entry->d_name);
            system(command);
        }
    }
    closedir(dir);
    chdir(commit_p);
    dir = opendir(".");
    // copy commit files
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            char command[2000];
            sprintf(command, "cp -r \"%s\" \"%s\"", entry->d_name, repo);
            system(command);
            sprintf(command, "cp -r \"%s\" \"%s\"", entry->d_name, staging_area());
            system(command);
        }
    }
    return 0;
}


int make_branch(char name[]){
    char branch_name[100] = "-";
    strcat(branch_name, name);
    if(branch_exist(master(), branch_name)){
        perror("branch already exists");
        return 1;
    }
    char a_c_b[1000];
    strcpy(a_c_b, absolute_current_branch());
    chdir(a_c_b);
    mkdir(branch_name, 0755);
    chdir(branch_name);
    FILE *last_one = fopen("last.txt", "w");
    char back[1000];
    chdir(last_commit_in_branch(a_c_b));
    realpath("..", back);
    fputs(relative_path2(last_commit_in_branch(a_c_b), back), last_one);
    return 0;
}


int list_branch(char file_address[], int dashes){
    dashes++;
    DIR *dir = opendir(file_address);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] == '-'){
            for(int i = 0; i < dashes; i++)
                printf("-");
            puts(entry->d_name);
            char file_address_again[1000];
            strcpy(file_address_again, file_address);
            strcat(file_address_again, "/");
            strcat(file_address_again, entry->d_name);
            list_branch(file_address_again, dashes);
        }
    }
}


int set_branch(char writeit[]){
    char repo_branch[1000];
    strcpy(repo_branch, repository_address());
    strcat(repo_branch, "/branch.txt");
    FILE *branch = fopen(repo_branch, "w");
    fputs(writeit, branch);
}


int checkout_commit(char c_path[], char b_path[]){
    if(go_to_commit(c_path) != 0)
        return 1;
    char thing[1000];
    sprintf(thing, "not HEAD\n%s", b_path);
    set_branch(thing);
}


int checkout(char branch_[100]){
    if(!branch_exist(master(), branch_)){
        perror("branch doesn't exists");
        return 1;
    }
    char b_p[1000];
    strcpy(b_p, branch_path(master(), branch_));
    if(go_to_commit(last_commit_in_branch(b_p)) != 0)
        return 1;
    set_branch(b_p);
    return 0;
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
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        if(strcmp(argv[2], "-redo") == 0){
            redo();
            return 0;
        }
        if(strcmp(argv[2], "-f") == 0){
            for(int i = 3; i < argc; i++){
                chdir(cwd);
                char real_arg[1000];
                realpath(argv[i], real_arg);
                add(real_arg, repository_address());
            }
        }if(strcmp(argv[2], "-n") == 0){
            char real_arg[1000];
            getcwd(real_arg, sizeof(real_arg));
            int depth;
            sscanf(argv[3], "%d", &depth);
            add_n(real_arg, depth, depth);
        }else{
            for(int i = 2; i < argc; i++){
                chdir(cwd);
                char real_arg[1000];
                realpath(argv[i], real_arg);
                add(real_arg, repository_address());
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
            FILE *last_add = fopen(file_path, "r");
            while(fgets(saved_path, 1000, last_add) != NULL){
                if(saved_path[strlen(saved_path) - 1] == '\n')
                    saved_path[strlen(saved_path) - 1] = '\0';
                char reposi[1000];
                strcpy(reposi, repository_address());
                reposi[strlen(reposi) - 8] = '\0';
                chdir(reposi);
                char r_s_p[1000];
                realpath(saved_path, r_s_p);
                puts(r_s_p);
                reset(r_s_p);
            }
            return 0;
        }
        char real_arg[1000];
        realpath(argv[2], real_arg);
        reset(real_arg);
    }else if(strcmp(argv[1], "status") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        if(argc != 2){
            perror("invalid command");
            return 1;
        }
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        char repo[1000];
        strcpy(repo, repository_address());
        repo[strlen(repo) - 8] = '\0';
        status(".", 0);
        status_deleted(".", 0);
        return 0;
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
    }else if(strcmp(argv[1], "log") == 0){
        if(argc == 2){
            char iidd[10];
            strcpy(iidd, last_commit());
            while(strcmp(iidd, "3426") != 0){
                print_log(commit_path(master(), iidd), NULL, NULL, NULL);
                strcpy(iidd, minus_one(iidd));
            }
        }else if(argc == 4){
            if(strcmp(argv[2], "-n") == 0){
                int n;
                sscanf(argv[3], "%d", &n);
                char iidd[10];
                strcpy(iidd, last_commit());
                for(int i = 0; i < n; i++){
                    if(strcmp(iidd, "3426") == 0)
                        break;
                    print_log(commit_path(master(), iidd), NULL, NULL, NULL);
                    strcpy(iidd, minus_one(iidd));
                }
            }else if(strcmp(argv[2], "-branch") == 0){
                char iidd[10];
                strcpy(iidd, last_commit());
                char checkexist[100] = "-";
                strcat(checkexist, argv[3]);
                if(!branch_exist(master(), checkexist) && strcmp(argv[3], "master") != 0){
                    perror("Branch doesn't exist");
                    return 1;
                }
                while(strcmp(iidd, "3426") != 0){
                    print_log(commit_path(master(), iidd), argv[3], NULL, NULL);
                    strcpy(iidd, minus_one(iidd));
                }
            }else if(strcmp(argv[2], "-author") == 0){
                char iidd[10];
                strcpy(iidd, last_commit());
                while(strcmp(iidd, "3426") != 0){
                    print_log(commit_path(master(), iidd), NULL, argv[3], NULL);
                    strcpy(iidd, minus_one(iidd));
                }
            }else if(strcmp(argv[2], "-search") == 0){
                char iidd[10];
                strcpy(iidd, last_commit());
                while(strcmp(iidd, "3426") != 0){
                    print_log(commit_path(master(), iidd), NULL, NULL, argv[3]);
                    strcpy(iidd, minus_one(iidd));
                }
            }
        }else{
            perror("invalid command");
        }
    }else if(strcmp(argv[1], "branch") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        if(argc == 3){
            if(strcmp(argv[2], "master") == 0){
                perror("Seriously bro?");
                return 1;
            }
            make_branch(argv[2]);
            return 0;
        }
        if(argc == 2){
            puts("master");
            list_branch(master(), -1);
            return 0;
        }
        perror("invalid command");
    }else if(strcmp(argv[1], "checkout") == 0){
        if(repository_address() == NULL){
            perror("You are not in a repository");
            return 1;
        }
        if(strcmp(argv[2], "HEAD") == 0){
            if(go_to_commit(last_commit_in_branch(absolute_current_branch())) != 0)
                return 1;
            set_branch(absolute_current_branch());
            return 0;
        }
        if(strcmp(argv[2], "master") == 0){
            if(go_to_commit(last_commit_in_branch(master())) != 0)
                return 1;
            set_branch(master());
            return 0;
        }
        if(commit_path(master(), argv[2]) != NULL){
            char arg_path[1000];
            strcpy(arg_path, commit_path(master(), argv[2]));
            char cwd_temp[1000];
            getcwd(cwd_temp, 1000);
            chdir(arg_path);
            chdir("../");
            char c_b_path[1000];
            getcwd(c_b_path, 1000);
            chdir(cwd_temp);
            if(strcmp(last_commit_in_branch(c_b_path), arg_path) == 0){
                if(go_to_commit(arg_path) != 0);
                    return 1;
                set_branch(c_b_path);
                return 0;
            }
            checkout_commit(arg_path, c_b_path);
            return 0;
        }
        char branch_name[100] = "-";
        strcat(branch_name, argv[2]);
        checkout(branch_name);
    }
    else{
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