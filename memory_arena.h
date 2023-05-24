#ifndef ARENA_H
#define ARENA_H

#include "ngcommon.h"

#define Gigabyte(N) ((N) * 1024 * 1024 * 1024)
#define Megabyte(N) ((N) * 1024 * 1024)
#define Kilobyte(N) ((N) * 1024)

#define AlignUpPow2(Pos, Alignment) ( ( (Pos) + ( (Alignment) - 1) ) & ~( (Alignment) - 1) )
#define AlignDownPow2(Pos, Alignment) ( (Pos) & ~ ( (Alignment) - 1) )

#ifdef _M_X64
	#define ARENA_DATA_ALIGNMENT 8
#else
	#define ARENA_DATA_ALIGNMENT 4
#endif

class memory_arena {
	LPVOID m_Memory;
	UINT64 m_Size;
	UINT64 m_Pos;

public:
	memory_arena();
	~memory_arena();

public:
	bool Reserve(UINT64 Size);
	void Release();

	LPVOID Push(UINT64 Size);
	void PopAmount(UINT64 Amount);
	void PopTo(UINT64 Pos);

	bool HasSpace(UINT64 Size) const;
};

#endif