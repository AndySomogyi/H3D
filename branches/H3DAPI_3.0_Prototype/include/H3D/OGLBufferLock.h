#ifndef __OGLBUFFERLOCK_H__
#define __OGLBUFFERLOCK_H__

#include <vector>
#include <GL/glew.h>

namespace H3D
{
	// --------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------
	struct OGLBufferRange
	{
		OGLBufferRange(size_t _startOffset, size_t _length)
		: startOffset(_startOffset), length(_length)
		{}

		size_t startOffset;
		size_t length;

		bool Overlaps(const OGLBufferRange& _rhs) const 
		{
			return	startOffset < (_rhs.startOffset + _rhs.length) && 
							_rhs.startOffset < (startOffset + length);
		}
	};

	// --------------------------------------------------------------------------------------------------------------------
	struct OGLBufferLock
	{
		OGLBufferLock(OGLBufferRange _range, GLsync _syncObject)
		: range(_range), syncObject(_syncObject)
		{}


		OGLBufferRange range;
		GLsync syncObject;
	};

	// --------------------------------------------------------------------------------------------------------------------
	class OGLBufferLockManager
	{
	public:
		OGLBufferLockManager(bool _cpuUpdates);
		~OGLBufferLockManager();

		void WaitForLockedRange(size_t _lockBeginBytes, size_t _lockLength);
		void LockRange(size_t _lockBeginBytes, size_t _lockLength);

	private:
		void wait(GLsync* _syncObj);
		void cleanup(OGLBufferLock* _bufferLock);

		std::vector<OGLBufferLock> bufferLocks;

		// Whether it's the CPU (true) that updates, or the GPU (false)
		bool CPUUpdates;
	};
}

#endif