#ifndef
#define __FLYWEIGHTMANAGER_H_

#include <vector>

namespace H3D {

template<typename ObjectType>
class FlyweightManager{
public:
	FlyweightManager();
	virtual ~FlyweightManager();



private:
	std::vector<ObjectType> objectContainer;
};

}
	
#endif