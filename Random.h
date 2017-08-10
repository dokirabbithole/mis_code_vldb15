#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "Graph.h"
#include "MIS.h"
#include <vector>
#include <algorithm>

/* sort by a.first in ascending order */
bool comp(const pair<int,int> &a, const pair<int,int> &b)
{
	return a.first < b.first;
}

class Random
{
public:
	/* sequentially scan the file in which adjacency lists are sorted randomly */
	MIS random(const char* ranfile)
	{
		ifstream infile(ranfile, ios::binary);
		int n;
		infile.read((char*)(&n),sizeof(n));

		/* store the independent set */
		MIS mis(n);
		for(int i=0;i<n;i++)
		{
			int id;
			int degree;
			infile.read((char*)(&id),sizeof(id));
			infile.read((char*)(&degree),sizeof(degree));

			if(mis.status(id)==true)
			{
				int w;
				for(int j=0;j<degree;j++)
				{
					infile.read((char*)(&w),sizeof(w));
					if(mis.status(w)==true)
						mis.set(w,false);
				}
			}
			else
			{
				int dummy;
				for(int j=0;j<degree;j++)
					infile.read((char*)(&dummy),sizeof(dummy));
			}
		}
		infile.close();
		return mis;
	}

	/* in-memory algorithm that sorts graph file randomly */
	void memRandomSort(const char* binaryfile, const char* ranfile)
	{
		Graph g(binaryfile);
		vector<pair<int, int> > randomSort;
		for(int i=0;i<g.n;i++)
		{
			randomSort.push_back(pair<int,int>(rand(),i));
		}
		sort(randomSort.begin(), randomSort.end(),comp);
		
		ofstream of(ranfile, ios::binary);
		int graphsize=randomSort.size();
		of.write((char*)(&graphsize),sizeof(graphsize));
		for(int i=0;i<graphsize;i++)
		{
			//cout<<randomSort[i].first<<"\t"<<randomSort[i].second;
			int v=randomSort[i].second;
			int d=g.deg[v];
			of.write((char*)(&v),sizeof(v));
			of.write((char*)(&d),sizeof(d));
			for(int j=0;j<d;j++)
			{
				int w=g.adj[v][j];
				of.write((char*)(&w),sizeof(w));
			}
		}
		of.close();
	}

	/* external memory algorithm that sorts graph file randomly */
};

#endif