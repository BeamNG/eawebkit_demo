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

BeamNG::Threading::Win64Mutex::Win64Mutex() {
}

BeamNG::Threading::Win64Mutex::~Win64Mutex() {

}

void BeamNG::Threading::Win64Mutex::Lock() {
    ++mLockCount;
}

bool BeamNG::Threading::Win64Mutex::TryLock() {
    if (mLockCount)
        return false;

    Lock();
    return true;
}

void BeamNG::Threading::Win64Mutex::Unlock() {
    --mLockCount;
}

void BeamNG::Threading::Win64ThreadCondition::Wait(EA::WebKit::IMutex* mutex)
{

}

bool BeamNG::Threading::Win64ThreadCondition::TimedWait(EA::WebKit::IMutex* mutex, double relativeTimeMS)
{
    return false;
}

void BeamNG::Threading::Win64ThreadCondition::Signal(bool broadcast)
{

}

EA::WebKit::ThreadId BeamNG::Threading::Win64Thread::Begin(EA::WebKit::ThreadFunc, void* pThreadContext, void* pUserData)
{
    //EAW_ASSERT_MSG(false, "Using a feature that requires thread creation. This is not supported.");
    return (EA::WebKit::ThreadId)0;
}

void BeamNG::Threading::Win64Thread::WaitForEnd(intptr_t* result)
{

}

void BeamNG::Threading::Win64Thread::SetName(const char* pName)
{

}

BeamNG::Threading::Win64ThreadLocalStorage::Win64ThreadLocalStorage() : m_value(0)
{

}

void* BeamNG::Threading::Win64ThreadLocalStorage::GetValue()
{
    return m_value;
}

bool BeamNG::Threading::Win64ThreadLocalStorage::SetValue(void* pData)
{
    m_value = pData;
    return false;
}


bool BeamNG::Threading::Win64ThreadSystem::Initialize()
{
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

void BeamNG::Threading::Win64ThreadSystem::DestroyAMutex(EA::WebKit::IMutex* mutex)
{
    delete mutex;
}

EA::WebKit::IThreadCondition* BeamNG::Threading::Win64ThreadSystem::CreateAThreadCondition()
{
    return new Win64ThreadCondition();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThreadCondition(EA::WebKit::IThreadCondition* threadCondition)
{
    delete threadCondition;
}

EA::WebKit::IThreadLocalStorage* BeamNG::Threading::Win64ThreadSystem::CreateAThreadLocalStorage()
{
    return new Win64ThreadLocalStorage();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThreadLocalStorage(EA::WebKit::IThreadLocalStorage* threadLocalStorage)
{
    delete threadLocalStorage;
}

EA::WebKit::IThread* BeamNG::Threading::Win64ThreadSystem::CreateAThread()
{
    return new Win64Thread();
}

void BeamNG::Threading::Win64ThreadSystem::DestroyAThread(EA::WebKit::IThread* pThread)
{
    delete pThread;
}

void BeamNG::Threading::Win64ThreadSystem::ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext)
{
    doWork(workContext);
}
EA::WebKit::ThreadId BeamNG::Threading::Win64ThreadSystem::CurrentThreadId()
{
    //06/03/2013 - We are taking advantage of this implementation in some places to detect if we have default thread system. So if the implementation ever changes, make sure to adjust the other code!
    return EA::WebKit::kThreadIdInvalid;
}

bool BeamNG::Threading::Win64ThreadSystem::IsMainThread()
{
    return true;
}

void BeamNG::Threading::Win64ThreadSystem::YieldThread()
{
}

void BeamNG::Threading::Win64ThreadSystem::SleepThread(uint32_t ms)
{
}