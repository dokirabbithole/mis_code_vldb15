#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <string.h>

using namespace std;

class StopWatch {
private:
	clock_t stime;
	
public:
	void start(){
		stime = clock();
	}
	
	void say(const char* msg){
		clock_t t=clock();
		cout<<msg<<"\t used time: "<<(double)(t - stime)/CLOCKS_PER_SEC<<"s"<<endl;
		stime=clock();
	}
};

#endif