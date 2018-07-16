#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "def.h"

using namespace std;

int main()
{
	srand(time(0));
	// define data
	int * dat1, *dat2;
	bulk_data target, work;
	mc run1;
	ifstream in1, in2;
	int num1, num2;
	int bin1,bin2,pos1;
	int err_old, err_new;
	// define temp
	string tmp1;
	int t1,t2;

	// open data file
	in1.open("dat1");
	in2.open("dat2");

	// find data in file
	for(num1=0,getline(in1,tmp1); !in1.eof(); getline(in1,tmp1),num1++) ;
	in1.clear();
	in1.seekg(0);
	for(num2=0,getline(in2,tmp1); !in2.eof(); getline(in2,tmp1),num2++) ;
	in2.clear();
	in2.seekg(0);
	
	// define data array and read
	dat1 = new int[num1];
	dat2 = new int[num2];
	for(t1=0; t1<num1; t1++)
		in1>>dat1[t1];
	for(t1=0; t1<num2; t1++)
		in2>>dat2[t1];
	in1.close();
	in2.close();
	
	// consider sort data and clean the same
	// import data into bins
	target.import_data_all(dat1,num1,num1);
	work.import_data_all(dat2,num1,num2);

	// defind mc
	run1.T_max = 10;
	run1.T_min = 0.01;
	run1.step_1 = 2000;
	run1.step_2 = 8000;
	run1.step_3 = 20000;
	run1.err_old=work.tot_err(target);
	for(t1=0; t1<100000; t1++)
	{
		run1.change_T(t1);
		work.find_p_rm(target,run1.T);
		bin1 = work.choose_bin_rm();
		pos1 = work.choose_pos_rm(target,run1.T,bin1);
		work.find_p_ad(target,run1.T,bin1,pos1);
		bin2 = work.choose_bin_ad();
		work.mv_data(bin1,bin2,pos1);
		err_new = work.tot_err(target);
		if( ! run1.if_accept(err_new) )
			work.undo(bin1,bin2);
		cout<<run1.err_old<<endl;
	}
	return 0;
}
