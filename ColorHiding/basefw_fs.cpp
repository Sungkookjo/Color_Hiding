#include "stdafx.h"

///////////////////////////////////////////////////
// ������
Basefw_FS::Basefw_FS()
{
	Basefile = NULL;
}

Basefw_FS::Basefw_FS(const char* path)
{
	Link(path);
}
// ������
///////////////////////////////////////////////////


///////////////////////////////////////////////////
// �Ҹ���
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
// �Ҹ���
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// ��ũ
// zip file open
int Basefw_FS::Link(const char *path)
{
	// ���� ����. 2�� ���� �б��
	fopen_s(&Basefile, path, "rb");
	// ���� �б� ����. 
	if(Basefile==NULL) return FS_WRONGPATH;
	// ���� �ʱ�ȭ ( ó������ �б� )
	fseek(Basefile, 0, SEEK_SET);

	// �ӽ� ����
	ZIP_LOCALFILE_HEADER zhf;
	ZIP_CENTRALFILE_HEADER zch;
	ZIP_ENDCENTRAL_HEADER zeh;

	memset(&zhf, 0,sizeof(ZIP_LOCALFILE_HEADER));

	int signature=0;

	do
	{
		// siganture �б�.
		fread(&signature, sizeof(int), 1, Basefile);

		// signature �� ���� �о�� ����Ÿ �з�
		if(signature == 0x04034b50)
		{
			// signature ����
			zhf.local_file_header_signature = signature;
			// ���� �������� ���� virsion �б� , �̶� �����̸� ���� ������. ��? ����;;
			fread(&zhf.version_needed_to_extract, FILE_HEADER_SIZE-SIGNATURE_SIZE, 1, Basefile);

			// filename �Ҵ�, �б�
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
			// �޸� �ּ� �ʱ�ȭ
			zhf.Memory_Head = NULL;
			// ����Ʈ �߰�
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

		// ��ȣ �ʱ�ȭ
		signature=0;

		// ���� ������.
	}while(!feof(Basefile));


	return FS_OK;
}
// ��ũ
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Read
// zip ���� ������ path ������ ã�� �޸𸮿� Ǯ��
// �޸� �ּ� ����.
unsigned char* Basefw_FS::Read(const char *path)
{
	end = fs_list.end();

	// fs_list ���� �˻�
	for(iter=fs_list.begin() ; iter != end ; ++iter)
	{
		// ������ ���� �˻�
		if(stricmp(reinterpret_cast<const char*>(iter->file_name), path) == 0)
		{
			// �޸𸮿� Ǯ���� �ִٸ�
			if(iter->Memory_Head){
				return iter->Memory_Head;
			}else{
				// �޸𸮿� Ǯ�����
				if(SetMemory((*iter))){
					return iter->Memory_Head;
				}
				return NULL;
			}
		}
	}

	cend = fs_clist.end();

	// fs_list ���� �˻�
	for(citer=fs_clist.begin() ; citer != cend ; ++citer)
	{
		// ������ ���� �˻�
		if(stricmp(reinterpret_cast<const char*>(citer->file_name), path) == 0)
		{
			printf("i find %s \n",citer->file_name);
			// �޸𸮿� Ǯ���� �ִٸ�
			//if(citer->Memory_Head){
			//	return citer->Memory_Head;
			//}else{
			//	// �޸𸮿� Ǯ�����
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
// ���ҽ� �޸� ������ ����
int Basefw_FS::ReadSize(const char *path)
{
	end = fs_list.end();
	for(iter=fs_list.begin() ; iter != end ; ++iter)
	{
		//������ ���ҽ��� ã����
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
// �޸𸮿� Ǯ�� ����
bool Basefw_FS::SetMemory(ZIP_LOCALFILE_HEADER& p_ZIP)
{
	// �޸� �Ҵ�
	unsigned char* tempout=new unsigned char[p_ZIP.uncompressed_size];
	unsigned char* tempin=new unsigned char[p_ZIP.compressed_size];

	// ���� ó������ �б� ( ���� Ŀ�� �ʱ�ȭ )
	fseek(Basefile, 0, SEEK_SET);
	// �ҷ��� ���ҽ� ���� �б� ���� ( ���� Ŀ�� ���ҽ� �κк��� )
	fseek(Basefile, p_ZIP.file_location, SEEK_SET);
	// tempin �� ���� �Ȱ� ����
	fread(tempin, p_ZIP.compressed_size, 1, Basefile);

	// z_stream �ʱ�ȭ
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

		// �޸𸮿� ��������

		err = inflate(&z,Z_NO_FLUSH); 
		// ���� �б� �Ϸ��
		if(err == Z_STREAM_END)
		{
			delete tempin;
			p_ZIP.Memory_Head = tempout;
			return true;
		}
		// z_stream ����.
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
