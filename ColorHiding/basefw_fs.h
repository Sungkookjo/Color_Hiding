#pragma once



#pragma pack(push, 1)

struct ZIP_LOCALFILE_HEADER
{
	//file header
	int  local_file_header_signature; //It must 0x04034b50
	short version_needed_to_extract;
	short general_purpose_bit_flag;
	short compression_method;
	short last_mod_file_time;
	short last_mod_file_date;
	int  crc_32;
	int  compressed_size;
	int  uncompressed_size;
	short file_name_length;
	short extra_field_length;

	//file content
	unsigned char *file_name;
	unsigned char *Memory_Head;
	int    extra_location;
	int    file_location;

	//data descriptor
	int  crc_32_end;
	int  compressed_size_end;
	int  uncompressed_size_endl;
};

const int SIGNATURE_SIZE = sizeof(int);
const int FILE_HEADER_SIZE = sizeof(int)*4+sizeof(short)*7;
const int DATA_DESCRT_SIZE = sizeof(int)*3;

struct ZIP_CENTRALFILE_HEADER
{
	int  central_file_header_signature; //must be 0x02014b50
	short version_made_by; 
	short version_needed_to_extract;
	short general_purpose_bit_flag;
	short compression_method;
	short last_mod_file_time;
	short last_mod_file_date;
	int  crc_32;
	int  compressed_size;
	int  uncompressed_size;
	short filename_length;
	short extra_field_length;
	short file_comment_length;
	short disk_number_start;
	short internal_file_attributes;
	int  external_file_attributes;
	int  relative_offset_of_local_header;

	unsigned char *file_name;
	int    extra_location;
	int    file_comment_location;
};

const int CENTRAL_HEADER_SIZE = sizeof(int)*6+sizeof(short)*11;

struct ZIP_ENDCENTRAL_HEADER
{
	int  end_of_central_dir_signature; //must be 0x06054b50
	short number_of_this_disk;
	short of_the_disk_with_the_start_of_the_central_dir;
	short total_of_entries_in_the_central_dir_on_this_disk;
	short total_of_entries_in_the_central_dir;
	int  size_of_central_directory;
	int  offset_of_start_of_central_dir_with_respect_to_the_starting_disk_number;
	short zipfile_comment_length;

	int  zipfile_comment_location;
};

const int ENDCENTRAL_HEADER_SIZE = sizeof(int)*3+sizeof(short)*5;

#pragma pack(pop)

const int FS_WRONGPATH = -1;
const int FS_OK   = 1;

class Basefw_FS
{
private:
	z_stream z;
	// 압축 파일
	FILE *Basefile;

	// 압축 파일 리스트.
	std::vector<ZIP_LOCALFILE_HEADER> fs_list;
	std::vector<ZIP_LOCALFILE_HEADER>::iterator iter;
	std::vector<ZIP_LOCALFILE_HEADER>::iterator end;

	std::vector<ZIP_CENTRALFILE_HEADER> fs_clist;
	std::vector<ZIP_CENTRALFILE_HEADER>::iterator citer;
	std::vector<ZIP_CENTRALFILE_HEADER>::iterator cend;

	std::vector<ZIP_ENDCENTRAL_HEADER> fs_elist;
	std::vector<ZIP_ENDCENTRAL_HEADER>::iterator eiter;
	std::vector<ZIP_ENDCENTRAL_HEADER>::iterator eend;

	bool SetMemory(ZIP_LOCALFILE_HEADER& p_ZIP);
public:
	Basefw_FS();
	Basefw_FS(const char* path);
	~Basefw_FS();

	int Link(const char* path); // ZIP 파일 열기
	unsigned char* Read(const char* path); // 리소스를 찾아 압축 해제
	int ReadSize(const char* path); // 리소스 사이즈 리턴
};
