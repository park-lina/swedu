#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define LIST  1
#define INODE 2
#define RECUR 4
#define ALL   8

void get_file_permissions(struct stat *buff, char *perms) {
  char rwx[] = "rwx";
  char sst[] = "sst";
  
  if      (S_ISDIR(buff->st_mode))  perms[0] = 'd';
  else if (S_ISCHR(buff->st_mode))  perms[0] = 'c';
  else if (S_ISBLK(buff->st_mode))  perms[0] = 'b';
  else if (S_ISFIFO(buff->st_mode)) perms[0] = 'p';
  else if (S_ISLNK(buff->st_mode))  perms[0] = 'l';
  else if (S_ISSOCK(buff->st_mode)) perms[0] = 's';

  int i;
  for (i=0; i<9; i++) {
    if ((buff->st_mode >> (8-i)) & 0x1)
      perms[1+i] = rwx[i%3];
  }

  for (i=0; i<3; i++) {
    if ((buff->st_mode >> (11-i)) & 0x1) {
      if(perms[(i+1)*3] == '-')
        perms[(i+1)*3] = sst[i] - ('a'-'A');
      else
        perms[(i+1)*3] = sst[i];
    }
  }
}

void print_fpath(char *path, char type, int flag) {
  if (type == 'l' && (flag & LIST)) {
    char buff[256];
    int ret;
    ret = readlink(path, buff, sizeof buff);
    buff[ret] = 0;
    printf("%s -> %s\n", path, buff);
  }
  else {
    printf("%s\n", path);
  }
}

void print_llist(struct stat *buff, char *perms) {
  struct passwd *pwd;
  struct group  *grp;
  struct tm     *tmp;
  
  // 1. 파일 종류 및 권한
  printf("%s ", perms);

  // 2. 하드 링크 수
  int lwidth = 1;
  printf("%*d ", lwidth, buff->st_nlink);

  // 3. 사용자 및 그룹 ID
  pwd = getpwuid(buff->st_uid);
  grp = getgrgid(buff->st_gid);
  
  printf("%s ", pwd->pw_name);
  printf("%s ", grp->gr_name);

  // 4. 파일 크기
  if (perms[0] == 'c' || perms[0] == 'b') {
    printf("%d, %d ", (buff->st_rdev>>8) & 0xff, (buff->st_rdev) & 0xff);
  }
  else {
    int swidth = 5;
    int fsize = buff->st_size;
    printf("%*d ", swidth, fsize);
  }

  // 5. 날짜
  tmp = localtime(&buff->st_mtime);

  int month = tmp->tm_mon;
  char *m;
  switch(month) {
    case 0 : m = "Jan"; break;
    case 1 : m = "Feb"; break;
    case 2 : m = "Mar"; break;
    case 3 : m = "Apr"; break;
    case 4 : m = "May"; break;
    case 5 : m = "Jun"; break;
    case 6 : m = "Jul"; break;
    case 7 : m = "Aug"; break;
    case 8 : m = "Sep"; break;
    case 9 : m = "Oct"; break;
    case 10: m = "Nov"; break;
    case 11: m = "Dec"; break;
  };
  printf("%s %02d %02d:%02d ", m, tmp->tm_mday, tmp->tm_hour, tmp->tm_min);
}

void _ls(char *path, int flag) {
  struct stat buff;
  struct dirent *p;
  DIR *dp;
  int total;

  chdir(path);
  dp = opendir(path);

  while((p = readdir(dp))) {
    lstat(p->d_name, &buff);
    total += buff.st_blocks;
  }
  rewinddir(dp);

  if (flag & RECUR) printf("%s:\n", path);
  if (flag & LIST) printf("total %d\n", total - 1);

  while((p = readdir(dp))) {
    char perms[] = "-----------";

    if (lstat(p->d_name, &buff) != -1) {
      get_file_permissions(&buff, perms);
    }

    if (strncmp(p->d_name, ".", 1)) {
      if (flag & INODE) printf("%llu ", buff.st_ino);
      if (flag & LIST)  print_llist(&buff, perms);

      print_fpath(p->d_name, perms[0], flag);
      continue;
    }
    
    if (flag & ALL) printf("%s\t", p->d_name);
  }
  printf("\n");
  rewinddir(dp);
  
  if (flag & RECUR) {  
    while((p = readdir(dp))) {
      lstat(p->d_name, &buff);

      if (S_ISDIR(buff.st_mode)) {
        
        if (strcmp(p->d_name, ".") && strcmp(p->d_name, "..")) {
          _ls(p->d_name, flag);
        }
      }
    }
    closedir(dp);
    chdir("..");
  }
} 

int main(int argc, char *argv[]) {
  int ch;
  int flag = 0;

  while ((ch = getopt(argc, argv, "liRa")) != -1) {
    switch(ch) {
      case 'l' : flag |= LIST;  break; 
      case 'i' : flag |= INODE; break;
      case 'R' : flag |= RECUR; break; 
      case 'a' : flag |= ALL;   break;
    }
  }

  char *d_name = ".";
  if (argv[2] != NULL) d_name = argv[2];
  else                 d_name = ".";

  _ls(d_name, flag);
  return 0;
} 
