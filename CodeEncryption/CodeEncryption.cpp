// .code SECTION---------------------------------------------------
/*
*	Keep unreferenced data, linker option /OPT:NOREF
*/
#pragma section(".code", execute, read, write)
#pragma comment(linker, "/MERGE:.text=.code")
#pragma comment(linker, "/MERGE:.data=.code")
#pragma comment(linker, "/SECTION:.code, ERW")

unsigned char var[] = { 0xBE, 0xBA, 0xFE, 0xCA };

// everything from here untill the next code_seg directive belogns to .code section
#pragma code_seg(".code")

void main()
{
	// pragma code here
	return;
} /*end main()-----------------------------------------------------*/