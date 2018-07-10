#ifndef MYCLASS
#define MYCLASS

class unit;
class bulk_data;

class unit
{
public:
	int x;
	unit * pre;
	unit * next;
	unit();
};

class bulk_data
{
private:
	int num_bin;	// number of bins
	unit * bin;		// data header
	int * len;		// length of each bin
	int * sum_bin;	// sum of data in each bin
	double * p_ad;	// probablity of choosing bin to add
	double * p_rm;	// probablity of choosing bin to remove
public:
	bulk_data();
	// basic operation
	void clean_self();
	bulk_data& operator = (const bulk_data &);
	void set_num_bin(int num);
	// basic ad / rm / mv data
	void add_data(int t1, int data);				// add a single number to a bin
	void get_len();
	void mv_data(int bin1, int bin2, int pos1);		// move pos1 th data from bin1 to bin2 bins start from 0, pos1 starts from 1
	void sum_up_bin();
	// massive data related
	void import_data(int * dat1, int num);
	void find_p_rm(bulk_data & T, double adjust);
	// debug
	void print_bin(int t1);
	void print_len();
	void print_bin_inv(int t1);
	void print_sum_bin();
};


#endif
