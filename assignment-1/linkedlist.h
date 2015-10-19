

typedef struct LinkedList {
	LinkedListNode head;
	int size;
} LinkedList;

typedef struct LinkedListNode {
	LinkedListNode* tail;
	int value;
} LinkedListNode;

LinkedList constructLinkedList(int initialValue) {
	LinkedListNode head = {
		NULL,
		0
	};
	LinkedList output = {
		head,
		1
	};

	return output;
}

LinkedListNode* getNodeAtIndex(LinkedList list, int index) {
	LinkedListNode* output = &list.head;
	int i;
	for (i = 0; i < index; i++) {
		if (i >= list.size || output->tail == NULL) {
			// We are at the end
			return output;
		} else {
			// Keep going
			output = output->tail;
		}
	}
	return output;
}
