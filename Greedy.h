#ifndef _GREEDY_H_
#define _GREEDY_H_

#include <vector>
#include <algorithm>

class Greedy
{
public:
	/* sequentially scan the file in which adjacency lists are sorted by degree */
	MIS greedy(const char* sortfile)
	{
		ifstream infile(sortfile, ios::binary);
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

	/* in-memory algorithm that sorts graph file by degree */
	void memSortByDegree(const char* binaryfile, const char* sortfile)
	{
		Graph g(binaryfile);
		vector<pair<int, int> > breaktie;
		for(int i=0;i<g.n;i++)
			breaktie.push_back(pair<int,int>(rand(),i));
		sort(breaktie.begin(), breaktie.end(), comp);

		vector<pair<int, int> > degSort;
		for(int i=0;i<breaktie.size();i++)
		{
			int v=breaktie[i].second;
			degSort.push_back(pair<int,int>(g.deg[v],v));
		}
		sort(degSort.begin(), degSort.end(),comp);
		
		ofstream of(sortfile, ios::binary);
		int graphsize=degSort.size();
		of.write((char*)(&graphsize),sizeof(graphsize));
		for(int i=0;i<graphsize;i++)
		{
			int v=degSort[i].second;
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
	
	/* external memory algorithm that sorts graph file by degree */
};

#endif
