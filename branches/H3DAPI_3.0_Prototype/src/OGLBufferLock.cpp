#include <H3D/OGLBufferLock.h>
#include <H3DUtil/Exception.h>
#include <sstream>

namespace H3D
{
	GLuint64 kOneSecondInNanoSeconds = 1000000000;

	// --------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------------------------------------------
	OGLBufferLockManager::OGLBufferLockManager(bool _cpuUpdates)
		: CPUUpdates(_cpuUpdates)
	{
	}

	// --------------------------------------------------------------------------------------------------------------------
	OGLBufferLockManager::~OGLBufferLockManager()
	{
		for (auto it = bufferLocks.begin(); it != bufferLocks.end(); ++it) 
		{
			cleanup(&*it);
		}

		bufferLocks.clear();
	}

	// --------------------------------------------------------------------------------------------------------------------
	void OGLBufferLockManager::WaitForLockedRange(size_t _lockBeginBytes, size_t _lockLength)
	{
		OGLBufferRange testRange(_lockBeginBytes, _lockLength);

		std::vector<OGLBufferLock> swapLocks;

		for(auto it = bufferLocks.begin(); it != bufferLocks.end(); ++it)
		{
			if(testRange.Overlaps(it->range)) 
			{
				wait(&it->syncObject);
				cleanup(&*it);
			} 
			else 
			{
				swapLocks.push_back(*it);
			}
		}

		bufferLocks.swap(swapLocks);
	}

	// --------------------------------------------------------------------------------------------------------------------
	void OGLBufferLockManager::LockRange(size_t _lockBeginBytes, size_t _lockLength)
	{
		OGLBufferRange newRange(_lockBeginBytes, _lockLength);
		GLsync syncName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		OGLBufferLock newLock(newRange, syncName);

		bufferLocks.push_back(newLock);
	}

	// --------------------------------------------------------------------------------------------------------------------
	void OGLBufferLockManager::wait(GLsync* _syncObj)
	{
		if(CPUUpdates)
		{
			GLbitfield waitFlags = 0;
			GLuint64 waitDuration = 0;
			while(1) 
			{
				GLenum waitRet = glClientWaitSync(*_syncObj, waitFlags, waitDuration);
				if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) 
				{
					return;
				}

				if (waitRet == GL_WAIT_FAILED) 
				{
					std::stringstream s;
					s << "OpenGL glClientWaitSync returned GL_WAIT_FAILED. Something probably went horribly wrong. Look in OGLBufferLock::wait";
					throw H3DUtil::Exception::H3DAPIException(s.str(), H3D_FULL_LOCATION);
					return;
				}

				// After the first time, need to start flushing, and wait for a looong time.
				waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
				waitDuration = kOneSecondInNanoSeconds;
			}
		} 
		else 
		{
			glWaitSync(*_syncObj, 0, GL_TIMEOUT_IGNORED);
		}
	}

	// --------------------------------------------------------------------------------------------------------------------
	void OGLBufferLockManager::cleanup(OGLBufferLock* _bufferLock)
	{
		glDeleteSync(_bufferLock->syncObject);
	}
}