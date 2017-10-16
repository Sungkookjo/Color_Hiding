#include "stdafx.h"

///////////////////////////////////////////////////
// 생성자
Basefw_FS::Basefw_FS()
{
	Basefile = NULL;
}

Basefw_FS::Basefw_FS(const char* path)
{
	Link(path);
}
// 생성자
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// 소멸자
Basefw_FS::~Basefw_FS()
{ 
	end = fs_list.end();
	cend = fs_clist.end();
	eend = fs_elist.end();

	for(iter=fs_list.begin() ; iter != end ; ++iter)
	{
		SAFE_DELETE(iter->Memory_Head);
		delete iter->file_name;
	}

	for(citer=fs_clist.begin() ; citer != cend ; ++citer)
	{
		delete citer->file_name;
	}

	for(eiter=fs_elist.begin() ; eiter != eend ; ++eiter)
	{
	}

	fclose(Basefile);
}
// 소멸자
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// 링크
// zip file open
int Basefw_FS::Link(const char *path)
{
	// 파일 오픈. 2진 파일 읽기용
	fopen_s(&Basefile, path, "rb");
	// 파일 읽기 실패. 
	if(Basefile==NULL) return FS_WRONGPATH;
	// 파일 초기화 ( 처음부터 읽기 )
	fseek(Basefile, 0, SEEK_SET);

	// 임시 변수
	ZIP_LOCALFILE_HEADER zhf;
	ZIP_CENTRALFILE_HEADER zch;
	ZIP_ENDCENTRAL_HEADER zeh;

	memset(&zhf, 0,sizeof(ZIP_LOCALFILE_HEADER));

	int signature=0;

	do
	{
		// siganture 읽기.
		fread(&signature, sizeof(int), 1, Basefile);

		// signature 에 따라 읽어올 데이타 분류
		if(signature == 0x04034b50)
		{
			// signature 설정
			zhf.local_file_header_signature = signature;
			// 파일 압축해제 위한 virsion 읽기 , 이때 파일이름 길이 설정됨. 왜? 몰라;;
			fread(&zhf.version_needed_to_extract, FILE_HEADER_SIZE-SIGNATURE_SIZE, 1, Basefile);

			// filename 할당, 읽기
			zhf.file_name = new unsigned char[zhf.file_name_length+1];
			fread(zhf.file_name, zhf.file_name_length, 1, Basefile);
			zhf.file_name[zhf.file_name_length] = '\0';

			if(zhf.extra_field_length > 0)
			{
				zhf.extra_location = ftell(Basefile);
				fseek(Basefile, zhf.extra_field_length, SEEK_CUR);
			}
			else zhf.extra_location = -1;

			zhf.file_location = ftell(Basefile);
			fseek(Basefile, zhf.compressed_size, SEEK_CUR);

			if(zhf.extra_field_length > 0)
			{
				fread(&zhf.crc_32_end, DATA_DESCRT_SIZE, 1, Basefile);
			};
			// 메모리 주소 초기화
			zhf.Memory_Head = NULL;
			// 리스트 추가
			fs_list.push_back(zhf);
		}
		else if(signature == 0x02014b50)
		{
			zch.central_file_header_signature = signature;
			fread(&zch.version_made_by, CENTRAL_HEADER_SIZE-SIGNATURE_SIZE, 1, Basefile);

			zch.file_name = new unsigned char[zch.filename_length+1];
			fread(zch.file_name, zch.filename_length, 1, Basefile);
			zch.file_name[zch.filename_length] = '\0';

			if(zch.extra_field_length > 0)
			{
				zch.extra_location = ftell(Basefile);
				fseek(Basefile, zch.extra_field_length, SEEK_CUR);
			}
			if(zch.file_comment_length > 0)
			{
				zch.file_comment_location = ftell(Basefile);
				fseek(Basefile, zch.file_comment_length, SEEK_CUR);
			}
			fs_clist.push_back(zch);
		}
		else if(signature == 0x06054b50)
		{
			zeh.end_of_central_dir_signature = signature;
			fread(&zeh.number_of_this_disk, ENDCENTRAL_HEADER_SIZE-SIGNATURE_SIZE, 1, Basefile);
			if(zeh.zipfile_comment_length > 0)
			{
				zeh.zipfile_comment_location = ftell(Basefile);
				fseek(Basefile, zeh.zipfile_comment_length, SEEK_CUR);
			}
			fs_elist.push_back(zeh);
		}

		// 신호 초기화
		signature=0;

		// 파일 끝까지.
	}while(!feof(Basefile));


	return FS_OK;
}
// 링크
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Read
// zip 파일 내에서 path 파일을 찾아 메모리에 풀어
// 메모리 주소 리턴.
unsigned char* Basefw_FS::Read(const char *path)
{
	end = fs_list.end();

	// fs_list 에서 검색
	for(iter=fs_list.begin() ; iter != end ; ++iter)
	{
		// 지정된 파일 검색
		if(stricmp(reinterpret_cast<const char*>(iter->file_name), path) == 0)
		{
			// 메모리에 풀어져 있다면
			if(iter->Memory_Head){
				return iter->Memory_Head;
			}else{
				// 메모리에 풀어놓음
				if(SetMemory((*iter))){
					return iter->Memory_Head;
				}
				return NULL;
			}
		}
	}

	cend = fs_clist.end();

	// fs_list 에서 검색
	for(citer=fs_clist.begin() ; citer != cend ; ++citer)
	{
		// 지정된 파일 검색
		if(stricmp(reinterpret_cast<const char*>(citer->file_name), path) == 0)
		{
			printf("i find %s \n",citer->file_name);
			// 메모리에 풀어져 있다면
			//if(citer->Memory_Head){
			//	return citer->Memory_Head;
			//}else{
			//	// 메모리에 풀어놓음
			//	if(SetMemory((*citer))){
			//		return citer->Memory_Head;
			//	}
			//	return NULL;
			//}
		}
	}

	return NULL;
}
// Read
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// ReadSize
// 리소스 메모리 사이즈 리턴
int Basefw_FS::ReadSize(const char *path)
{
	end = fs_list.end();
	for(iter=fs_list.begin() ; iter != end ; ++iter)
	{
		//지정된 리소스를 찾았음
		if(stricmp(reinterpret_cast<const char*>(iter->file_name), path) == 0)
		{
			return iter->uncompressed_size;
		}
	}

	return -1;
}
// ReadSize
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// SetMemory
// 메모리에 풀어 놓기
bool Basefw_FS::SetMemory(ZIP_LOCALFILE_HEADER& p_ZIP)
{
	// 메모리 할당
	unsigned char* tempout=new unsigned char[p_ZIP.uncompressed_size];
	unsigned char* tempin=new unsigned char[p_ZIP.compressed_size];

	// 파일 처음부터 읽기 ( 파일 커서 초기화 )
	fseek(Basefile, 0, SEEK_SET);
	// 불러올 리소스 부터 읽기 시작 ( 파일 커서 리소스 부분부터 )
	fseek(Basefile, p_ZIP.file_location, SEEK_SET);
	// tempin 에 파일 옴겨 쓰기
	fread(tempin, p_ZIP.compressed_size, 1, Basefile);

	// z_stream 초기화
	z.zalloc = (alloc_func)0;
	z.zfree = (free_func)0;
	z.opaque = (voidpf)0;

	z.next_in = tempin;
	z.avail_in = 0;
	z.next_out = tempout;

	int err = inflateInit2(&z,-(MAX_WBITS));

	while( z.total_out < (uLong)(p_ZIP.uncompressed_size) 
		&& z.total_in < (uLong)(p_ZIP.compressed_size) )
	{
		z.avail_in = z.avail_out = 4096;

		// 메모리에 압축해제

		err = inflate(&z,Z_NO_FLUSH); 
		// 파일 읽기 완료시
		if(err == Z_STREAM_END)
		{
			delete tempin;
			p_ZIP.Memory_Head = tempout;
			return true;
		}
		// z_stream 실패.
		if(err != Z_OK)
		{
			break;
		}
	}

	err = inflateEnd(&z);

	delete tempin;
	delete tempout;
	p_ZIP.Memory_Head = NULL;
	return false;
}
// SetMemory
///////////////////////////////////////////////////
