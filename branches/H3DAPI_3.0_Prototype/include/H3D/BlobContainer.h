#ifndef __BLOBCONTAINER_H__
#define __BLOBCONTAINER_H__
#include <vector>

namespace H3D {
	//BLOB == Binary Large OBject. 
	//Ultra simple class for containing any sort of data.
	struct BlobContainer {

		//dataSize should be in bytes.
		BlobContainer(void* data = 0, unsigned int dataSize = 0) {
			if(data != 0 && dataSize != 0) {
				dataContainer.resize(dataSize);
				memcpy(&dataContainer[0], data, dataSize);
			} else {
				if(dataSize != 0) {		
					dataContainer.resize(dataSize);
				}
			}
			//Else we fail quietly...?
		}

		BlobContainer(BlobContainer& rhs) {
			unsigned int containerSize = rhs.dataContainer.size();
			dataContainer.resize(containerSize);
			memcpy(&dataContainer[0], &rhs.dataContainer[0], containerSize);
		}

	BlobContainer& operator=(BlobContainer& rhs) {
			unsigned int containerSize = rhs.dataContainer.size();
			dataContainer.resize(containerSize);
			memcpy(&dataContainer[0], &rhs.dataContainer[0], containerSize);
		}

		~BlobContainer() {
			dataContainer.clear();
		}

		void* getDataHeadPtr() {
			return &dataContainer[0];
		}

		//Returns size in bytes.
		unsigned int getSize() {
			return dataContainer.size();
		}

	private:
	std::vector<char> dataContainer;
	};
}
#endif