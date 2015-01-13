#ifndef __FLYWEIGHTMANAGER_H__
#define __FLYWEIGHTMANAGER_H__

#include <utility>
#include <vector>
#include <string>
#include <set>

namespace H3D {
	//Short for FlyweightHandle
	typedef unsigned int FWh;
	typedef std::pair<std::string, FWh> nameHandlePair;
	typedef std::iterator<std::string, FWh> setIterator;

	template<typename ObjectType>
	class FlyweightManager{
	public:
		FlyweightManager() {
			objectContainer.clear();
			pairings.clear();
		}

		virtual ~FlyweightManager() {
			objectContainer.clear();
			pairings.clear();
		}

	
	protected:
		//If we find a duplicate, the function will return true and the handle will be filled with the found value.
		//Else it'll return false and handle won't be written to.
		bool lookForDuplicate(std::string objectURL, FWh& handle) {
			setIterator it = pairings.find(objectURL);

			if(it != pairings.end()) {
				handle = (*it)->second;
				return true;
			}
			
			return false;
		}

		void insertNewPair(std::string url, FWh handle) {
			pairings.insert(createPair(url, handle));
		}

		nameHandlePair createPair(std::string url, FWh handle) {
			return nameHandlePair(url, handle);
		}

	private:
		std::vector<ObjectType> objectContainer;
		std::set<std::string, FWh> pairings;
	};
}
#endif