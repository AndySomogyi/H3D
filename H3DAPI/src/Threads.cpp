//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file Threads.cpp
/// \brief cpp file for thread handling functions.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif

#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;
#include "Threads.h"
#include <errno.h>

#ifdef HAVE_OPENHAPTICS
#include <HD/hd.h>
#endif

#ifdef MACOSX
#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#endif



using namespace H3D;
using namespace std;

/// Constructor.
MutexLock::MutexLock() {
  pthread_mutex_init( &mutex, NULL );
}
    
/// Destructor.
MutexLock::~MutexLock() {
  pthread_mutex_destroy( &mutex );
}

/// Locks the mutex.
void MutexLock::lock() {
  pthread_mutex_lock( &mutex );
}

/// Unlocks the mutex.
void MutexLock::unlock() {
  pthread_mutex_unlock( &mutex );
}

/// Try to lock the mutex, if the lock is not available false is returned.
bool MutexLock::tryLock() {
  return pthread_mutex_trylock( &mutex ) != EBUSY;
}


/// Constructor.
ConditionLock::ConditionLock() {
  pthread_cond_init( &cond, NULL );
}
    
/// Destructor.
ConditionLock::~ConditionLock() {
  pthread_cond_destroy( &cond );
}

/// Wait for the conditional to get a signal.
void ConditionLock::wait() {
  pthread_cond_wait( &cond, &mutex );
}

/// Wait for the conditional to get a signal, but only wait a
/// certain time. If the time exceeds the specified time false
/// is returned. If signal is received true is returned.
bool ConditionLock::timedWait( const struct timespec *abstime) {
  return pthread_cond_timedwait( &cond, &mutex, abstime ) != ETIMEDOUT;
}

/// Wakes up at least one thread blocked on this condition lock.
void ConditionLock::signal() {
  pthread_cond_signal( &cond ); 
}

/// This wakes up all of the threads blocked on the condition lock.
void ConditionLock::broadcast() {
  pthread_cond_broadcast( &cond );
}

void *Thread::thread_func( void * _data ) {
  Thread *thread = static_cast< Thread * >( _data );

#ifdef MACOSX
  // set thread priority
  struct thread_time_constraint_policy ttcpolicy;
  int ret;
  ttcpolicy.period=      100000;
  ttcpolicy.computation=  20000;
  ttcpolicy.constraint=  100000;
  ttcpolicy.preemptible=  1;
  if ((ret=thread_policy_set( mach_thread_self(),
                              THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy,
                              THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS) {
    cerr << "set_realtime() failed" << endl;
    return 0;
  }
#endif
  
#ifdef WIN32
  HANDLE hTimer = NULL;
  LARGE_INTEGER liDueTime;
  // add some time to compensate for the time spend between waking up
  // and setting the new timer.
  liDueTime.QuadPart=(LONGLONG)-(1e7 / thread->frequency ) + 400;
  if( thread->frequency > 0 ) {

    // set the multimedia timer frequency to 1 ms
    timeBeginPeriod(1); 
    
    // Create a waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
    if (!hTimer) {
      cerr << "CreateWaitableTimer failed (%d)" << endl 
           << GetLastError() << endl;
      timeEndPeriod(1); 
      return NULL;
    }
  
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
      cerr << "SetWaitableTimer failed (%d)\n" << GetLastError() << endl;
      timeEndPeriod(1); 
      return NULL;
    }
  }

#endif

  while( true ) {
    if( thread->frequency > 0 ) {
#ifdef WIN32
      if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
        printf("WaitForSingleObject failed (%d)\n", GetLastError());

      // Set a timer to wait for.
      if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
        cerr << "SetWaitableTimer failed (%d)\n" << GetLastError() << endl;
        return NULL;
      }
#endif
    }

    thread->callback_lock.lock();
    vector< Thread::CallbackList::iterator > to_remove( 30 );
    to_remove.clear();
    for( Thread::CallbackList::iterator i = thread->callbacks.begin();
         i != thread->callbacks.end(); i++ ) {
      Thread::CallbackCode c = (*i).first( (*i).second );
      if( c == Thread::CALLBACK_DONE ) {
        to_remove.push_back( i );
      }
    }
    
    // remove all callbacks that returned CALLBACK_DONE.
    for( vector< Thread::CallbackList::iterator >::iterator i = 
           to_remove.begin();
         i != to_remove.end(); i++ ) {
      thread->callbacks.erase( *i );
    }
  
    thread->callback_lock.signal();
    thread->callback_lock.unlock();
  }
} 

ThreadBase::ThreadId ThreadBase::main_thread_id = ThreadBase::getCurrentThreadId();
ConditionLock HapticThreadBase::haptic_lock; 
ConditionLock HapticThreadBase::sg_lock; 
int HapticThreadBase::haptic_threads_left;
std::vector< HapticThreadBase * > HapticThreadBase::threads;
HLThread *HLThread::singleton = new HLThread;

Thread::Thread( int _thread_priority,
                int _thread_frequency ):
  priority( _thread_priority ),
  frequency( _thread_frequency ) {
  pthread_attr_t attr;
  sched_param p;
  p.sched_priority = priority;
  pthread_attr_init( &attr );
  pthread_attr_setschedparam( &attr, &p );
  pthread_create( &thread_id, &attr, thread_func, this ); 
}

Thread::~Thread() {
  // TODO: what signal? 
  // synch callback to exit?
//  pthread_kill( 0 );
}


void Thread::synchronousCallback( CallbackFunc func, void *data ) {
  callback_lock.lock();
  callbacks.push_back( make_pair( func, data ) );
  callback_lock.wait();
  callback_lock.unlock();
}

void Thread::asynchronousCallback( CallbackFunc func, void *data ) {
  callback_lock.lock();
  callbacks.push_back( make_pair( func, data ) );
  callback_lock.unlock();
}

#ifdef HAVE_OPENHAPTICS
HDCallbackCode HDCALLBACK hdCallback( void *_data ) {
  void * * data = static_cast< void * * >( _data );
  HLThread::CallbackFunc* func = static_cast< HLThread::CallbackFunc* >( _data );
  //HLThread::CallbackFunc func = static_cast< HLThread::CallbackFunc >( data[0] );
  HLThread::CallbackCode c = (*func)( data[1] );
  if( c == HLThread::CALLBACK_DONE ) return HD_CALLBACK_DONE;
  else if( c == HLThread::CALLBACK_CONTINUE ) return HD_CALLBACK_CONTINUE;
  else return c;
}
#endif

void HLThread::synchronousCallback( CallbackFunc func, void *data ) {
#ifdef HAVE_OPENHAPTICS
  void * param[] = { (void*)func, data };
  hdScheduleSynchronous( hdCallback,
                         param,
                         HD_DEFAULT_SCHEDULER_PRIORITY );
#endif
}

void HLThread::asynchronousCallback( CallbackFunc func, void *data ) {
#ifdef HAVE_OPENHAPTICS
  void * * param = new void * [2];
  param[0] = (void*)func;
  param[1] = data;
  hdScheduleAsynchronous( hdCallback,
                          param,
                          HD_DEFAULT_SCHEDULER_PRIORITY );
#endif
}

Thread::CallbackCode HLThread::setThreadId( void * data ) {
  HLThread *thread = static_cast< HLThread * >( data );
  thread->thread_id = Thread::getCurrentThreadId();
  return Thread::CALLBACK_DONE;
}

void HLThread::setActive( bool _active ) { 
  if( _active && !is_active ) {
    sg_lock.lock(); 
    threads.push_back( this );
    sg_lock.unlock();
    synchronousCallback( setThreadId, this );
  } else if( !_active && is_active ) {
    sg_lock.lock();
    vector< HapticThreadBase *>::iterator i = 
      std::find( threads.begin(), 
                 threads.end(), 
                 this );
    if( i != threads.end() ) {
      threads.erase( i );
    }
    sg_lock.unlock();
  }
  is_active = _active; 
}

HapticThreadBase::HapticThreadBase() {
  sg_lock.lock();
  threads.push_back( this );
  sg_lock.unlock();

}

HapticThreadBase::~HapticThreadBase() {
  sg_lock.lock();
	vector< HapticThreadBase *>::iterator i = 
    std::find( threads.begin(), 
               threads.end(), 
               this );
  if( i != threads.end() ) {
    threads.erase( i );
  }
  sg_lock.unlock();
}


bool HapticThreadBase::inHapticThread() {
	Thread::ThreadId id = Thread::getCurrentThreadId();
  for( vector< HapticThreadBase *>::iterator i = threads.begin();
       i != threads.end(); i++ ) {
    ThreadBase *thread = dynamic_cast< ThreadBase * >( *i );
    if( thread && pthread_equal( thread->getThreadId(), id ) )
      return true;
  }
  return false;
}

ThreadBase::CallbackCode HapticThreadBase::sync_haptics( void * ) {
  sg_lock.lock();
  haptic_threads_left--;
  if( haptic_threads_left == 0 ) {
    sg_lock.signal();
    sg_lock.wait();
    sg_lock.unlock();
    haptic_lock.lock();
    haptic_lock.broadcast();
    haptic_lock.unlock();
  } else {
    sg_lock.unlock();
    haptic_lock.lock();
    haptic_lock.wait();
    haptic_lock.unlock();
  }
  
  return ThreadBase::CALLBACK_DONE;
}

void HapticThreadBase::synchronousHapticCB( Thread::CallbackFunc func, 
                                            void *data ) {
  sg_lock.lock(); 
  haptic_threads_left = threads.size();
  if( haptic_threads_left > 0 ) {
    for( vector< HapticThreadBase *>::iterator i = threads.begin();
         i != threads.end(); i++ ) {
      ThreadBase *thread = dynamic_cast< ThreadBase * >( *i );
      if( thread )
        thread->asynchronousCallback( sync_haptics, NULL );
    }
    sg_lock.wait();
    func( data );
    sg_lock.signal();

  } else {
    func( data );
  }
  sg_lock.unlock();
  sched_yield();
}

ThreadBase::ThreadId ThreadBase::getCurrentThreadId() {
  return pthread_self();
} 

/// Returns true if the call was made from the main thread.
bool ThreadBase::inMainThread() {
  return pthread_equal( main_thread_id, getCurrentThreadId() ) != 0;
} 















