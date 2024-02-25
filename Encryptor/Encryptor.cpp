#include <Windows.h>
#include <stdio.h>

typedef struct _ADDRESS_INFO
{
	DWORD moduleBase;			//base address of executable
	DWORD moduleCodeOffset;		//offset of .code section in memory
	DWORD fileCodeOffset;		//offset of .code section in file
	DWORD fileCodeSize;			//# of bytes used by .code section in file
}ADDRESS_INFO, * PADDRESS_INFO;

void TraverseSectionHeaders(PIMAGE_SECTION_HEADER section, DWORD nSections, PADDRESS_INFO addrInfo)
{
	DWORD i;
	printf("[Dump sections]:------------------------------\n\n");
	for (i = 0; i < nSections; i++)
	{
		if (strcmp((const char*)(*section).Name, ".code") == 0)
		{
			(*addrInfo).fileCodeOffset = (*section).PointerToRawData;
			(*addrInfo).fileCodeSize = (*section).SizeOfRawData;
		}
		section = section + 1;
	}
	return;
}

void GetCodeLoc(DWORD* baseAddress, PADDRESS_INFO addrInfo)
{
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS peHeader;
	IMAGE_OPTIONAL_HEADER32 optionalHeader;

	dosHeader = (PIMAGE_DOS_HEADER)baseAddress;
	peHeader = (PIMAGE_NT_HEADERS)((DWORD)baseAddress + (*dosHeader).e_lfanew);
	optionalHeader = (*peHeader).OptionalHeader;

	(*addrInfo).moduleBase = optionalHeader.ImageBase;
	(*addrInfo).moduleCodeOffset = optionalHeader.BaseOfCode;

	printf("# sections=%d\n", (*peHeader).FileHeader.NumberOfSections);
	TraverseSectionHeaders(IMAGE_FIRST_SECTION(peHeader), (*peHeader).FileHeader.NumberOfSections, addrInfo);
	return;
}

int getHMODULE(const char* fileName, DWORD** hFileMapping)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open input file.");
		return 0;
	}
	DWORD fileSize = GetFileSize(hFile, 0);
	*hFileMapping = (DWORD*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, fileSize);
	DWORD tmp;
	ReadFile(hFile, *hFileMapping, fileSize, &tmp, 0);
	CloseHandle(hFile);
	return 1;
}

void cipherBytes(DWORD* hInMem, PADDRESS_INFO addrInfo)
{
	BYTE* codeStart = (BYTE*)hInMem + (*addrInfo).fileCodeOffset;
	int curPos = 0;
	for(int i = 0; i < (*addrInfo).fileCodeSize; i++)
	{
		*codeStart = *codeStart ^ 0x0f;
		codeStart++;
	}
}

int main()
{
	DWORD retVal;
	DWORD* hFileMapping = 0;
	ADDRESS_INFO addrInfo;

	char fileName[] = "C:\\Users\\atsee\\source\\repos\\CodeEncryption\\Debug\\CodeEncryption.exe";
	retVal = getHMODULE(fileName, &hFileMapping);
	if (retVal == FALSE) { return -1; }

	GetCodeLoc(hFileMapping, &addrInfo);
	cipherBytes(hFileMapping, &addrInfo);

	HANDLE hNewFile = CreateFile("xored", GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD tmp;
	WriteFile(hNewFile, hFileMapping, fileSize &tmp, 0);
	return 0;
}