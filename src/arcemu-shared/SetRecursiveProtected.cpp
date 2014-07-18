#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SetRecursiveProtected.h"

RecursiveExitAutoCallback::RecursiveExitAutoCallback( )
{
	CallbackOwner = 0;
}

RecursiveExitAutoCallback::RecursiveExitAutoCallback( SampleObject *Obj )
{
	CallbackOwner = Obj;
	CallbackOwner->RecursionAutoLockAquire();
}
RecursiveExitAutoCallback::~RecursiveExitAutoCallback()
{
	CallbackOwner->RecursionAutoLockRelease();
}