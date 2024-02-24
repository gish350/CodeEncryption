#include <Windows.h>
#include <stdio.h>

typedef struct _ADDRESS_INFO
{
	DWORD moduleBase;			//base address of executable
	DWORD moduleCodeOffset;		//offset of .code section in memory
	DWORD fileCodeOffset;		//offset of .code section in file
	DWORD fileCodeSize;			//# of bytes used by .code section in file
}ADDRESS_INFO, * PADDRESS_INFO;

void GetCodeLoc(LPVOID baseAddress, PADDRESS_INFO addrInfo)
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
	TraverseSectionHeaders
	(
		IMAGE_FIRST_SECTION(peHeader),
		(*peHeader).FileHeader.NumberOfSections,
		addrInfo
	);
	return;
}

void TraverseSectionHeaders
(
	PIMAGE_SECTION_HEADER section,
	DWORD nSections,
	PADDRESS_INFO addrInfo
)
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

int getHMODULE(const char* fileName, DWORD* hFileMapping)
{	
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open input file.");
		return 0;
	}
	DWORD fileSize = GetFileSize(hFile, 0);
	BYTE* hFileMapping = (BYTE*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, fileSize);
	DWORD tmp;
	ReadFile(hFile, hFileMapping, fileSize, &tmp, 0);
	CloseHandle(hFile);
	return 1;
}

void cipherBytes(const char* fileName)
{

}

int main()
{
	DWORD retVal;
	DWORD hFileMapping;
	ADDRESS_INFO addrInfo;
	
	char fileName[] = "filename";
	retVal = getHMODULE(fileName, &hFileMapping);
	if (retVal == FALSE) { return; }

	GetCodeLoc(&hFileMapping, &addrInfo);
	cipherBytes(fileName, &addrInfo);
	return 0;
}