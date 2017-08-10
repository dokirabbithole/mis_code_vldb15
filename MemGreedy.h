#ifndef _MEMGREEDY_H
#define _MEMGREEDY_H

#include <vector>
#include <algorithm>
#include <math.h>
#include "Graph.h"
#include "StopWatch.h"
#include "fibnode.h"
#include "fiboheap.h"
#include "MIS.h"

class MemGreedy
{
public:
	MIS greedy(const char* file)
	{
		StopWatch timer;
		timer.start();

		Graph g(file);
		MIS mis(g.n);
		bool *extract=new bool[g.n]; 
		FibNode<int,int> **node=new FibNode<int,int>*[g.n];
		for(int i=0;i<g.n;i++)
		{
			FibNode<int,int> *temp=new FibNode<int,int>(g.deg[i],i,nullptr);
			node[i]=temp;
		}
		
		//vector<pair<int,int> > breaktie;
		//for(int i=0;i<g.n;i++)
			//breaktie.push_back(pair<int,int>(rand(),i));
		//sort(breaktie.begin(),breaktie.end());
				
		FibHeap<int,int> fibheap;
		for(int i=0;i<g.n;i++)
		{
			fibheap.insert(node[i]);
			extract[i]=false;
		}
		FibNode<int,int> *fibnode;
		int upperbound=0;
		for(int i=0;i<g.n;i++)
		{
			fibnode=fibheap.extract_min();
			int v=fibnode->getValue();
			extract[v]=true;
			if(mis.status(v)==true)
			{
				int count=0;
				for(int j=0;j<g.deg[v];j++)
				{
					int w=g.adj[v][j];
					if(mis.status(w)==true)
					{
						mis.set(w,false);
						count++;
						for(int k=0;k<g.deg[w];k++)
						{
							int x=g.adj[w][k];
							if(!extract[x])
								fibheap.decrease_key(node[x],node[x]->key - 1);
						}
					}
				}
				upperbound+=max(count,1);
			}
			// else do nothing
		}
		timer.say("DU algorithm");
		
		cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(file)<<endl;
		cout<<"Independent set size\t"<<mis.size()<<endl;

		cout<<"upper bound\t"<<upperbound<<endl;
	}
};

#endif