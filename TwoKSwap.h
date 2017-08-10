#ifndef _TWOKSWAP_H_
#define _TWOKSWAP_H_

#include <fstream>
#include "MIS.h"

class TwoKSwap
{
public:
	/* For non-IS vertex:
	if it's a 1-k-swap candidate, i.e., having only one adjacent IS vertex, isn records that vertex, while isn2 is undefined;
	if it's a 2-k-swap candidate, i.e., having exactly two adjacent IS vertex, isn and isn2 record these two IS vertex;
	otherwise isn and isn2 are undefined (set NIL).
	For IS vertex:
	isn records the number of adjacent 1-k-swap candidates;
	isn2 only indicates whether the vertex has adjacent 2-k-swap candidates, and the value has no meaning. 
	Memory cost (theoretically): 2|V|log_2(|V|+1) bits. */
	int* isn;
	int* isn2;
	/* Indicating that the vertex does not participate in the swap. */
	static const int NIL=-1;
	/* Indicating if the iteration can stop or not. If this iteration has any 1-k-swap, 2-k-swap or 0-1-swap, another iteration is needed to 
	guarantee all possible swaps are performed. In practice the algorithm can stop much earlier, i.e., after 2 or 3 iterations. */
	bool canSwap;

	void swap(const char* file, MIS &mis, int T)
	{
		cout<<"Independent set is maximal?\t"<<mis.isMaximalIS(file)<<endl;

		int n=mis.n;		
		isn=new int[n];
		isn2=new int[n];
		for(int i=0;i<n;i++)
		{
			isn[i]=NIL;
			isn2[i]=NIL;
		}

		int iteration=0;
		canSwap=true;
		while(canSwap && iteration < T)
		{
			_2kswap(file,mis);
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
	(Notes: 1. The algorithm adopts the streaming mode, i.e., adjacency lists are sequentially scanned 
	but not recorded in memory at all. In practice, IS vertex has relatively low degree. If
	the adjacency list of a vertex of this kind (e.g., degree is O(logn)) can be cached in
	memory when it is processed, fewer scans are needed.
			2. For 2-k-swap, there are three basic patterns. For real world graph, most swaps belong to
	the pattern that two 1-k-swap candidates and one 2-k-swap candidate together swap their two adjacent
	IS vertices. Thus other two patterns are ignored. To handle all three patterns, twice the memory cost
	and another sequential scan are needed.) */
	void _2kswap(const char* file, MIS &mis)
	{
		canSwap=false;
		int n=mis.n;
		for(int i=0;i<n;i++)
		{
			/* For a IS vertex, isn records the number of adjacent 1-k-swap candidates, 
			and isn2 indicates if 2-k-swap candidates exist. */
			if(mis.status(i)==true)
			{
				isn[i]=0;
				isn2[i]=0;
			}
			else
			{
				isn[i]=i;
				isn2[i]=i;
			}
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
			if(mis.status(v)==false) // update isn info of a non-IS vertex, see if it's a 1-k-swap oe 2-k-swap candidate
			{
				int w;
				for(int j=0;j<d;j++)
				{
					infile.read((char*)(&w),sizeof(w));
					if(mis.status(w)==true)
					{
						if(isn[v]==v) 
							isn[v]=w;
						else if(isn[v]!=v && isn[v]>=0 && isn[v]!=w && isn2[v]==v)
							isn2[v]=w;
						else if(isn[v]!=v && isn[v]>=0 && isn[v]!=w && isn2[v]!=v && isn2[v]>=0 && isn2[v]!=w)
						{
							isn[v]=NIL;
							isn2[v]=NIL;
						}
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

		// update isn and isn2 info of IS vertices
		for(int i=0;i<n;i++)
		{
			if(mis.status(i)==false && isn[i]>=0 && isn2[i]==i)
			{
				if(mis.status(isn[i])==true)
					isn[isn[i]]++;
				else // should never reach here for a simple graph
				{
					cout<<i<<"\t"<<mis.status(i)<<"\t"<<isn[i]<<"\t"<<isn2[i]<<endl;
					//system("PAUSE");
				}
			}
			else if(mis.status(i)==false && isn[i]>=0 && isn2[i]>=0 && isn2[i]!=i)
			{
				if(mis.status(isn[i])==true && mis.status(isn2[i])==true) // the value has no specific meaning
				{
					isn2[isn[i]]++;
					isn2[isn2[i]]++;
				}
				else // should never reach here for a simple graph
				{
					cout<<i<<"\t"<<mis.status(i)<<"\t"<<isn[i]<<"\t"<<isn2[i]<<endl;
					//system("PAUSE");
				}
			}
		} // end for
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
			if(mis.status(v)==false && isn[v]>=0 && isn2[v]==v)
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
					both v and w are 1-k-swap candidates, with different adjacent IS vertices;
					isn[v] is not swapped yet;
					isn[w] is 1-1-swapped, by isn[isn[w]];
					then v will not do swap to give chance to w for a 1-2-swap. */
					else if(mis.status(w)==false && isn[w]>=0 && isn2[w]==w && mis.status(isn[v])==true && mis.status(isn[w])==false && isn[isn[w]]>=0)
						vertexCanSwap=false;

					if(w!=v && isn[w]==isn[v] && isn2[w]==w)
						count++;
				}
				// if v has edges to all other remaining 1-k-swap candidates of isn[v], and isn[v] has no adjacent 2-k-swap candidate, can not swap 
				if(mis.status(isn[v])==true && count==isn[isn[v]]-1 && isn2[isn[v]]==0)
					vertexCanSwap=false;

				if(vertexCanSwap)
				{
					int pv=isn[v];
					mis.set(v,true);
					// for a newly swapped IS vertex, isn=NIL; otherwise, isn>=0 (=# of adjacent candidates)
					isn[v]=NIL;
					// In fact, the swap algorithm will stop or not can be determined by if the independent set size increases after this iteration.
					canSwap=true;
					// Note that 1-1-swap is allowed here.
					if(mis.status(pv)==true)
					{
						mis.set(pv,false);
						isn[pv]=NIL;
					}
				}
				else // 1-k-swap candidate can not swap, update isn info of its adjacent IS vertex and itself
				{
					if(mis.status(isn[v])==true && isn[isn[v]]>0)
					{
						isn[isn[v]]--;
						isn[v]=NIL;
					}
				}
			} // end if v is a 1-k-swap candidate

			/* v is a 2-k-swap candidate (only handles the case 2-k-swap=1-1-swap + 1-k-swap) */
			else if(mis.status(v)==false && isn[v]>=0 && isn2[v]>=0 && isn2[v]!=v)
			{
				// Case 1. isn2[v] is swapped, isn[v] not. v tries to swap isn[v] by 1-k-swap.
				if(mis.status(isn[v])==true && mis.status(isn2[v])==false)
				{
					bool vertexCanSwap=true;
					int w;
					int count=0;
					for(int j=0;j<d;j++)
					{
						infile.read((char*)(&w),sizeof(w));
						if(mis.status(w)==true && isn[v]!=w)
							vertexCanSwap=false;
						// w is a 1-k-swap candidate of isn[v]
						if(mis.status(w)==false && isn[w]==isn[v] && isn2[w]==w) 
							count++;
					}
					if(count==isn[isn[v]])
						vertexCanSwap=false;

					if(vertexCanSwap)
					{
						int pv=isn[v];
						mis.set(v,true);
						isn[v]=NIL;
						isn2[v]=NIL;
						mis.set(pv,false);
						isn[pv]=NIL;
					}
				}
				// Case 2. isn[v] is swapped, isn2[v] not. v tries to swap isn2[v] by 1-k-swap.
				else if(mis.status(isn[v])==false && mis.status(isn2[v])==true)
				{
					bool vertexCanSwap=true;
					int w;
					int count=0;
					for(int j=0;j<d;j++)
					{
						infile.read((char*)(&w),sizeof(w));
						if(mis.status(w)==true && isn2[v]!=w)
							vertexCanSwap=false;
						// w is a 1-k-swap candidate of isn2[v]
						if(mis.status(w)==false && isn[w]==isn2[v] && isn2[w]==w)
							count++;
					}
					if(count==isn[isn2[v]])
						vertexCanSwap=false;

					if(vertexCanSwap)
					{
						int pv=isn2[v];
						mis.set(v,true);
						isn[v]=NIL;
						isn2[v]=NIL;
						mis.set(pv,false);
						isn[pv]=NIL;
					}
				}
				// Case 3. Both isn[v] and isn2[v] are swapped. Try 0-1-swap.
				else if(mis.status(isn[v])==false && mis.status(isn2[v])==false)
				{
					bool vertexCanSwap=true;
					int w;
					int count=0;
					for(int j=0;j<d;j++)
					{
						infile.read((char*)(&w),sizeof(w));
						if(mis.status(w)==true)
							vertexCanSwap=false;
					}
					if(vertexCanSwap)
					{
						mis.set(v,true);
						isn[v]=NIL;
						isn2[v]=NIL;
					}
				}
				else
				{
					int dummy;
					for(int j=0;j<d;j++)
						infile.read((char*)(&dummy),sizeof(dummy));
				}
			} // end if v is a 2-k-swap candidate

			else // do nothing
			{
				int dummy;
				for(int j=0;j<d;j++)
					infile.read((char*)(&dummy),sizeof(dummy));
			}
		} // end for

		// note that no cancellation of 1-1-swap here

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