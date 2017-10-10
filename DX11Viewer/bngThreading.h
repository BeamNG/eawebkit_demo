/*
The MIT License

Copyright (c) 2017 BeamNG GmbH. https://github.com/BeamNG/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include "EAWebKit\EAWebKitThreadInterface.h"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace BeamNG {

namespace Threading {

class Win64Mutex : public EA::WebKit::IMutex {
public:
	friend class DefaultThreadCondition;
    friend class Win64ThreadCondition;
    Win64Mutex();
    virtual ~Win64Mutex();
    void Lock();
	bool TryLock();
	void Unlock();

private:
    std::mutex m_mutex;
};

class Win64ThreadCondition : public EA::WebKit::IThreadCondition {
public:
    Win64ThreadCondition();
	void Wait(EA::WebKit::IMutex* mutex);
	bool TimedWait(EA::WebKit::IMutex* mutex, double relativeTimeMS);
	void Signal(bool broadcast);
private:
    std::condition_variable m_cond_var;
};


class Win64Thread : public EA::WebKit::IThread {
public:
	virtual EA::WebKit::ThreadId Begin(EA::WebKit::ThreadFunc,void* pThreadContext, void* pUserData);
	virtual void WaitForEnd(intptr_t* result);
	virtual void SetName(const char* pName);
protected:
    std::thread m_thread;
};

class Win64ThreadLocalStorage : public EA::WebKit::IThreadLocalStorage {
public:
    Win64ThreadLocalStorage();
    virtual void* GetValue() { return m_value; }
    bool SetValue(void* pData) { m_value = pData; return true; }
private:
    void* m_value;
};

class Win64ThreadSystem : public EA::WebKit::IThreadSystem {
public:
	virtual bool Initialize();
	virtual bool Shutdown();
	virtual EA::WebKit::IMutex* CreateAMutex();
	virtual void DestroyAMutex(EA::WebKit::IMutex*);
	virtual EA::WebKit::IThreadCondition* CreateAThreadCondition();
	virtual void DestroyAThreadCondition(EA::WebKit::IThreadCondition*);
	virtual EA::WebKit::IThreadLocalStorage* CreateAThreadLocalStorage();
	virtual void DestroyAThreadLocalStorage(EA::WebKit::IThreadLocalStorage*);
	virtual EA::WebKit::IThread* CreateAThread();
	virtual void DestroyAThread(EA::WebKit::IThread*);
	virtual void ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext);
	virtual EA::WebKit::ThreadId CurrentThreadId();
	virtual bool IsMainThread();
	virtual void YieldThread();
	virtual void SleepThread(uint32_t ms);
protected:
    std::thread::id main_thread_id;
};


} // namespace Threading
}  // namespace BamNG
