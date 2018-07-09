#include <iostream>
#include "def.h"

using namespace std;

unit :: unit()
{
	x = 0;
	pre = nullptr;
	next = nullptr;
}

//=============================================

bulk_data :: bulk_data()
{
	num_bin = 0;
	bin = nullptr;
	len = nullptr;
	sum_bin = nullptr;
}

void bulk_data :: clean_self()
{
	unit * p1;
	for(int t1=0; t1<num_bin; t1++)
	{
		for(p1=&bin[t1]; p1->next != nullptr; p1 = p1->next) ;
		while(p1->pre != nullptr)
		{
			p1 = p1->pre;
			delete p1->next;
			p1->next = nullptr;
		}
	}
	delete[] len;
	delete[] sum_bin;
	num_bin = 0;
}

bulk_data& bulk_data :: operator = (const bulk_data& B)
{
	unit * p1, * p2;
	this->clean_self();
	num_bin = B.num_bin;
	bin = new unit[num_bin];
	len = new int[num_bin];
	sum_bin = new int[num_bin];
	for(int t1=0; t1<num_bin; t1++)
	{
		for(p1=&bin[t1], p2=&(B.bin[t1]); p2->next != nullptr; p1=p1->next, p2=p2->next)
		{
			p1->next = new unit;
			p1->next->x = p2->next->x;
			p1->next->pre = p1;
		}
		len[t1] = B.len[t1];
		sum_bin[t1] = B.sum_bin[t1];
	}
}

void bulk_data :: set_num_bin(int num)
{
	num_bin = num;
	bin = new unit[num_bin];
	len = new int[num_bin];
	sum_bin = new int[num_bin];
}

void bulk_data :: add_data(int t1, int data)
{
	unit *p1;
	for(p1=&bin[t1]; p1->next != nullptr; p1=p1->next) ;
	p1->next = new unit;
	p1->next->x = data;
	p1->next->pre = p1;
}

void bulk_data :: get_len()
{
	unit *p1;
	for (int t1=0; t1<num_bin; t1++)
		for(len[t1]=0,p1=&bin[t1];p1->next != nullptr; len[t1]++,p1 = p1->next) ;
}

void bulk_data :: mv_data(int bin1, int bin2, int pos1)
{
	int t1;
	unit * b1_p1, *b1_p2, *b2_p1;
	// find pos1 th data in bin1
	for (t1=0, b1_p1=&bin[bin1]; t1<pos1; t1++, b1_p1 = b1_p1->next) ;
	b1_p2 = b1_p1->pre;
	b1_p2->next = b1_p1->next;
	b1_p1->next = nullptr;
	for (b2_p1=&bin[bin2]; b2_p1->next != nullptr; b2_p1 = b2_p1->next) ;
	b2_p1->next = b1_p1;
	b1_p1->pre = b2_p1;
}

void bulk_data :: sum_up_bin()
{
	unit *p1;
	for(int t1=0; t1<num_bin; t1++)
		for(sum_bin[t1]=0, p1=&bin[t1]; p1->next != nullptr; p1 = p1->next)
			sum_bin[t1] += p1->next->x;
}

void bulk_data :: import_data(int * dat1, int num)
{
	int t1, t2;
	for(t1=0; t1<num; t1++)
	{
		t2 = t1%num_bin;
		this->add_data(t2,dat1[t1]);
	}
}

//------------debug-------------
void bulk_data :: print_bin(int t1)
{
	unit * p1;
	for(p1=&bin[t1]; p1->next != nullptr; p1=p1->next)
		cout<<p1->next->x<<'\t';
	cout<<endl;
}

void bulk_data :: print_len()
{
	for(int t1=0; t1<num_bin; t1++)
		cout<<len[t1]<<'\t';
	cout<<endl;
}

void bulk_data :: print_bin_inv(int t1)
{
	unit *p1;
	for(p1=&bin[t1]; p1->next != nullptr; p1 = p1->next) ;
	while(p1->pre != nullptr)
	{
		p1 = p1->pre;
		cout<<p1->next->x<<'\t';
	}
	cout<<endl;
}

void bulk_data :: print_sum_bin()
{
	for(int t1=0; t1 < num_bin; t1++)
		cout<<sum_bin[t1]<<endl;
}
