#include <stdlib.h>
#include <time.h>

#include "bintree.h"

int main(void)
{
    srand((int)time(nullptr));

    BinaryTree <int,int> tree;
    for (int i=0; i<100; ++i)
    {
        tree.Insert(rand()%100,0);
    }
    for (auto hd=tree.Last(); true==hd.IsNotNull(); hd=tree.FindPrev(hd))
    {
        auto prev = tree.FindPrev(hd);
        if(true==prev.IsNotNull())
        {
            auto key1 = tree.GetKey(hd);
            auto key2 = tree.GetKey(prev);
            if (key2>key1)
            {
                return 1;
            }
        }
    }
    
    return 0;
}