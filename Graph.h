#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;
/* Graphs are assumed to be simple, i.e., no self loops or multi-edges. */
class Graph{
public:
	/* Number of vertices */
	int n;
	/* Number of edges */
	long m;
	/* Adjacency lists */
	int** adj;
	
	int* deg;
	Graph()
	{
		n=0;
		m=0;
	}
	Graph(const char* file)
	{
		n=0;
		m=0;
		readAdjacencyListBinary(file);
	}
	~Graph()
	{
		for(int i=0;i<n;i++)
			delete[] adj[i];
		delete[] adj;
		delete[] deg;
	}

	/* load the graph into memory */
	void readAdjacencyListBinary(const char* file)
	{
		ifstream infile(file, ios::binary);
		infile.read((char*)(&n), sizeof(n));
		adj=new int*[n];
		deg=new int[n];
		for(int i=0;i<n;i++)
		{
			int id;
			infile.read((char*)(&id), sizeof(id));
			int degree;
			infile.read((char*)(&degree), sizeof(degree));

			adj[id]=new int[degree];
			deg[id]=degree;
			for(int j=0;j<degree;j++)
			{
				int w;
				infile.read((char*)(&w), sizeof(w));
				adj[id][j]=w;
			}
			m+=degree;
		}
		infile.close();
		cout << "read binary file " << file << "\t done" << endl;
		cout << "n= " << n << " , m= " << m/2 << endl;
	}

	void readTextWriteBinary(const char* textInFile, const char* binaryOutFile)
	{
		ifstream infile(textInFile);
		ofstream outfile(binaryOutFile, ios::binary);
		int graphsize;
		infile >> graphsize;
		outfile.write((char*)(&graphsize), sizeof(graphsize));
		//cout<<graphsize<<endl;
		for(int i=0;i<graphsize;i++)
		{
			int id;
			int degree;
			infile >> id;
			outfile.write((char*)(&id), sizeof(id));
			//cout<<id;
			infile >> degree;
			outfile.write((char*)(&degree), sizeof(degree));
			//cout<<" "<<degree; 
			for(int j=0;j<degree;j++)
			{
				int w;
				infile >> w;
				outfile.write((char*)(&w), sizeof(w));
				//cout<<" "<<w;
			}
			//cout<<endl;
		}
		infile.close();
		outfile.close();
	}
};

#endif