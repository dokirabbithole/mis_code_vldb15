#ifndef FIBNODE_H
#define FIBNODE_H

template<class T1, class T2>
class FibNode
  {
  public:
    FibNode(T1 k, T2 v, void *pl)
      :key(k),value(v),mark(false),p(nullptr),left(nullptr),right(nullptr),child(nullptr),degree(-1),payload(pl)
    {
    }
    
    ~FibNode()
      {
      }
	T1 getKey()
	{
		return key;
	}
	T2 getValue()
	{
		return value;
	}

    T1 key;
	T2 value;
    bool mark;
    FibNode *p;
    FibNode *left;
    FibNode *right;
    FibNode *child;
    int degree;
    void *payload;
  }; // end FibNode

#endif