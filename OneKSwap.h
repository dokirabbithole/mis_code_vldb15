#ifndef _ONEKSWAP_H_
#define _ONEKSWAP_H_

#include "MIS.h"

class OneKSwap
{
public:
	/* For non-IS vertex:
	isn records the only adjacent IS neighbor for some non-IS vertices, i.e., 1-k-swap candidates;
	for non-IS vertex with more than one adjacent IS neighbors, isn is set NIL, indicating that it does not participate in the swap.
	For IS vertex:
	if it's not a newly swapped one in this iteration, isn records the number of adjacent swap candidates and is updated dynamically;
	otherwise, isn is set NIL. 
	Memory cost (theoretically): |V|log_2(|V|+1) bits. */
	int* isn;
	/* Indicating that the vertex does not participate in the swap. */
	static const int NIL=-1;
	/* Indicating if the iteration can stop or not. If this iteration has any 1-k-swap or 0-1-swap, another iteration is needed to 
	guarantee all possible swaps are performed. In practice the algorithm can stop much earlier, i.e., after 2 or 3 iterations. */
	bool canSwap;
	
	void swap(const char* file, MIS &mis, int T)
	{
		cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(file)<<endl;

		int n=mis.n;		
		isn=new int[n];
		for(int i=0;i<n;i++)
			isn[i]=NIL;

		int iteration=0;
		canSwap=true;
		while(canSwap && iteration < T)
		{
			_1kswap(file,mis);
			iteration++;
			cout<<iteration<<endl;
			cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(file)<<endl;
			cout<<"Independent set size\t"<<mis.size()<<endl;
		}
	}
private:
	/* One iteration of 1-k-swap. Each iteration needs three sequential scans of the file.
	1st scan: update isn info;
	2nd scan: 1-k-swap;
	3rd scan: 0-1-swap.
	(Notes: The algorithm adopts the streaming mode, i.e., adjacency lists are sequentially scanned 
	but not recorded in memory at all. In practice, IS vertex has relatively low degree. If
	the adjacency list of a vertex of this kind (e.g., degree is O(logn)) can be cached in
	memory when it is processed, fewer scans are needed.) */
	void _1kswap(const char* file, MIS &mis)
	{
		canSwap=false;
		int n=mis.n;
		for(int i=0;i<n;i++)
		{
			/* For a IS vertex, isn records the number of adjacent candidates. */
			if(mis.status(i)==true)
				isn[i]=0;
			/* For a non-IS vertex, if it's a candidate, i.e., it has only one adjacent IS vertex, 
			isn records that vertex; otherwise it's NIL, which is undefined. */
			else
				isn[i]=i;
		}
		/* 1st scan */
		ifstream infile(file, ios::binary);
		infile.read((char*)(&n),sizeof(n));
		for(int i=0;i<n;i++)
		{
			int v;
			int d;
			infile.read((char*)(&v),sizeof(v));
			infile.read((char*)(&d),sizeof(d));
			if(mis.status(v)==false) // update isn info of a non-IS vertex, see if it's a candidate
			{
				int w;
				for(int j=0;j<d;j++)
				{
					infile.read((char*)(&w),sizeof(w));
					if(mis.status(w)==true)
					{
						if(isn[v]==v) 
							isn[v]=w;
						else if(isn[v]!=v && isn[v]>=0 && isn[v]!=w)
							isn[v]=NIL;
					}
				}
			}
			else // v is an IS vertex, do nothing
			{
				int dummy;
				for(int j=0;j<d;j++)
					infile.read((char*)(&dummy),sizeof(dummy));
			}
		} // end for

		// update isn info of IS vertices
		for(int i=0;i<n;i++)
		{
			if(mis.status(i)==false && isn[i]>=0)
			{
				if(mis.status(isn[i])==true)
					isn[isn[i]]++;
				else // should never reach here for a simple graph
				{
					cout<<i<<"\t"<<mis.status(i)<<"\t"<<isn[i]<<endl;
					//system("PAUSE");
				}
			}
		}
		/* 2nd scan */
		infile.seekg(0, ios::beg);
		infile.read((char*)(&n),sizeof(n));
		for(int i=0;i<n;i++)
		{
			int v;
			int d;
			infile.read((char*)(&v),sizeof(v));
			infile.read((char*)(&d),sizeof(d));
			/* v is a 1-k-swap candidate */
			if(mis.status(v)==false && isn[v]>=0)
			{
				bool vertexCanSwap=true;
				int w;
				int count=0;
				for(int j=0;j<d;j++)
				{
					infile.read((char*)(&w),sizeof(w));
					/* If v has an adjacent IS vertex w but not isn[v], w must be newly swapped. */
					if(mis.status(w)==true && isn[v]!=w)
						vertexCanSwap=false;
					/* Deadlock: 
					both v and w are candidates, with different adjacent IS vertices;
					isn[v] is not swapped yet;
					isn[w] is 1-1-swapped, by isn[isn[w]];
					then v will not do swap to give chance to w for a 1-2-swap. */
					else if(mis.status(w)==false && isn[w]>=0 && mis.status(isn[v])==true && mis.status(isn[w])==false && isn[isn[w]]>=0)
						vertexCanSwap=false;

					if(w!=v && isn[w]==isn[v])
						count++;
				}
				// if isn[v] has only one candidate (i.e., v) remained, can not swap
				if(mis.status(isn[v])==true && isn[isn[v]]<=1)
					vertexCanSwap=false;
				// if v has edges to all other remaining swap candidates of isn[v], can not do 1-k-swap (k>=2) 
				if(mis.status(isn[v])==true && count==isn[isn[v]]-1)
					vertexCanSwap=false;

				if(vertexCanSwap)
				{
					int pv=isn[v];
					mis.set(v,true);
					// for a newly swapped IS vertex, isn=NIL; otherwise, isn>=0 (=# of adjacent candidates)
					isn[v]=NIL;

					// first do 1-1-swap
					if(mis.status(pv)==true)
					{
						mis.set(pv,false);
						isn[pv]=v;
					}
					// 1-2-swap occurs, set canSwap=true
					else if(isn[pv]>=0 && isn[pv]!=v)
					{
						isn[pv]=NIL;
						canSwap=true;
					}
					// else 1-k-swap (k>2)
				}
				else // candidate can not swap, update isn info of its adjacent IS vertex and itself
				{
					if(mis.status(isn[v])==true && isn[isn[v]]>0)
					{
						isn[isn[v]]--;
						isn[v]=NIL;
					}
				}
			}
			else
			{
				int dummy;
				for(int j=0;j<d;j++)
					infile.read((char*)(&dummy),sizeof(dummy));
			}
		} // end for

		/* cancellation of 1-1-swap
		At this time, if a non-IS vertex v has w=isn[v]>=0, it was once an IS vertex and get 1-1-swapped;
		since 1-1-swap does not increase the independent set (and for greedy algorithm very possibly deg(v)<deg(w)
		), swap back. */
		for(int i=0;i<n;i++)
		{
			if(mis.status(i)==false && isn[i]>=0 && mis.status(isn[i])==true && isn[isn[i]]==NIL)
			{
				int pi=isn[i];
				mis.set(i,true);
				isn[i]=0;
				mis.set(pi,false);
				isn[pi]=NIL;
			}
		}
		
		/* third scan */
		infile.seekg(0, ios::beg);
		infile.read((char*)(&n),sizeof(n));
		for(int i=0;i<n;i++)
		{
			int v;
			int d;
			infile.read((char*)(&v),sizeof(v));
			infile.read((char*)(&d),sizeof(d));

			if(mis.status(v)==false)
			{
				bool hasAdjIS=false;
				int w;
				for(int j=0;j<d;j++)
				{
					infile.read((char*)(&w),sizeof(w));
					if(mis.status(w)==true)
						hasAdjIS=true;
				}
				if(!hasAdjIS) // 0-1-swap, making the independent set maximal
				{
					mis.set(v,true);
					canSwap=true;
				}
			}
			else
			{
				int dummy;
				for(int j=0;j<d;j++)
					infile.read((char*)(&dummy),sizeof(dummy));
			}
		}
		infile.close();
	}
};

#endif