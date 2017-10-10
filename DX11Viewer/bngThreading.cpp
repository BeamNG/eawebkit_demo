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

#include "bngThreading.h"
#include <sstream>
#include <windows.h> // Sleep


BeamNG::Threading::Win64Mutex::Win64Mutex() {
}

BeamNG::Threading::Win64Mutex::~Win64Mutex() {

}

void BeamNG::Threading::Win64Mutex::Lock() {
    m_mutex.lock();
}

bool BeamNG::Threading::Win64Mutex::TryLock() {
    return m_mutex.try_lock();
}

void BeamNG::Threading::Win64Mutex::Unlock() {
    m_mutex.unlock();
}

BeamNG::Threading::Win64ThreadCondition::Win64ThreadCondition() {
}

void BeamNG::Threading::Win64ThreadCondition::Wait(EA::WebKit::IMutex* _mutex) {
    Win64Mutex* mutex = dynamic_cast<Win64Mutex*>(_mutex);
    if (!mutex) return;
    std::unique_lock<std::mutex> m_lock(mutex->m_mutex);
    m_cond_var.wait(m_lock);
}

bool BeamNG::Threading::Win64ThreadCondition::TimedWait(EA::WebKit::IMutex* _mutex, double relativeTimeMS) {
    Win64Mutex* mutex = dynamic_cast<Win64Mutex*>(_mutex);
    if (!mutex) return false;
    std::unique_lock<std::mutex> m_lock(mutex->m_mutex);

    std::cv_status status = m_cond_var.wait_for(m_lock, std::chrono::milliseconds((long)relativeTimeMS));
    return status == std::cv_status::no_timeout; // what return means???
}

void BeamNG::Threading::Win64ThreadCondition::Signal(bool broadcast) {
    if (broadcast) {
        m_cond_var.notify_all();
    } else {
        m_cond_var.notify_one();
    }
}

EA::WebKit::ThreadId BeamNG::Threading::Win64Thread::Begin(EA::WebKit::ThreadFunc fn, void* pThreadContext, void* pUserData)
{
    //EAW_ASSERT_MSG(false, "Using a feature that requires thread creation. This is not supported.");
    m_thread = std::thread(fn, pThreadContext);

    std::stringstream ss;
    ss << m_thread.get_id();
    return std::stoul(ss.str());
}

void BeamNG::Threading::Win64Thread::WaitForEnd(intptr_t* result)
{
    m_thread.join();
}

void BeamNG::Threading::Win64Thread::SetName(const char* pName)
{
    // TODO
}

BeamNG::Threading::Win64ThreadLocalStorage::Win64ThreadLocalStorage() : m_value(0)
{

}
/*
void* BeamNG::Threading::Win64ThreadLocalStorage::GetValue()
{
    return m_value;
}

bool BeamNG::Threading::Win64ThreadLocalStorage::SetValue(void* pData)
{
    m_value = pData;
    return false;
}
*/


bool BeamNG::Threading::Win64ThreadSystem::Initialize()
{
    main_thread_id = std::this_thread::get_id();
    return true;
}

bool BeamNG::Threading::Win64ThreadSystem::Shutdown()
{
    return true;
}

EA::WebKit::IMutex* BeamNG::Threading::Win64ThreadSystem::CreateAMutex()
{
    return new Win64Mutex();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAMutex(EA::WebKit::IMutex* _mutex)
{
    BeamNG::Threading::Win64Mutex* mutex = (BeamNG::Threading::Win64Mutex*)_mutex;
    delete mutex;
}

EA::WebKit::IThreadCondition* BeamNG::Threading::Win64ThreadSystem::CreateAThreadCondition()
{
    return new Win64ThreadCondition();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThreadCondition(EA::WebKit::IThreadCondition* _threadCondition)
{
    BeamNG::Threading::Win64ThreadCondition* threadCondition = (BeamNG::Threading::Win64ThreadCondition*)_threadCondition;
    delete threadCondition;
}

EA::WebKit::IThreadLocalStorage* BeamNG::Threading::Win64ThreadSystem::CreateAThreadLocalStorage()
{
    return new Win64ThreadLocalStorage();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThreadLocalStorage(EA::WebKit::IThreadLocalStorage* _threadLocalStorage)
{
    BeamNG::Threading::Win64ThreadLocalStorage* threadLocalStorage = (BeamNG::Threading::Win64ThreadLocalStorage*)_threadLocalStorage;
    delete threadLocalStorage;
}

EA::WebKit::IThread* BeamNG::Threading::Win64ThreadSystem::CreateAThread()
{
    return new Win64Thread();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThread(EA::WebKit::IThread* _pThread)
{
    BeamNG::Threading::Win64Thread* pThread = (BeamNG::Threading::Win64Thread*)_pThread;
    delete pThread;
}

void BeamNG::Threading::Win64ThreadSystem::ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext)
{
    doWork(workContext);
}
EA::WebKit::ThreadId BeamNG::Threading::Win64ThreadSystem::CurrentThreadId()
{
    //06/03/2013 - We are taking advantage of this implementation in some places to detect if we have default thread system. So if the implementation ever changes, make sure to adjust the other code!
    //return EA::WebKit::kThreadIdInvalid;
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return std::stoul(ss.str());
}

bool BeamNG::Threading::Win64ThreadSystem::IsMainThread()
{
    return (std::this_thread::get_id() == main_thread_id);
}

void BeamNG::Threading::Win64ThreadSystem::YieldThread()
{
}

void BeamNG::Threading::Win64ThreadSystem::SleepThread(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds((long)ms));
}