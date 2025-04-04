#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define SP_OPEN_DIR_ERR -1
#define SP_DIR_READ_ERR -2
#define SP_UNHANDLED -3
#define SP_DIR_SEP "===================================================================================================\n"


char get_file_type(mode_t mode);
int list_directory(const char*);

int main(int argc, char** argv){
    if (argc == 1){
        printf("Please enter at least one directory name.\n");
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        switch (list_directory(argv[i]))
            {
            case SP_OPEN_DIR_ERR:
                perror("Error opening directory");
                printf("Error opening directory: %s\n", argv[i]);
                break;
            case SP_DIR_READ_ERR:
                perror("Error reading directory");
                printf("Error reading directory: %s\n", argv[i]);
                break;
            case 0:
                break;
            default:
                perror("Unhandled\n");
                printf("Unhandled\n");
                return SP_UNHANDLED; 
                break;
        }
    }
    return 0;
}


int list_directory(const char *path_dir){
    DIR *dir = opendir(path_dir);
    if (dir == NULL){
        return SP_OPEN_DIR_ERR;
    }
    char buff[NAME_MAX] = {0};

    struct dirent* entry = {0};
    struct stat st = {0};
    int flag_first = 1;
    char full_path[PATH_MAX] = {0}; 
    while (1) {
        if ((entry = readdir(dir)) == NULL){
            closedir(dir);
            if (errno == EBADF) return SP_DIR_READ_ERR;
            else{
                close(dir);
                return 0;
            }
        }
        if (strcmp(entry->d_name, ".") == 0
        || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        
        if (flag_first == 1){
            printf(SP_DIR_SEP);
            printf("DIR: %s:\n", path_dir);
            flag_first = 0;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, entry->d_name);

        if (fstatat(AT_FDCWD, full_path, &st, AT_SYMLINK_NOFOLLOW) == -1){
            printf("Error getting stat: %s\n", full_path);
            continue;
        }
        mode_t mode_st = st.st_mode;

        int fd = open(full_path, O_RDONLY);
        if (fd == -1){
            printf("Error opening file: %s\n", full_path);
            continue;
        }
        struct fiemap fiemap = {0};
        fiemap.fm_start = 0;
        fiemap.fm_length = 1;
        fiemap.fm_flags = FIEMAP_FLAG_SYNC;
        fiemap.fm_extent_count = 1;
        //FIBMAP
        //FS_IOC_FIEMAP
        if (ioctl(fd, FS_IOC_FIEMAP, &fiemap) == -1){

            printf("Error getting byte offset of extent on disk: %s\n", full_path);
            close(fd);
            continue;
        }

        printf("%-15llu %c %s\n", fiemap.fm_extents[0].fe_physical, get_file_type(mode_st), entry->d_name);
        close(fd);  
    }
    closedir(dir);
    return 0;
}

char get_file_type(mode_t mode) {
    if (S_ISREG(mode))  return 'F';
    if (S_ISDIR(mode))  return 'D';
    if (S_ISCHR(mode))  return 'C';
    if (S_ISBLK(mode))  return 'B';
    if (S_ISFIFO(mode)) return 'P';
    if (S_ISLNK(mode))  return 'L';
    if (S_ISSOCK(mode)) return 's';
    return '?';
}