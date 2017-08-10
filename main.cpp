#include <sstream>
#include <stdlib.h>
#include "Random.h"
#include "Greedy.h"
#include "MemGreedy.h"
#include "StopWatch.h"
#include "OneKSwap.h"
#include "TwoKSwap.h"
//#include "STXXL_Random.h"

#include "fiboheap.h"
#include "fibnode.h"

/* random + 1-k-swap */
void random_1kswap(const char* file)
{
	Random random_mis;
	StopWatch timer;
	timer.start();

	stringstream ss;
	ss << file << "-topology";
	//random_mis.memRandomSort(file, ss.str().c_str());
	cout<<ss.str().c_str()<<endl;
	//timer.say("sort file randomly in memory");

	MIS mis=random_mis.random(ss.str().c_str());
	timer.say("semi-external random algorithm");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;

	timer.start();
	OneKSwap onekswap;
	onekswap.swap(ss.str().c_str(), mis, 3);
	timer.say("random + 1-k-swap");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;
}

/* greedy + 1-k-swap */
void greedy_1kswap(const char* file)
{
	Greedy greedy_mis;
	StopWatch timer;
	timer.start();

	stringstream ss;
	ss << file << "-greedy";
	//greedy_mis.memSortByDegree(file, ss.str().c_str());
	cout<<ss.str().c_str()<<endl;
	//timer.say("sort file by degree in memory");

	MIS mis=greedy_mis.greedy(ss.str().c_str());
	timer.say("semi-external greedy algorithm");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;

	timer.start();
	OneKSwap onekswap;
	onekswap.swap(ss.str().c_str(), mis, 3);
	timer.say("greedy + 1-k-swap");
}

/* random + 1-k-swap + 2-k-swap */
void random_2kswap(const char* file)
{
	Random random_mis;
	StopWatch timer;
	timer.start();

	stringstream ss;
	ss << file << "-topology";
	//random_mis.memRandomSort(file, ss.str().c_str());
	cout<<ss.str().c_str()<<endl;
	//timer.say("sort file randomly in memory");

	MIS mis=random_mis.random(ss.str().c_str());
	timer.say("semi-external random algorithm");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;

	timer.start();
	OneKSwap onekswap;
	onekswap.swap(ss.str().c_str(), mis, 2);
	TwoKSwap twokswap;
	twokswap.swap(ss.str().c_str(), mis, 2);
	timer.say("random + 1-k-swap + 2-k-swap");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;
}

/* greedy + 1-k-swap + 2-k-swap */
void greedy_2kswap(const char* file)
{
	Greedy greedy_mis;
	StopWatch timer;
	timer.start();

	stringstream ss;
	ss << file << "-greedy";
	//greedy_mis.memSortByDegree(file, ss.str().c_str());
	cout<<ss.str().c_str()<<endl;
	//timer.say("sort file by degree in memory");

	MIS mis=greedy_mis.greedy(ss.str().c_str());
	timer.say("semi-external greedy algorithm");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;

	timer.start();
	OneKSwap onekswap;
	onekswap.swap(ss.str().c_str(), mis, 2);
	TwoKSwap twokswap;
	twokswap.swap(ss.str().c_str(), mis, 2);
	timer.say("greedy + 1-k-swap + 2-k-swap");
	cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(ss.str().c_str())<<endl;
	cout<<"Independent set size\t"<<mis.size()<<endl;
}

/* dynamic update */
void du(const char* file)
{
	MemGreedy du;
	du.greedy(file);
}
int main(int argc, char** args)
{
	// random_1kswap(args[1]);
	// greedy_1kswap(args[1]);
	// random_2kswap(args[1]);
	// greedy_2kswap(args[1]);
	// du(args[1]);
	return 0;
}