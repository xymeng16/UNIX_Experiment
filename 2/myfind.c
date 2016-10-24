/********************************
Unix Experiment 1: Traversal of directory-tree
Author: Xiangyi Meng
ID: 22920142203887
Date: 15th Oct, 2016
Usage: myfind  <pathname>  [-comp <filename> | -name <str>…]
********************************/
#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
/* function type that is called for each filename */
typedef	int	Myfunc(const char *, const struct stat *, int);

static Myfunc	myfunc;
static Myfunc myfunc_1;
static Myfunc myfunc_2;
static int		myftw(char *, Myfunc *);
static int		dopath(Myfunc *);
static char **names;
static int name_count;
static long	nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot,nlt1024;
char *buffer;
off_t bufsize;
int
main(int argc, char *argv[])
{
	int		ret;

	if (argc<2 || argc == 3)
		err_quit("usage: myfind  <pathname>  [-comp <filename> | -name <str>…]");
	if(argc==2){
	ret = myftw(argv[1], myfunc);		/* does it all */

	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if (ntot == 0)
		ntot = 1;		/* avoid divide by 0; print 0 for all counts */
	printf("regular files  = %7ld, %5.2f %%\n", nreg,
	  nreg*100.0/ntot);
	printf("directories    = %7ld, %5.2f %%\n", ndir,
	  ndir*100.0/ntot);
	printf("block special  = %7ld, %5.2f %%\n", nblk,
	  nblk*100.0/ntot);
	printf("char special   = %7ld, %5.2f %%\n", nchr,
	  nchr*100.0/ntot);
	printf("FIFOs          = %7ld, %5.2f %%\n", nfifo,
	  nfifo*100.0/ntot);
	printf("symbolic links = %7ld, %5.2f %%\n", nslink,
	  nslink*100.0/ntot);
	printf("sockets        = %7ld, %5.2f %%\n", nsock,
	  nsock*100.0/ntot);
	printf("reg <= 4096    = %7ld, %5.2f %%\n",nlt1024,
	  nlt1024*100.0/nreg);
	exit(ret);
	}
	if(argc>=4&&strcmp("-name",argv[2])==0)
	{
	    name_count=argc-3;
	    names=argv+3;
	    ret=myftw(argv[1],myfunc_2);
	}
	else if(argc==4)
	{
	    if(strcmp("-comp",argv[2])!=0)
	    {
		err_quit("myfind  <pathname>  [-comp <filename> | -name <str>…]");
	    }
	    struct stat filestat;
	    if(stat(argv[3],&filestat)<0)
	    {
		err_ret("stat error");
	    }
	    bufsize=filestat.st_size;
	    buffer=malloc(bufsize+1);
	    int fid;
	    if((fid=open(argv[3],O_RDONLY))<0)
		err_sys("open error");
	    if(read(fid,buffer,bufsize)!=bufsize)
		err_sys("read error");
	    ret = myftw(argv[1], myfunc_1);
	}
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define	FTW_F	1		/* file other than directory */
#define	FTW_D	2		/* directory */
#define	FTW_DNR	3		/* directory that can't be read */
#define	FTW_NS	4		/* file that we can't stat */

static char	*fullpath;		/* contains full pathname for every file */
static size_t pathlen;

static int					/* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
	fullpath = path_alloc(&pathlen);	/* malloc PATH_MAX+1 bytes */
										/* ({Prog pathalloc}) */
	if (pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath, pathname);
	return(dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int					/* we return whatever func() returns */
dopath(Myfunc* func)
{
	struct stat		statbuf;
	struct dirent	*dirp;
	DIR				*dp;
	int				ret, n;

	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		return(func(fullpath, &statbuf, FTW_NS));
	if (S_ISDIR(statbuf.st_mode) == 0)	/* not a directory */
		return(func(fullpath, &statbuf, FTW_F));

	/*
	 * It's a directory.  First call func() for the directory,
	 * then process each filename in the directory.
	 */
	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return(ret);

	n = strlen(fullpath);
	if (n + NAME_MAX + 2 > pathlen) {	/* expand path buffer */
		pathlen *= 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	fullpath[n++] = '/';
	fullpath[n] = 0;
	printf("Before open DIR:%s\t",fullpath);
	if ((dp = opendir(fullpath)) == NULL)	/* can't read directory */
		return(func(fullpath, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0  ||
		    strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */
		strcpy(&fullpath[n], dirp->d_name);	/* append name after "/" */
		if ((ret = dopath(func)) != 0)		/* recursive */
			break;	/* time to leave */
	}
	fullpath[n-1] = 0;	/* erase everything from slash onward */
	printf("Current DIR:%s\n",fullpath);
	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);
	return(ret);
}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	case FTW_F:
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:	nreg++;	if(statptr->st_size<=4096)nlt1024++;break;
		case S_IFBLK:	nblk++;		break;
		case S_IFCHR:	nchr++;		break;
		case S_IFIFO:	nfifo++;	break;
		case S_IFLNK:	nslink++;	break;
		case S_IFSOCK:	nsock++;	break;
		case S_IFDIR:	/* directories should have type = FTW_D */
			err_dump("for S_IFDIR for %s", pathname);
		}
	case FTW_D:
		ndir++;
		break;
	case FTW_DNR:
		err_ret("can't read directory %s", pathname);
		break;
	case FTW_NS:
		err_ret("stat error for %s", pathname);
		break;
	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return(0);
}

static int
myfunc_1(const char *pathname, const struct stat *statptr, int type)
{
    // Compare if the contents of the file[pathname] are same as that in the file described in the parameter
    if(statptr->st_size!=bufsize) // If the size is not equal, we can simply say that the two files are different.
	return 0;
    
    int fd=open(pathname,O_RDONLY);
    
    char *buf=malloc(100);
    if(read(fd,buf,bufsize)!=bufsize)
    {
	err_sys("read error");
    }
    if(memcmp(buf,buffer,bufsize)==0)
    {
    char *rp;
    realpath(pathname,rp);
    printf("%s\n",rp);
    }
    close(fd);
    return 0;
}

static int
myfunc_2(const char *pathname, const struct stat *statptr, int type)
{
    int last_slash_pos=-1;
    int t=0,path_len=strlen(pathname);
    while(pathname[t]!='\0')
    {
	if(pathname[t]=='/')
	    last_slash_pos=t;
	t++;
    }
    for(int i=0;i<name_count;i++)
    {
	if(strcmp(pathname+last_slash_pos+1,names[i])==0)
	{
	 char *rp;
         realpath(pathname,rp);
         printf("%s\n",rp);
    //printf("%s\n",pathname);
	    break;
	}
    }
    return 0;
}
