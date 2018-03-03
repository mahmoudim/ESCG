#ifndef FIBHEAP_H
#define FIBHEAP_H

//***************************************************************************
// The Fibonacci heap implementation contained in FIBHEAP.H and FIBHEAP.CPP
// is Copyright (c) 1996 by John Boyer
//
// Once this Fibonacci heap implementation (the software) has been published
// by Dr. Dobb's Journal, permission to use and distribute the software is
// granted provided that this copyright notice remains in the source and
// and the author (John Boyer) is acknowledged in works that use this program.
//
// Every effort has been made to ensure that this implementation is free of
// errors.  Nonetheless, the author (John Boyer) assumes no liability regarding
// your use of this software.
//
// The author would also be very glad to hear from anyone who uses the
// software or has any feedback about it.
// Email: jboyer@gulf.csc.uvic.ca
//***************************************************************************

#define OK      0
#define NOTOK   -1

#include <cstdio>

//======================================================
// Fibonacci Heap Node Class
//======================================================

class FibHeap;

class FibHeapNode
{
	friend class FibHeap;
	FibHeapNode *Left, *Right, *Parent, *Child;
	short Degree, Mark, NegInfinityFlag;

protected:
	inline int  FHN_Cmp(FibHeapNode& RHS) {
		if (NegInfinityFlag)
			return RHS.NegInfinityFlag ? 0 : -1;
		return RHS.NegInfinityFlag ? 1 : 0;
	}
	inline void FHN_Assign(FibHeapNode& RHS) {
		NegInfinityFlag = RHS.NegInfinityFlag;
	}

public:
	inline FibHeapNode() {
		Left = Right = Parent = Child = NULL;
		Degree = Mark = NegInfinityFlag = 0;
	}
	~FibHeapNode()
	{
	}
	virtual void operator =(FibHeapNode& RHS)
	{
	     FHN_Assign(RHS);
	     // Key assignment goes here in derived classes
	}

	virtual int  operator ==(FibHeapNode& RHS)
	{
	     if (FHN_Cmp(RHS)) return 0;
	     // Key compare goes here in derived classes
	     return 1;
	}
	virtual int operator <(FibHeapNode& RHS)
	{
		int X;

		if ((X=FHN_Cmp(RHS)) != 0)
			return X < 0 ? 1 : 0;
	     // Key compare goes here in derived classes
	     return 0;
	}

	virtual void Print()
	{
	     //if (NegInfinityFlag)
		 //cout << "-inf.";
	}

};

//========================================================================
// Fibonacci Heap Class
//========================================================================

class FibHeap
{
	FibHeapNode *MinRoot;
	long NumNodes, NumTrees, NumMarkedNodes;
	int HeapOwnershipFlag;

public:
	FibHeap()
	{
	     MinRoot = NULL;
	     NumNodes = NumTrees = NumMarkedNodes = 0;
	     ClearHeapOwnership();
	}
	virtual ~FibHeap()
	{
		FibHeapNode *Temp;

	     if (GetHeapOwnership())
	     {
	         while (MinRoot != NULL)
 	        {
  	           Temp = ExtractMin();
	             delete Temp;
		 }
  	   }
	}

	// The Standard Heap Operations
	void Insert(FibHeapNode *NewNode)
{
     if (NewNode == NULL) return;

// If the heap is currently empty, then new node becomes singleton
// circular root list
 
     if (MinRoot == NULL)
	 MinRoot = NewNode->Left = NewNode->Right = NewNode;

     else
     {
// Pointers from NewNode set to insert between MinRoot and MinRoot->Right

         NewNode->Right = MinRoot->Right;
	 NewNode->Left = MinRoot;

// Set Pointers to NewNode  

	 NewNode->Left->Right = NewNode;
         NewNode->Right->Left = NewNode;

// The new node becomes new MinRoot if it is less than current MinRoot

         if (*NewNode < *MinRoot)
	     MinRoot = NewNode;
     }

// We have one more node in the heap, and it is a tree on the root list

     NumNodes++;

     NumTrees++;
     NewNode->Parent = NULL;
}
	void Union(FibHeap *OtherHeap)
{
FibHeapNode *Min1, *Min2, *Next1, *Next2;

     if (OtherHeap == NULL || OtherHeap->MinRoot == NULL) return;

// We join the two circular lists by cutting each list between its
// min node and the node after the min.  This code just pulls those
// nodes into temporary variables so we don't get lost as changes
// are made.

     Min1 = MinRoot;
     Min2 = OtherHeap->MinRoot;
     Next1 = Min1->Right;
     Next2 = Min2->Right;

// To join the two circles, we join the minimum nodes to the next
// nodes on the opposite chains.  Conceptually, it looks like the way
// two bubbles join to form one larger bubble.  They meet at one point
// of contact, then expand out to make the bigger circle.
 
     Min1->Right = Next2;
     Next2->Left = Min1;
     Min2->Right = Next1;
     Next1->Left = Min2;

// Choose the new minimum for the heap
 
     if (*Min2 < *Min1)
         MinRoot = Min2;

// Set the amortized analysis statistics and size of the new heap
                   
     NumNodes += OtherHeap->NumNodes;
     NumMarkedNodes += OtherHeap->NumMarkedNodes;
     NumTrees += OtherHeap->NumTrees;

// Complete the union by setting the other heap to emptiness
// then destroying it

     OtherHeap->MinRoot  = NULL;
     OtherHeap->NumNodes =
     OtherHeap->NumTrees =
     OtherHeap->NumMarkedNodes = 0;

     delete OtherHeap;
}
	inline FibHeapNode *Minimum() { return MinRoot; }
	FibHeapNode *ExtractMin()
{
FibHeapNode *Result;
FibHeap *ChildHeap = NULL;

// Remove minimum node and set MinRoot to next node

     if ((Result = Minimum()) == NULL)
          return NULL;

     MinRoot = Result->Right;
     Result->Right->Left = Result->Left;
     Result->Left->Right = Result->Right;
     Result->Left = Result->Right = NULL;

     NumNodes --;
     if (Result->Mark)
     {
	 NumMarkedNodes --;
         Result->Mark = 0;
     }
     Result->Degree = 0;

// Attach child list of Minimum node to the root list of the heap
// If there is no child list, then do no work

     if (Result->Child == NULL)
     {
	 if (MinRoot == Result)
	     MinRoot = NULL;
     }

// If MinRoot==Result then there was only one root tree, so the
// root list is simply the child list of that node (which is
// NULL if this is the last node in the list)

     else if (MinRoot == Result)
         MinRoot = Result->Child;

// If MinRoot is different, then the child list is pushed into a
// new temporary heap, which is then merged by Union() onto the
// root list of this heap.

     else 
     {
         ChildHeap = new FibHeap();
         ChildHeap->MinRoot = Result->Child;
     }

// Complete the disassociation of the Result node from the heap

     if (Result->Child != NULL)
	 Result->Child->Parent = NULL;
     Result->Child = Result->Parent = NULL;

// If there was a child list, then we now merge it with the
//	rest of the root list

     if (ChildHeap)
         Union(ChildHeap);

// Consolidate heap to find new minimum and do reorganize work

     if (MinRoot != NULL)
         _Consolidate();

// Return the minimum node, which is now disassociated with the heap
// It has Left, Right, Parent, Child, Mark and Degree cleared.

     return Result;
}
	int DecreaseKey(FibHeapNode *theNode, FibHeapNode& NewKey)
{
FibHeapNode *theParent;

     if (theNode==NULL || *theNode < NewKey)
         return NOTOK;

     *theNode = NewKey;

     theParent = theNode->Parent;
     if (theParent != NULL && *theNode < *theParent)
     {
         _Cut(theNode, theParent);
         _CascadingCut(theParent);
     }

     if (*theNode < *MinRoot)
         MinRoot = theNode;

     return OK;
}

	int Delete(FibHeapNode *theNode)
{
FibHeapNode Temp;
int Result;

     if (theNode == NULL) return NOTOK;

     Temp.NegInfinityFlag = 1;
     Result = DecreaseKey(theNode, Temp);

     if (Result == OK)
         if (ExtractMin() == NULL)
             Result = NOTOK;

     if (Result == OK)
     {
         if (GetHeapOwnership())
	      delete theNode;
	 else theNode->NegInfinityFlag = 0;
     }
         
     return Result;
}


	// Extra utility functions
	int  GetHeapOwnership() { return HeapOwnershipFlag; };
	void SetHeapOwnership() { HeapOwnershipFlag = 1; };
	void ClearHeapOwnership() { HeapOwnershipFlag = 0; };
	long GetNumNodes() { return NumNodes; };
	long GetNumTrees() { return NumTrees; };
	long GetNumMarkedNodes() { return NumMarkedNodes; };
	void Print(FibHeapNode *Tree = NULL, FibHeapNode *theParent=NULL){};

private:
	// Internal functions that help to implement the Standard Operations
	inline void _Exchange(FibHeapNode* &N1, FibHeapNode* &N2) {
		FibHeapNode *Temp; Temp = N1; N1 = N2; N2 = Temp;
	}
	void _Consolidate()
{
FibHeapNode *x, *y, *w;
FibHeapNode *A[1+8*sizeof(long)]; // 1+lg(n)
int  I=0, Dn = 1+8*sizeof(long);
short d;

// Initialize the consolidation detection array

     for (I=0; I < Dn; I++)
          A[I] = NULL;

// We need to loop through all elements on root list.
// When a collision of degree is found, the two trees
// are consolidated in favor of the one with the lesser
// element key value.  We first need to break the circle
// so that we can have a stopping condition (we can't go
// around until we reach the tree we started with
// because all root trees are subject to becoming a
// child during the consolidation).

     MinRoot->Left->Right = NULL;
     MinRoot->Left = NULL;
     w = MinRoot;

     do {
//cout << "Top of Consolidate's loop\n";
//Print(w);

	x = w;
        d = x->Degree;
        w = w->Right;

// We need another loop here because the consolidated result
// may collide with another large tree on the root list.

        while (A[d] != NULL)
        {
             y = A[d];
	     if (*y < *x)
		 _Exchange(x, y);
             if (w == y) w = y->Right;
             _Link(y, x);
             A[d] = NULL;
             d++;
//cout << "After a round of Linking\n";
//Print(x);
	}
	A[d] = x;

     } while (w != NULL);

// Now we rebuild the root list, find the new minimum,
// set all root list nodes' parent pointers to NULL and
// count the number of subtrees.

     MinRoot = NULL;
     NumTrees = 0;
     for (I = 0; I < Dn; I++)
          if (A[I] != NULL)
              _AddToRootList(A[I]);
}
	void _Link(FibHeapNode *y, FibHeapNode *x)
{
// Remove node y from root list

     if (y->Right != NULL)
	 y->Right->Left = y->Left;
     if (y->Left != NULL)
         y->Left->Right = y->Right;
     NumTrees--;

// Make node y a singleton circular list with a parent of x

     y->Left = y->Right = y;
     y->Parent = x;

// If node x has no children, then list y is its new child list

     if (x->Child == NULL)
	 x->Child = y;

// Otherwise, node y must be added to node x's child list

     else
     {
         y->Left = x->Child;
         y->Right = x->Child->Right;
         x->Child->Right = y;
         y->Right->Left = y;
     }

// Increase the degree of node x because it's now a bigger tree

     x->Degree ++;

// Node y has just been made a child, so clear its mark

     if (y->Mark) NumMarkedNodes--;
     y->Mark = 0;
}
	void _AddToRootList(FibHeapNode *x)
{
     if (x->Mark) NumMarkedNodes --;
     x->Mark = 0;

     NumNodes--;
     Insert(x);
}
	void _Cut(FibHeapNode *x, FibHeapNode *y)
{
     if (y->Child == x)
         y->Child = x->Right;
     if (y->Child == x)
	 y->Child = NULL;

     y->Degree --;

     x->Left->Right = x->Right;
     x->Right->Left = x->Left;

     _AddToRootList(x);
}
	void _CascadingCut(FibHeapNode *y)
{
FibHeapNode *z = y->Parent;

     while (z != NULL)
     {
         if (y->Mark == 0)
         {
             y->Mark = 1;
             NumMarkedNodes++;
             z = NULL;
         }
         else
         {
             _Cut(y, z);
             y = z;
	     z = y->Parent;
         }
     }
}
};

#endif /* FIBHEAP_H */
