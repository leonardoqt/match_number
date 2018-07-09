#include <iostream>
#include "def.h"

using namespace std;

int main()
{
	int dat1[10] = {1,2,3,4,5,6,7,8,9,10};
	bulk_data bins1;
	bins1.set_num_bin(4);
	bins1.import_data(dat1,10);
	bins1.sum_up_bin();
	bins1.print_bin(0);
	bins1.print_bin(1);
	bins1.print_bin(2);
	bins1.print_bin(3);
	bins1.print_sum_bin();	
	return 0;
}
