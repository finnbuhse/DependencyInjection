#include "Container.h"

ObjectReference InjectableInfo::getInstance(Container& container)
{
	return mGetInstanceFunc(container);
}

void InjectableInfo::destroyInstance(Container& container, ObjectReference pObject)
{
	return mDestroyInstanceFunc(container, pObject);
}
