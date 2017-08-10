#ifndef _MIS_H_
#define _MIS_H_

#include <fstream>
#include <iostream>

using namespace std;

class MIS
{
public:
	int n;
	bool* is;

	MIS(int n_)
	{
		n=n_;
		is=new bool[n];
		for(int i=0;i<n;i++)
			is[i]=true;
	}
	~MIS()
	{
		//delete[] is;
	}
	void set(int pos, bool val)
	{
		if(pos>=0 && pos<n)
			is[pos]=val;
	}
	bool status(int pos)
	{
		if(pos>=0 && pos<n)
			return is[pos];
	}
	int size()
	{
		int size=0;
		for(int i=0;i<n;i++)
			if(is[i]==true)
				size++;
		return size;
	}

	/* Check if it's an independent set. 
	Returns true if it's a maximal independent set;
	returns false if the independent set is not maximal;
	program terminates if it's not an independent set. */
	bool isMaximalIS(const char* file)
	{
		bool isMaximal=true;
		ifstream infile(file, ios::binary);
		int graphsize;
		infile.read((char*)(&graphsize), sizeof(graphsize));
		for(int i=0;i<graphsize;i++)
		{
			int id;
			int degree;
			infile.read((char*)(&id),sizeof(id));
			infile.read((char*)(&degree),sizeof(degree));

			if(status(id)==true)
			{
				int nbr;
				for(int j=0;j<degree;j++)
				{
					infile.read((char*)(&nbr),sizeof(nbr));
					if(status(nbr)==true)
					{
						cout<<"not an independent set\t"<<id<<" , "+status(id)<<"\t"<<nbr<<" , "<<status(nbr)<<endl;
						//system("PAUSE");
					}
				}
			}
			else //status(id)==false
			{
				bool hasAdjIS=false;
				int nbr;
				for(int j=0;j<degree;j++)
				{
					infile.read((char*)(&nbr),sizeof(nbr));
					if(status(nbr)==true)
						hasAdjIS=true;
				}
				if(hasAdjIS==false)
				{
					cout<<"independent set is not maximal\t"<<id<<" , "<<status(id)<<" has no adjacent IS vertex"<<endl;
					isMaximal=false;
				}
			}
		}
		infile.close();
		return isMaximal;
	}
};

#endif