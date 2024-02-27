// .code SECTION---------------------------------------------------
/*
*	Keep unreferenced data, linker option /OPT:NOREF
*/

#pragma section(".code", execute, read, write)
#pragma comment(linker, "/MERGE:.text=.code")
#pragma comment(linker, "/MERGE:.data=.code")
#pragma comment(linker, "/SECTION:.code,ERW")

unsigned char var[] = { 0xBE, 0xBA, 0xFE, 0xCA };

// everything from here untill the next code_seg directive belogns to .code section
#pragma code_seg(".code")

void main()
{
	// program code goes here
	int var = 0xBABE;
	return;
} /*end main()-----------------------------------------------------*/

//.stub SECTION-----------------------------------------------------
#pragma section(".stub", execute, read)
#pragma comment(linker, "/entry:\StubEntry\"")
#pragma code_seg(".stub");
//#include <stdio.h>

/*
 can determine these values via dumpbin.exe then set at compile time
 can also have cryptor parse PE and set these during encryption
*/

#define CODE_BASE_ADDRESS	0x00413000
#define CODE_SIZE			0x00000200
#define KEY					0x0F

void decryprCodeSection()
{
	//we'll use a Mickey-Mouse encoding scheme to keep things simple
	unsigned char* ptr;
	long int i;
	long int nbytes;
	ptr = (unsigned char*)CODE_BASE_ADDRESS;
	nbytes = CODE_SIZE;
	for (i = 0; i < nbytes; i++)
	{
		ptr[i] = ptr[i] ^ KEY;
	}
	return;
}/*end decryptSection()----------------------------------------------*/

void StubEntry()
{
	decryprCodeSection();
	//printf("Started In Stub()\n");
	main();
	return;
}/*end StubEntry()--------------------------------------------------*/