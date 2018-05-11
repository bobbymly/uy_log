#include "FileUtil.h"


AppendFile::AppendFile(std::string filename):fp_(fopen(filename.c_str(),"ae"))
{
    setbuffer(fp_,buf,sizeof(buf));
}


AppendFile::~AppendFile()
{
    close(fp_);
}

AppendFile::append(const char* log,size_t len)
{
    size_t n=this->write(logline,len);
	size_t remain=len-n;
	while(remain>0)
	{
		size_t res=this->write(logline+n,remain);
		if(res==0)
		{
			int err=ferror(fp_);
			if(err)fprintf(stderr,"AppendFile::append() failed\n");
			break;
		}
		n+=res;
		remain=len-n;
	}
}

size_t AppendFile::write(const char* log,size_t len)
{
    unlocked_write(log,1,len,fp_);
}



void AppendFile::flush()
{
    fflush(fp_);
}