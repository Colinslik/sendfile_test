#define _GNU_SOURCE        /* or _BSD_SOURCE or _SVID_SOURCE */
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <limits.h>

#define	LOCAL_COPY_DEFAULT		0
#define	LOCAL_COPY_BY_SENDFILE		1
//#define	LOCAL_COPY_METHOD	LOCAL_COPY_DEFAULT
#define       LOCAL_COPY_METHOD       LOCAL_COPY_BY_SENDFILE
#define TRUE 1
#define FALSE 0
//#define Src "/share/1/testroot1/testfile"
//#define Dest "/share/1/testroot1/Dest/recv"
#define Src "/share/Public/Smoking_Test_VM_01/Smoking_Test_VM_02-flat.vmdk"
#define Dest "/share/NTFS/01/Smoking_Test_VM_02-flat.vmdk"

int main(int argc, char** argv)
{
	int iRet=0, writein = -1;
	int fdSrc = open64(Src, O_RDONLY);
        int fdDst = open64(Dest, O_CREAT|O_WRONLY);
#if (LOCAL_COPY_METHOD == LOCAL_COPY_BY_SENDFILE)
	struct stat64 stat_buf;
        off64_t offset = 0LL;
	fstat64(fdSrc, &stat_buf);
	while (offset < stat_buf.st_size) {
  		size_t count;
  		off64_t remaining = stat_buf.st_size- offset;
  		if (remaining > SSIZE_MAX)
      			count = SSIZE_MAX;
  		else 
      			count = remaining;
  		writein = sendfile64 (fdDst, fdSrc, &offset, count);
		printf("sendfile64 writein: %x \n",writein);
  		if (writein == 0) {
     			break;
  		}
  		if (writein == -1) {
     		printf("error from sendfile: %s\n", strerror(errno));
     		goto exit_copy_file;
  		}
	}
	if (offset != stat_buf.st_size) {
		printf("incomplete transfer from sendfile: %lld of %lld bytes\n",writein ,(long long)stat_buf.st_size);
   		goto exit_copy_file;
 	}
#else
	int result;
	char buffer[1024*1024];
	result = read(fdSrc, buffer, 1024*1024);
	while(result>0)
	{
		writein = write(fdDst, buffer, result);
		result = read(fdSrc, buffer, 1024*1024);		
		printf("writein: %x \n",writein);
		if(writein < 0)
		{
			printf("write fail: %d \n",writein);
			break;
		}
	}
#endif

exit_copy_file:
	if (-1 != fdSrc)  close(fdSrc);
	if (-1 != fdDst)  close(fdDst);

	return iRet;
}
