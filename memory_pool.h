#ifndef POOL_H
#define POOL_H

#include "memory_arena.h"
#include "ngcommon.h"

class memory_pool {

	enum SlotListIndex: UINT32 {
		FREE = 0,
		USED = 1,
	};

	struct Slot {
		LPVOID Memory;
		UINT64 Size;
		Slot* Next;
	};

private:
	Slot* SlotLists[2];
	UINT64 SlotListCounts[2];

public:
	memory_pool();
	~memory_pool();

private:
	void PushSlot(SlotListIndex SlotIndex, Slot* SlotToPush);
	Slot* PopSlot(SlotListIndex SlotIndex);

public:
	BOOL Init(memory_arena* Arena, UINT64 SizePerSlot, UINT64 SlotCount);

	LPVOID New();
	void Release(LPVOID Memory);
};


#endif

