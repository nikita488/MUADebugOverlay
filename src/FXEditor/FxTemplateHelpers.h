#pragma once

#include <Misc/IMemory.h>

#include <FX/CFxTemplate.h>
#include <FX/CFxPrimitiveTemplate.h>

const char* GetPrimitiveName(EFXPrimType primType);

const char* GetPrimitiveDisplayName(EFXPrimType primType);

bool FreeFxTemplate(CFxTemplate* fxTemplate, EMemoryPoolID poolId);

bool FreeFxTemplate(const char* resSpec);

bool CanAllocFxTemplate();

bool CanAllocFxPrimitiveTemplate();

void FreeFxPrimitiveTemplate(CFxPrimitiveTemplate* fxPrimitive);

int CloneVector(int handle);

int CloneQuad(int handle);

void CloneRangedComponents(CFxPrimitiveTemplate* primitive, bool free = true);
