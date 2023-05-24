#include "memory_arena.h"

memory_arena::memory_arena() {
	m_Memory = nullptr;
	m_Size = 0;
	m_Pos = 0;
}

memory_arena::~memory_arena() {
	Release();
}

void memory_arena::Release() {
	if(m_Memory) {
		VirtualFree(m_Memory, 0, MEM_RELEASE);
		m_Memory = nullptr;
	}
	
	m_Size = 0;
	m_Pos = 0;
}

bool memory_arena::Reserve(UINT64 Size) {
	bool Success = true;
	if(Size > 0 && !m_Memory) {
		const INT ALLOC_FLAGS = MEM_COMMIT | MEM_RESERVE;
		m_Memory = VirtualAlloc(0, Size, ALLOC_FLAGS, PAGE_READWRITE);
		
		if(!m_Memory)
			Success = false;
		else {
			m_Size = Size;
			m_Pos = 0;
		}
	}
	
	return Success;
}

LPVOID memory_arena::Push(UINT64 Size) {
	LPVOID Memory = nullptr;
	if(Size > 0) {
		if(m_Pos + Size < m_Size) {
			Memory = (UINT8*) m_Memory + m_Pos;
			UINT64 NextPos = m_Pos + Size;
			m_Pos = AlignUpPow2(NextPos, ARENA_DATA_ALIGNMENT);
		}
	}
	
	return Memory;
}

void memory_arena::PopAmount(UINT64 Amount) {
	if(Amount > 0 && Amount < m_Pos) {
		UINT64 NextPos = m_Pos - Amount;
		m_Pos = AlignDownPow2(NextPos, ARENA_DATA_ALIGNMENT);
	}
}

void memory_arena::PopTo(UINT64 Pos) {
	if(Pos < m_Pos)
		m_Pos = AlignDownPow2(Pos, ARENA_DATA_ALIGNMENT); 
}

bool memory_arena::HasSpace(UINT64 Size) const {
	return m_Size > Size;
}
