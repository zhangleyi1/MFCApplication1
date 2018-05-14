#include "zip_helper.h"  
  
  
#ifndef _WIN32  
#ifndef __USE_FILE_OFFSET64  
#define __USE_FILE_OFFSET64  
#endif  
#ifndef __USE_LARGEFILE64  
#define __USE_LARGEFILE64  
#endif  
#ifndef _LARGEFILE64_SOURCE  
#define _LARGEFILE64_SOURCE  
#endif  
#ifndef _FILE_OFFSET_BIT  
#define _FILE_OFFSET_BIT 64  
#endif  
#endif  
  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <errno.h>  
#include <fcntl.h>
  
#include <direct.h>  
#include <io.h>  
  
#include "zlib.h"  
#include "zip.h"  
  
#ifdef _WIN32  
#define USEWIN32IOAPI  
#include "iowin32.h"  
#endif  
  
#define WRITEBUFFERSIZE (16384)  
#define MAXFILENAME (256)  
  

#ifdef _WIN32  
  
static char* Utf8ToAnsi(const char* utf8)    
{    
    // 先转换为UNICODE  
    int dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,utf8,-1,NULL,0);    
    if(!dwUnicodeLen)    
    {    
        return strdup(utf8);    
    }    
    size_t num = dwUnicodeLen*sizeof(wchar_t);    
    wchar_t *pwText = (wchar_t*)malloc(num+2);    
    memset(pwText,0,num+2);    
    MultiByteToWideChar(CP_UTF8,0,utf8,-1,pwText,dwUnicodeLen);  
      
    // 再转换为ANSI  
    int len;    
    len = WideCharToMultiByte(CP_ACP, 0, pwText, -1, NULL, 0, NULL, NULL);    
    char *szANSI = (char*)malloc(len + 1);    
    memset(szANSI, 0, len + 1);    
    WideCharToMultiByte(CP_ACP, 0, pwText, -1, szANSI, len, NULL,NULL);    
  
    free(pwText);  
    return (char*)szANSI;    
}   
  
static wchar_t* QXUtf82Unicode(const char* utf)    
{    
    if(!utf || !strlen(utf))    
    {    
        return NULL;    
    }    
    int dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,utf,-1,NULL,0);    
    size_t num = dwUnicodeLen*sizeof(wchar_t);    
    wchar_t *pwText = (wchar_t*)malloc(num);    
    memset(pwText,0,num);    
    MultiByteToWideChar(CP_UTF8,0,utf,-1,pwText,dwUnicodeLen);    
    return pwText;    
}

static char* QXUnicode2Utf8(const wchar_t* unicode)    
{    
    if(unicode == NULL)
	{
		return strdup("\0");
	}

    int len;  
    len = WideCharToMultiByte(CP_UTF8, 0,unicode, -1, NULL, 0, NULL, NULL);  
    char *szUtf8 = (char*)malloc(len + 1);  
    memset(szUtf8, 0, len + 1);  
    WideCharToMultiByte(CP_UTF8, 0,unicode, -1, szUtf8, len, NULL,NULL);  
    return szUtf8;   
}   
  
static FILE* ZipFopen(const char* path,const char* mode)  
{  
    wchar_t* path_u = QXUtf82Unicode(path);  
    wchar_t* mode_u = QXUtf82Unicode(mode);  
    FILE* file = _wfopen(path_u,mode_u);  
    free(path_u);  
    free(mode_u);  
    return file;  
}  
  
/* name of file to get info on */  
/* return value: access, modific. and creation times */  
/* dostime */  
uLong filetime(const char* f, tm_zip *tmzip, uLong *dt)            
{  
    int ret = 0;  
    {  
        FILETIME ftLocal;  
        HANDLE hFind;  
        WIN32_FIND_DATA ff32;  
        wchar_t *unicode = QXUtf82Unicode(f);  
        hFind = FindFirstFile(unicode,&ff32);  
        if (hFind != INVALID_HANDLE_VALUE)  
        {  
            FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);  
            FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);  
            FindClose(hFind);  
            ret = 1;  
        }  
        free(unicode);  
    }  
    return ret;  
}  
#else  
#define ZipFopen fopen;  
#endif  
  
ZipHelper& ZipHelper::AddFile(const char* input_path,const char* inzip_path)  
{  
    files_[input_path] = inzip_path;  
    return *this;  
}

ZipHelper& ZipHelper::AddDir(const char* input_dir,const char* temp_dir){
	WIN32_FIND_DATA findFileData;
	auto input_dirw = QXUtf82Unicode(input_dir);

	std::wstring path(input_dirw);
	auto pos = path.at(path.size()-1);
	if(pos != L'\\' && pos != L'/'){
		path.append(L"/");
	}
	HANDLE hFind  = ::FindFirstFile((path+L"*").c_str(),&findFileData);
	size_t file_name_len = 0;
	if (hFind == INVALID_HANDLE_VALUE){  
		free(input_dirw);
		return *this;  
	} else {

		if(!temp_dir){
			temp_dir = input_dir;
		}
		std::string temp_dir2(temp_dir);
		pos = temp_dir2.at(temp_dir2.size()-1);
		if(pos != '\\' && pos != '/'){
			temp_dir2.append("/");
		}

		while (FindNextFile(hFind, &findFileData)){ 

			if(!wcscmp(findFileData.cFileName,L".") || !wcscmp(findFileData.cFileName,L".."))
				continue;

			std::wstring path1(path);
			path1.append(findFileData.cFileName);
			char* path_utf8 = QXUnicode2Utf8(path1.c_str());
			if(findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
				AddDir(path_utf8,temp_dir);
			}else{
				AddFile(path_utf8,path_utf8+strlen(temp_dir2.c_str()));
			}
			free(path_utf8);
		}  
		FindClose(hFind);  
	}
	free(input_dirw);
}


bool ZipHelper::ToZip(const char* output_path)  
{  
    int err=0;  
    zipFile zf;  
    int errclose;  
    int opt_compress_level = Z_DEFAULT_COMPRESSION;  
#ifdef USEWIN32IOAPI  
    zlib_filefunc64_def ffunc;  
    fill_win32_filefunc64W(&ffunc);  
    wchar_t* temp_path = QXUtf82Unicode(output_path);  
    zf = zipOpen2_64(temp_path,APPEND_STATUS_CREATE,NULL,&ffunc);  
    free(temp_path);  
#else  
    zf = zipOpen64(output_path,APPEND_STATUS_CREATE);  
#endif  
  
    if (zf == NULL)  
    {  
        printf("error opening %s\n",output_path);  
        err= ZIP_ERRNO;  
        return false;  
    }  
  
    void* buf = NULL;  
    int size_buf = WRITEBUFFERSIZE;  
    buf = (void*)malloc(size_buf);  
	for (auto ite = files_.begin(); ite != files_.end(); ++ite)  
    {  
        FILE * fin;  
        int size_read;  
		const char* filenameinzip = ite->first.c_str();
		const char *savefilenameinzip = ite->second.c_str();  
        zip_fileinfo zi;  
        unsigned long crcFile=0;  
        int zip64 = 0;  
  
        zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =  
            zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;  
        zi.dosDate = 0;  
        zi.internal_fa = 0;  
        zi.external_fa = 0;  
        filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);  
  
		const char* pos = NULL;  
		if(!strlen(savefilenameinzip)){
			savefilenameinzip = filenameinzip;
			if( (pos = strrchr(savefilenameinzip,'\\'))   
				|| (pos = strrchr(savefilenameinzip,'/')) ){  
					pos++;  
			}else{  
				pos = savefilenameinzip;  
			}
		}else{
			pos = savefilenameinzip;
		}
          
        // 这个版本不支持UTF8字符串的正确存储,所以要转换为ANSI显示.  
        char* pos_ansi = Utf8ToAnsi(pos);  
        err = zipOpenNewFileInZip3_64(zf,pos_ansi,&zi,  
            NULL,0,NULL,0,NULL,  
            (opt_compress_level != 0) ? Z_DEFLATED : 0,  
            opt_compress_level,0,  
            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,  
            NULL,crcFile, zip64);  
        free(pos_ansi);  
  
        if (err != ZIP_OK)  
        {  
            printf("error in opening %s in zipfile\n",pos);  
        }  
        else  
        {  
            fin = ZipFopen(filenameinzip,"rb");  
            if (fin==NULL)  
            {  
                err=ZIP_ERRNO;  
                printf("error in opening %s for reading\n",filenameinzip);  
            }  
        }  
  
        if (err == ZIP_OK)  
            do  
            {  
                err = ZIP_OK;  
                size_read = (int)fread(buf,1,size_buf,fin);  
                if (size_read < size_buf)  
                {  
                    if (feof(fin)==0)  
                    {  
                        printf("error in reading %s\n",filenameinzip);  
                        err = ZIP_ERRNO;  
                    }  
                }  
                if (size_read>0)  
                {  
                    err = zipWriteInFileInZip (zf,buf,size_read);  
                    if (err<0)  
                    {  
                        printf("error in writing %s in the zipfile\n",  
                            filenameinzip);  
                    }  
                }  
            } while ((err == ZIP_OK) && (size_read>0));  
  
            if(fin)  
            {  
                fclose(fin);  
            }  
  
            if (err<0)  
            {  
                err=ZIP_ERRNO;  
            }  
            else  
            {  
                err = zipCloseFileInZip(zf);  
                if (err!=ZIP_OK)  
                {  
                    printf("error in closing %s in the zipfile\n",filenameinzip);  
                }  
  
            }  
    }  
    errclose = zipClose(zf,NULL);  
    if (errclose != ZIP_OK)  
    {  
        printf("error in closing %s\n",output_path);  
        return false;  
    }  
    return true;  
}