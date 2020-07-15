#include <iostream.h>
#include "stdio.h"
#include "System.h"
#include "lock.h"

extern int userMain(int argc, char* argv[]);



int main(int argc, char* argv[])
{

	printf("===========================================\n");
	printf("======= MILADIN DEVEDZIC 2018/0484 ========\n");
	printf("=============== MAIN STARTS ===============\n");
	System::initializeKernel();
	int res = userMain(argc, argv);
	System::restoreKernel();
	printf("================= MAIN ENDS ================\n");
	printf("============================================\n");
	return res;

}

	

