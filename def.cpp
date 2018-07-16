#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
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
	p_ad = nullptr;
	p_rm = nullptr;
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
	delete[] p_ad;
	delete[] p_rm;
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
		p_ad[t1] = B.p_ad[t1];
		p_rm[t1] = B.p_rm[t1];
	}
}

void bulk_data :: set_num_bin(int num)
{
	num_bin = num;
	bin = new unit[num_bin];
	len = new int[num_bin];
	sum_bin = new int[num_bin];
	p_ad = new double[num_bin];
	p_rm = new double[num_bin];
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
//	this->print_bin(bin1);
//	cout<<"should choose "<<pos1<<" th from list"<<endl;
	// find pos1 th data in bin1
	for (t1=0, b1_p1=&bin[bin1]; t1<pos1; t1++, b1_p1 = b1_p1->next) ;
	b1_p2 = b1_p1->pre;
	b1_p2->next = b1_p1->next;
	if (b1_p1->next != nullptr)
		b1_p1->next->pre = b1_p2;
	b1_p1->next = nullptr;
	for (b2_p1=&bin[bin2]; b2_p1->next != nullptr; b2_p1 = b2_p1->next) ;
	b2_p1->next = b1_p1;
	b1_p1->pre = b2_p1;
	sum_bin[bin1] -= b1_p1->x;
	sum_bin[bin2] += b1_p1->x;
	len[bin1]--;
	len[bin2]++;
}

void bulk_data :: undo(int bin1, int bin2)
{
	this->mv_data(bin2, bin1, len[bin2]);
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

void bulk_data :: import_data_all(int * dat1, int num_bin, int num_data)
{
	this->set_num_bin(num_bin);
	this->import_data(dat1,num_data);
	this->sum_up_bin();
	this->get_len();
}

int bulk_data :: tot_err(bulk_data & T)
{
	int err=0;
	for (int t1=0; t1<num_bin; t1++)
		err += abs(sum_bin[t1] - T.sum_bin[t1]);
	return err;
}

void bulk_data :: find_p_rm(bulk_data & T, double adjust)
{
	for (int t1=0; t1<num_bin; t1++)
	{
		p_rm[t1] = sum_bin[t1] - T.sum_bin[t1] + adjust;
		if (p_rm[t1] < 0)
			p_rm[t1] = 0;
		// need to consider if using linear or high order expression
		// p_rm[t1] = p_rm[t1] * p_rm[t1];
	}
	for (int t1=1; t1<num_bin; t1++)
		p_rm[t1] += p_rm[t1-1];
	for (int t1=0; t1<num_bin; t1++)
		p_rm[t1] /= p_rm[num_bin-1];
}

void bulk_data :: find_p_ad(bulk_data & T, double adjust, int bin1, int pos1)
{
	// remember to make p_ad[bin1] =0
	int to_ad = 0;
	int t1;
	unit *p1;
	for(p1=&bin[bin1], t1=0; t1<pos1; p1=p1->next,t1++) ;
	to_ad = p1->x;
	for(t1=0; t1<num_bin; t1++)
		p_ad[t1] = 1/(abs(sum_bin[t1] + to_ad - T.sum_bin[t1]) + adjust);
	p_ad[bin1] = 0;
	for(t1=1; t1<num_bin; t1++)
		p_ad[t1] += p_ad[t1-1];
	for(t1 = 0; t1<num_bin; t1++)
		p_ad[t1] /= p_ad[num_bin-1];
}

int bulk_data :: choose_bin_rm()
{
	int t1;
	double p;
	p = rand()/(double) RAND_MAX;
	for(t1=0; t1<num_bin; t1++)
		if(p_rm[t1] > p)
			break;
	if (len[t1] ==0)
		t1 = this->choose_bin_rm();
	return t1;
}

int bulk_data :: choose_bin_ad()
{
	int t1;
	double p;
	p = rand()/(double) RAND_MAX;
	for(t1=0; t1<num_bin; t1++)
		if(p_ad[t1] > p)
			break;
	return t1;
}


int bulk_data :: choose_pos_rm(bulk_data & T, double adjust, int bin1)
{
	int t1;
	unit * p1;
	double *p, pp;
	p = new double[len[bin1]];
	for (t1=0,p1=&bin[bin1]; p1->next != nullptr; p1=p1->next,t1++)
		p[t1] = 1/(abs(T.sum_bin[bin1] + p1->next->x - sum_bin[bin1]) + adjust);
	for (t1=1; t1<len[bin1]; t1++)
		p[t1] += p[t1-1];
	for (t1=0; t1<len[bin1]; t1++)
		p[t1] /= p[len[bin1]-1];
	pp = rand()/(double)RAND_MAX;
	for (t1=0; t1<len[bin1]; t1++)
		if (p[t1] > pp)
			break;
	delete[] p;
	return t1+1;
}
//------------debug-------------
void bulk_data :: print_bin(int t1)
{
	unit * p1;
	cout<<"bin "<<t1<<" :"<<endl;
	for(p1=&bin[t1]; p1->next != nullptr; p1=p1->next)
		cout<<p1->next->x<<'\t';
	cout<<endl;
}

void bulk_data :: print_bin(int t1, ofstream & out)
{
	unit * p1;
	out<<"bin "<<t1<<" :"<<endl;
	for(p1=&bin[t1]; p1->next != nullptr; p1=p1->next)
		out<<p1->next->x<<'\t';
	out<<endl;
}

void bulk_data :: print_bin()
{
	for (int t1=0; t1<num_bin; t1++)
		this->print_bin(t1);
}

void bulk_data :: print_bin(ofstream & out)
{
	for (int t1=0; t1<num_bin; t1++)
		this->print_bin(t1, out);
}

void bulk_data :: print_len()
{
	cout<<"length of each bin :"<<endl;
	for(int t1=0; t1<num_bin; t1++)
	{
		cout<<"bin "<<t1<<" :"<<endl;
		cout<<len[t1]<<endl;
	}
	cout<<endl;
}

void bulk_data :: print_len(ofstream & out)
{
	out<<"length of each bin :"<<endl;
	for(int t1=0; t1<num_bin; t1++)
	{
		out<<"bin "<<t1<<" :"<<endl;
		out<<len[t1]<<endl;
	}
	out<<endl;
}

void bulk_data :: print_len(int t1, ofstream & out)
{
	out<<len[t1]<<endl;
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
	cout<<"sum of each bin :"<<endl;
	for(int t1=0; t1 < num_bin; t1++)
	{
		cout<<"bin "<<t1<<" :"<<endl;
		cout<<sum_bin[t1]<<endl;
	}
	cout<<endl;
}

void bulk_data :: print_sum_bin(ofstream & out)
{
	out<<"sum of each bin :"<<endl;
	for(int t1=0; t1 < num_bin; t1++)
	{
		out<<"bin "<<t1<<" :"<<endl;
		out<<sum_bin[t1]<<endl;
	}
	out<<endl;
}

void bulk_data :: print_sum_bin(int t1, ofstream & out)
{
	out<<sum_bin[t1]<<endl;
}

void bulk_data :: print_p_rm()
{
	cout<<"print_p_rm:"<<endl;
	for(int t1=0; t1<num_bin; t1++)
		cout<<p_rm[t1]<<'\t';
	cout<<endl;
}

void bulk_data :: print_p_ad()
{
	cout<<"print_p_ad:"<<endl;
	for(int t1=0; t1<num_bin; t1++)
		cout<<p_ad[t1]<<'\t';
	cout<<endl;
}

//=============================================

void mc :: change_T(int t1)
{
	double step_round = t1%step_3;
	if (step_round <= step_1)
		T = T_max;
	else if (step_round >= step_2)
		T = T_min;
	else
		T = T_max - (T_max - T_min) * (step_round - step_1) / (step_2 - step_1);
}

int mc :: if_accept(int err_new)
{
	int flag;
	double p;
	if (err_new < err_old)
		flag = 1;
	else
	{
		p = rand()/(double)RAND_MAX;
		if ( exp((err_old-err_new)/T/err_new > p) )
			flag = 1;
		else
			flag = 0;
	}
	if (flag == 1)
		err_old = err_new;
	return flag;
}
