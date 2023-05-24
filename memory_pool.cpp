#include "memory_pool.h"

memory_pool::memory_pool() {
	SlotLists[SlotListIndex::USED] = nullptr;
	SlotLists[SlotListIndex::FREE] = nullptr;
	SlotListCounts[SlotListIndex::USED] = 0;
	SlotListCounts[SlotListIndex::FREE] = 0;
}

memory_pool::~memory_pool() {
	SlotLists[SlotListIndex::FREE] = nullptr;
	SlotLists[SlotListIndex::USED] = nullptr;
	SlotListCounts[SlotListIndex::FREE] = 0;
	SlotListCounts[SlotListIndex::USED] = 0;
}

BOOL memory_pool::Init(memory_arena* Arena, UINT64 DataSize, UINT64 Count) {
	if(Arena && DataSize > 0 && Count > 0) {
		UINT64 TotalSlotSize = sizeof(Slot) * Count;
		UINT64 TotalDataSize = DataSize * Count;
		UINT64 TotalSize = TotalSlotSize + TotalDataSize;
		
		if(Arena->HasSpace(TotalSize)) {
			// get a pointer to our memory block
			LPVOID SlotMemory = Arena->Push(TotalSize);
			LPVOID Memory = (UINT8*) SlotMemory + TotalSlotSize;
			
			// init slot memory needed for the pool
			SlotLists[SlotListIndex::FREE] = (Slot*) SlotMemory;
			SlotLists[SlotListIndex::USED] = nullptr;
			
			SlotListCounts[SlotListIndex::FREE] = Count;
			SlotListCounts[SlotListIndex::USED] = 0;
			
			for(UINT64 I = 0; I < Count; I++) {
				Slot* FreeList = SlotLists[SlotListIndex::FREE];
				
				FreeList[I].Memory = (UINT*) Memory + I * DataSize;
				FreeList[I].Size = DataSize;
				
				bool HasNext = ((I + 1) < Count);
				if(HasNext)
					FreeList[I].Next = &FreeList[I + 1];
			}
			
			return true;
		}
	}
	
	return false;
}

void memory_pool::PushSlot(SlotListIndex SlotIndex, Slot* SlotToPush) {
	if(!SlotLists[SlotIndex])
		SlotLists[SlotIndex] = SlotToPush;
	else {
		SlotToPush->Next = SlotLists[SlotIndex];
		SlotLists[SlotIndex] = SlotToPush;
	}
	
	SlotListCounts[SlotIndex]++;
}

memory_pool::Slot* memory_pool::PopSlot(SlotListIndex SlotIndex) {
	Slot* SlotToPop = nullptr; 
	
	if(SlotToPop = SlotLists[SlotIndex]) {
		SlotLists[SlotIndex] = SlotToPop->Next ? SlotToPop->Next : nullptr;
		SlotListCounts[SlotIndex]--;
		SlotToPop->Next = nullptr;
	}
	
	return SlotToPop;
}

LPVOID memory_pool::New() {
	LPVOID UserData = nullptr;
	
	if(SlotListCounts[SlotListIndex::FREE] > 0) {
		Slot* FreeSlot = PopSlot(SlotListIndex::FREE);
		PushSlot(SlotListIndex::USED, FreeSlot);
		
		// save user data to be returned below
		UserData = FreeSlot->Memory;
	}
	
	return UserData;
}

void memory_pool::Release(LPVOID Memory) {
	if(Memory && SlotListCounts[SlotListIndex::USED] > 0) {
		Slot* PrevSlot = nullptr;
		Slot* NextSlot = SlotLists[SlotListIndex::USED];
		
		while(NextSlot) {
			if(Memory == NextSlot->Memory) {
				if(PrevSlot)
					PrevSlot->Next = NextSlot->Next;
				break;
			}
			
			PrevSlot = NextSlot;
			NextSlot = NextSlot->Next;
		}
		
		PushSlot(SlotListIndex::FREE, NextSlot);
	}
}

