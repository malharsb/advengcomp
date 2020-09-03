#include <stdio.h>
#include <cstddef>

#include <iostream> //malhar
using namespace std;

template <class KeyClass,class ValueClass>
class BinaryTree
{
protected:
	class Node
	{
	public:
		KeyClass key;
		ValueClass value;
		Node *left,*right,*up;
		int height;
		

		Node() : left(nullptr),right(nullptr),up(nullptr),height(1)
		{
		}

		
	};
public:
	
	bool autoRebalance;

	class NodeHandle
	{
	friend BinaryTree <KeyClass,ValueClass>;
	private:
		Node *ptr;
	public:
		inline void Nullify(void)
		{
			ptr=nullptr;
		}
		inline bool IsNull(void) const
		{
			return ptr==nullptr;
		}
		inline bool IsNotNull(void) const
		{
			return ptr!=nullptr;
		}
		inline bool operator==(NodeHandle hd) const
		{
			return this->ptr==hd.ptr;
		}
		inline bool operator!=(NodeHandle hd) const
		{
			return this->ptr!=hd.ptr;
		}
		inline bool operator==(std::nullptr_t) const
		{
			return ptr==nullptr;
		}
		inline bool operator!=(std::nullptr_t) const
		{
			return ptr!=nullptr;
		}
	};
protected:
// public:

	int GetBalanceFactor(NodeHandle ndHd)
	{
		auto nodePtr=GetNode(ndHd);
		auto hleft = GetHeight(MakeHandle(nodePtr->left));
		auto hright = GetHeight(MakeHandle(nodePtr->right));

		auto bf = hleft - hright;
		return bf;
	}

	void Rebalance(NodeHandle ndHd)
	{
		auto nodePtr = GetNode(ndHd);
		if (ndHd==nullptr) return;

		
		while (nodePtr!=nullptr)
		{
			auto bf = GetBalanceFactor(MakeHandle(nodePtr));
			int lc_bf, rc_bf;

			if (bf>1)
			{
				auto lc_hleft = GetHeight(MakeHandle(nodePtr->left->left));
				auto lc_hright = GetHeight(MakeHandle(nodePtr->left->right));
				lc_bf = lc_hleft - lc_hright;
				// printf("bf: %d bf_lc: %d\n",bf, lc_bf);
			}
			else if (bf<-1)
			{
				auto rc_hleft = GetHeight(MakeHandle(nodePtr->right->left));
				auto rc_hright = GetHeight(MakeHandle(nodePtr->right->right));
				rc_bf = rc_hleft - rc_hright;
				// printf("bf: %d bf_rc: %d\n",bf, rc_bf);
			}
			// else
			// {
			// 	// printf("bf: %d\n",bf);
			// 	continue;
			// }

			
			if (abs(bf)<=1)
			{
				if (nodePtr->up == nullptr) {
					// printf("Tree is balanced\n");
					break;
				}
				else nodePtr = nodePtr->up;
			}
			else if (abs(bf)>1)
			{
				
				if (bf>1 && lc_bf>=0) // LL imbalance
				{
					RotateRight(MakeHandle(nodePtr));
				}
				else if (bf<1 && rc_bf<=0) // RR imbalance
				{
					RotateLeft(MakeHandle(nodePtr));
				}
				else if (bf>1 && lc_bf<0) // LR imbalance
				{	
					auto tempPtr2 = nodePtr;
					RotateLeft(MakeHandle(nodePtr->left));
					RotateRight(MakeHandle(tempPtr2));
				}
				else if (bf<1 && rc_bf>0) // RL imbalance
				{
					auto tempPtr2 = nodePtr;
					RotateRight(MakeHandle(nodePtr->right));
					RotateLeft(MakeHandle(tempPtr2));
				}
				else
				{
					printf("exception!");
				}
			}
		}
		// printf("Balanced");
	}
	Node *GetNode(NodeHandle ndHd)
	{
		if(ndHd.IsNotNull())
		{
			return ndHd.ptr;
		}
		return nullptr;
	}
	const Node *GetNode(NodeHandle ndHd) const
	{
		if(ndHd.IsNotNull())
		{
			return ndHd.ptr;
		}
		return nullptr;
	}
	static NodeHandle MakeHandle(Node *nodePtr)
	{
		NodeHandle ndHd;
		ndHd.ptr=nodePtr;
		return ndHd;
	}
	bool UpdateHeight(Node *nodePtr)
	{
		int leftHeight=1,rightHeight=1;
		if(nullptr!=nodePtr->left)
		{
			leftHeight=nodePtr->left->height+1;
		}
		if(nullptr!=nodePtr->right)
		{
			rightHeight=nodePtr->right->height+1;
		}
		int newHeight=1;
		if(leftHeight>rightHeight)
		{
			newHeight=leftHeight;
		}
		else
		{
			newHeight=rightHeight;
		}
		if(newHeight!=nodePtr->height)
		{
			nodePtr->height=newHeight;
			return true;
		}
		return false;
	}
	void UpdateHeightCascade(Node *nodePtr)
	{
		bool first=true;
		while(nullptr!=nodePtr)
		{
			auto changed=UpdateHeight(nodePtr);
			if(true!=first && true!=changed)
			{
				break;
			}
			nodePtr=nodePtr->up;
			first=false;
		}
	}

private:
	Node *root;
	long long int nElem;

public:
	BinaryTree()
	{
		root=nullptr;
		nElem=0;
		autoRebalance=false; // Always remains false by default
	}
	~BinaryTree()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		CleanUp(GetNode(RootNode()));
	}
private:
	void CleanUp(Node *nodePtr)
	{
		if(nullptr!=nodePtr)
		{
			CleanUp(nodePtr->left);
			CleanUp(nodePtr->right);
			delete nodePtr;
		}
	}
public:
	static NodeHandle Null(void)
	{
		NodeHandle ndHd;
		ndHd.ptr=nullptr;
		return ndHd;
	}
	NodeHandle RootNode(void) const
	{
		return MakeHandle(root);
	}
	NodeHandle Left(NodeHandle ndHd) const
	{
		auto nodePtr=GetNode(ndHd);
		if(nullptr!=nodePtr)
		{
			return MakeHandle(nodePtr->left);
		}
		return Null();
	}
	NodeHandle Up(NodeHandle ndHd) const
	{
		auto nodePtr=GetNode(ndHd);
		if(nullptr!=nodePtr)
		{
			return MakeHandle(nodePtr->up);
		}
		return Null();
	}
	NodeHandle Right(NodeHandle ndHd) const
	{
		auto nodePtr=GetNode(ndHd);
		if(nullptr!=nodePtr)
		{
			return MakeHandle(nodePtr->right);
		}
		return Null();
	}

	long long int GetN(void) const
	{
		return nElem;
	}
	const KeyClass &GetKey(NodeHandle ndHd) const
	{
		// This will crash if ndHd==nullptr.  Therefore, ndHd must be non-null to use this function.
		return GetNode(ndHd)->key;
	}
	ValueClass &GetValue(NodeHandle ndHd)
	{
		// This will crash if ndHd==nullptr.  Therefore, ndHd must be non-null to use this function.
		return GetNode(ndHd)->value;
	}
	const ValueClass &GetValue(NodeHandle ndHd) const
	{
		// This will crash if ndHd==nullptr.  Therefore, ndHd must be non-null to use this function.
		return GetNode(ndHd)->value;
	}
	NodeHandle FindNode(const KeyClass &key) const
	{
		auto ndHd=RootNode();
		while(nullptr!=ndHd)
		{
			if(key==GetKey(ndHd))
			{
				return ndHd;
			}
			if(key<GetKey(ndHd))
			{
				ndHd=Left(ndHd);
			}
			else
			{
				ndHd=Right(ndHd);
			}
		}
		return Null();
	}
	bool IsKeyIncluded(const KeyClass &key) const
	{
		return FindNode(key).IsNotNull();
	}
	int GetHeight(NodeHandle ndHd) const
	{
		auto nodePtr=GetNode(ndHd);
		if(nullptr!=nodePtr)
		{
			return nodePtr->height;
		}
		return 0;
	}

	NodeHandle Insert(const KeyClass &key,const ValueClass &value)
	{
		auto newNode=new Node;
		newNode->key=key;
		newNode->value=value;

		auto ndHd=RootNode();
		if(ndHd.IsNull())
		{
			root=newNode;
		}
		else
		{
			while(ndHd.IsNotNull())
			{
				if(key<GetKey(ndHd))
				{
					if(Left(ndHd)!=nullptr)
					{
						ndHd=Left(ndHd);
					}
					else
					{
						GetNode(ndHd)->left=newNode;
						newNode->up=GetNode(ndHd);
						break;
					}
				}
				else
				{
					if(Right(ndHd)!=nullptr)
					{
						ndHd=Right(ndHd);
					}
					else
					{
						GetNode(ndHd)->right=newNode;
						newNode->up=GetNode(ndHd);
						break;
					}
				}
			}
		}

		UpdateHeightCascade(newNode);
		nElem++;

		// malhar
		if (autoRebalance) Rebalance(MakeHandle(newNode));

		return MakeHandle(newNode);
	}

	NodeHandle First(void) const
	{
		auto ndHd=RootNode();
		while(Left(ndHd).IsNotNull())
		{
			ndHd=Left(ndHd);
		}
		return ndHd;
	}
	NodeHandle FindNext(NodeHandle ndHd) const
	{
		auto rightHd=Right(ndHd);
		if(rightHd.IsNotNull())
		{
			// Has a right sub-tree.
			// The next node is the left-most of the right sub-tree.
			ndHd=Right(ndHd);
			while(Left(ndHd).IsNotNull())
			{
				ndHd=Left(ndHd);
			}
			return ndHd;
		}
		else
		{
			// Does not have a right sub-tree.
			// Go up until it goes up from the left.
			while(ndHd.IsNotNull())
			{
				auto upHd=Up(ndHd);
				if(upHd.IsNotNull() && ndHd==Left(upHd))
				{
					return upHd;
				}
				ndHd=upHd;
			}
			return Null();
		}
	}
	NodeHandle Last(void) const  // Do it in the assignment.
	{
		auto ndHd = RootNode();
		while (Right(ndHd).IsNotNull())
		{
			ndHd = Right(ndHd);
		}
		return ndHd;
	}

	

	NodeHandle FindPrev(NodeHandle ndHd) const // Do it in the assignment.
	{
		auto leftHd = Left(ndHd);
		if (leftHd.IsNotNull())
		{


			ndHd = Left(ndHd);
			while(Right(ndHd).IsNotNull())
			{
				ndHd = Right(ndHd);
			}
			return ndHd;
		}
		else
		{


			while (ndHd.IsNotNull())
			{
				auto upHd = Up(ndHd);
				if (upHd.IsNotNull() && ndHd==Right(upHd))
				{
					return upHd;
				}
				ndHd=upHd;
			}
			return Null();
		}
	}


private:
	NodeHandle RightMostOf(NodeHandle ndHd)
	{
		while(Right(ndHd).IsNotNull())
		{
			ndHd=Right(ndHd);
		}
		return ndHd;
	}
	bool SimpleDetach(NodeHandle ndHd)
	{
		if(ndHd.IsNotNull())
		{
			auto upHd=Up(ndHd);
			auto rightHd=Right(ndHd);
			auto leftHd=Left(ndHd);
			if(rightHd.IsNull() && leftHd.IsNull())
			{
				if(upHd.IsNull()) // ndHd is a root.
				{
					root=nullptr;
				}
				else
				{
					auto upPtr=GetNode(upHd);
					if(Left(upHd)==ndHd)
					{
						upPtr->left=nullptr;
					}
					else if(Right(upHd)==ndHd)
					{
						upPtr->right=nullptr;
					}
					else
					{
						fprintf(stderr,"Error! Internal Tree Data Structure is broken.\n");
						return false;
					}
				}
				UpdateHeightCascade(GetNode(upHd));
				return true;
			}
			else if(rightHd.IsNull())
			{
				if(upHd.IsNull())
				{
					root=GetNode(leftHd);
					root->up=nullptr;
					return true;
				}
				else
				{
					// Connect upHd and leftHd
					auto upPtr=GetNode(upHd);
					auto leftPtr=GetNode(leftHd);
					if(Left(upHd)==ndHd)
					{
						upPtr->left=leftPtr;
						leftPtr->up=upPtr;
						UpdateHeightCascade(GetNode(upHd));
						return true;
					}
					else if(Right(upHd)==ndHd)
					{
						upPtr->right=leftPtr;
						leftPtr->up=upPtr;
						UpdateHeightCascade(GetNode(upHd));
						return true;
					}
					else
					{
						fprintf(stderr,"Error! Internal Tree Data Structure is broken.\n");
						return false;
					}
				}
			}
			else if(leftHd.IsNull())
			{
				if(upHd.IsNull())
				{
					root=GetNode(rightHd);
					root->up=nullptr;
					return true;
				}
				else
				{
					// Connect upHd and rightHd
					auto upPtr=GetNode(upHd);
					auto rightPtr=GetNode(rightHd);
					if(Left(upHd)==ndHd)
					{
						upPtr->left=rightPtr;
						rightPtr->up=upPtr;
						UpdateHeightCascade(GetNode(upHd));
						return true;
					}
					else if(Right(upHd)==ndHd)
					{
						upPtr->right=rightPtr;
						rightPtr->up=upPtr;
						UpdateHeightCascade(GetNode(upHd));
						return true;
					}
					else
					{
						fprintf(stderr,"Error! Internal Tree Data Structure is broken.\n");
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}
		return false;
	}
public:
	bool Delete(NodeHandle ndHd)
	{	
	
		if(true==SimpleDetach(ndHd))
		{
			auto start_rebal = GetNode(Up(ndHd));
			delete GetNode(ndHd);
			if (autoRebalance) Rebalance(MakeHandle(start_rebal));
			
			--nElem;
			return true;
		}
		else if(ndHd.IsNotNull())
		{
			// Right most of left. Always Simple-Detachable.
			// Also, since SimpleDetach of itself has failed, it must have a left sub-tree.
			auto RMOL=RightMostOf(Left(ndHd));
			if(true==SimpleDetach(RMOL))
			{
				auto start_rebal = GetNode(Left(ndHd));
				// Now, RMOL needs to take position of ndHd.
				auto RMOLptr=GetNode(RMOL);
				auto upPtr=GetNode(Up(ndHd));
				auto leftPtr=GetNode(Left(ndHd));
				auto rightPtr=GetNode(Right(ndHd));

				auto start_rebal2 = RMOLptr->up;

				auto upOfRMOLptr=RMOLptr->up;
				if(upOfRMOLptr==GetNode(ndHd))
				{
					upOfRMOLptr=RMOLptr;	// Now it is correct.
					start_rebal2 = upOfRMOLptr;
				}

				if(nullptr==upPtr)
				{
					root=RMOLptr;
					root->up=nullptr;
				}
				else if(upPtr->left==GetNode(ndHd))
				{
					upPtr->left=RMOLptr;
					RMOLptr->up=upPtr;
				}
				else if(upPtr->right==GetNode(ndHd))
				{
					upPtr->right=RMOLptr;
					RMOLptr->up=upPtr;
				}
				else
				{
					fprintf(stderr,"Error! Internal Tree Data Structure is broken.\n");
					return false;
				}

				RMOLptr->left=leftPtr;
				if(nullptr!=leftPtr)
				{
					leftPtr->up=RMOLptr;
				}
				RMOLptr->right=rightPtr;
				if(nullptr!=rightPtr)
				{
					rightPtr->up=RMOLptr;
				}

				UpdateHeightCascade(RMOLptr);

				
				delete GetNode(ndHd);
				if (autoRebalance) Rebalance(MakeHandle(start_rebal2));

				--nElem;
				return true;
			}
		}
		return false; // Cannot delete a null node.
	}

	bool RotateLeft(NodeHandle ndHd)
	{
		auto nodePtr=GetNode(ndHd); // Get node pointer
		if(nullptr!=nodePtr && nullptr!=nodePtr->right) 
		// For left rotation of a node, its right child will replace it, hence
		// cannot be NULL
		{
			auto rightPtr=nodePtr->right;
			auto leftOfRight=nodePtr->right->left;

			if(nullptr==nodePtr->up) // If node is the root
			{
				root=rightPtr;
				rightPtr->up=nullptr;
			}
			else // If node is not the tree root
			{
				rightPtr->up=nodePtr->up;
				if(nodePtr->up->left==nodePtr)
				{
					nodePtr->up->left=rightPtr;
				}
				else
				{
					nodePtr->up->right=rightPtr;
				}
			}

			rightPtr->left=nodePtr;
			nodePtr->up=rightPtr;

			nodePtr->right=leftOfRight;
			if(nullptr!=leftOfRight)
			{
				leftOfRight->up=nodePtr;
			}
			UpdateHeight(nodePtr);
			UpdateHeightCascade(rightPtr);
			return true;
		}
		return false;
	}


	bool RotateRight(NodeHandle ndHd)
	{
		auto nodePtr=GetNode(ndHd); // Get node pointer
		if(nullptr!=nodePtr && nullptr!=nodePtr->left) 
		// For right rotation of a node, its left child will replace it, hence
		// cannot be NULL
		{
			auto leftPtr=nodePtr->left;
			auto rightOfLeft=nodePtr->left->right;

			if(nullptr==nodePtr->up) // If node is the root
			{
				root=leftPtr;
				leftPtr->up=nullptr;
			}
			else // If node is not the tree root
			{
				leftPtr->up=nodePtr->up;
				if(nodePtr->up->left==nodePtr)
				{
					nodePtr->up->left=leftPtr;
				}
				else
				{
					nodePtr->up->right=leftPtr;
				}
			}

			leftPtr->right=nodePtr;
			nodePtr->up=leftPtr;

			nodePtr->left=rightOfLeft;
			if(nullptr!=rightOfLeft)
			{
				rightOfLeft->up=nodePtr;
			}
			UpdateHeight(nodePtr);
			UpdateHeightCascade(leftPtr);
			return true;
		}
		return false;
	}

	void TreeToVine() // DO ALL MOUSEON COMMANDS NEED A BOOL OUTPUT???

	// How do you get the root node???
	{
		// auto nodePtr=GetNode(ndHd); // Get node pointer
		auto nodePtr = root;

		
		if(nullptr==nodePtr) return; // CHECK CONDITION

		auto vine_tail = nodePtr;
		
		

		while(vine_tail->left!=nullptr)
		{
			auto temp = vine_tail->left;
			// std::cout << "vine_tail is:" << vine_tail->left << "\n";
			RotateRight(MakeHandle(vine_tail));
			vine_tail = temp;
		}
		int counter=0;

		auto remainder = vine_tail->right;
		while(remainder!=nullptr)
		{

			if (remainder->left==nullptr)
			{
				vine_tail = remainder;
				remainder = remainder->right;
			}

			else
			{
				auto rotr = RotateRight(MakeHandle(remainder));
				remainder = vine_tail->right;
			}
		}

	}

	void Compress(int N)
	{
		auto nodePtr = root;

		for (int i=0; i<= 2*(N-1); i+=2)
		{
			auto temp = nodePtr->right->right;
			RotateLeft(MakeHandle(nodePtr));
			nodePtr = temp;
		}
	}

	void VineToTree()
	{

		auto temp = root;
		while (temp!=nullptr)
		{
			if (temp->left!=nullptr)
			{
				printf("%s\n", "Not a vine");
				return;
			}
			temp=temp->right;
		}
				
		auto sz = GetN();
		auto exp = int(std::log2(sz+1));
		auto lc = sz + 1 - pow(2,exp);
		Compress(lc);
		sz=sz-lc;

		while(sz>1)
		{
			Compress(sz/2);
			sz/=2;
		}

	}


};


