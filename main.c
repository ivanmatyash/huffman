#include "heap.h"

int main(void)
{
	heapNode node;
 	heap *t;
	t = create_heap(10);
	insert_node_heap(t, 'a', 98);
	insert_node_heap(t, 199, 'b');
	insert_node_heap(t, 200, 'b');
	
	int i = 0, amount = t->curSize;
	heapNode tempNode;
	for (i = 0; i < amount; i++)
	{
		tempNode = remove_min_node_heap(t);
		printf("%d %d\n", tempNode.priority, tempNode.value);
	}	

	delete_heap(t);
}
