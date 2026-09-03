
#include "TimeManager.h"

std::chrono::high_resolution_clock::time_point CTimeManager::mPrevTime;

float CTimeManager::mDeltaTime = 0.f;

uint64_t CTimeManager::mNextTimerID = 1;
std::unordered_map<uint64_t, FTimerInfo> CTimeManager::mTimerMap;


//델타 타임
//컴퓨터 성능에 관계없이 항상 일정한 속도로 게임이 실행되도록 시간을 만든다.
//이전 프레임과 현재 프레임을 이용해서 두개를 뺀값으로 사용된다.

bool CTimeManager::Init()
{
    mPrevTime = std::chrono::high_resolution_clock::now();
    mNextTimerID = 1;
    mTimerMap.clear();

    return true;
}

void CTimeManager::Update()
{
    auto CurrentTime = std::chrono::high_resolution_clock::now();

    //이전 시간과 현재시간의 차를 구해 진행된 시간을 계산한다.
    std::chrono::duration<float> Time = CurrentTime - mPrevTime;
    mDeltaTime = Time.count();

    mPrevTime = CurrentTime;


    //타이머를 갱신해준다.
    if (mTimerMap.empty())
    {
        return;
    }

    //타이머 순회도중 타이머가 사라졌을때
    //무효화 방지
    std::vector<uint64_t> CurrentIDs;
    for (const auto& It : mTimerMap)
    {
        CurrentIDs.push_back(It.first);
    }

    for (uint64_t ID : CurrentIDs)
    {
        auto iter = mTimerMap.find(ID);

        if (iter == mTimerMap.end())
        {
            continue;
        }

        FTimerInfo& Timer = iter->second;
        Timer.ElapsedTime += mDeltaTime;


        if (Timer.ElapsedTime >= Timer.Time)
        {
            //콜백 안에서 이 타이머 자신이 해제될 수 있다.
            //그러면 Timer 참조와 iter가 모두 무효화되므로
            //콜백 호출전에 필요한 값을 복사해둔다.
            const bool Loop = Timer.Loop;
            const float TargetTime = Timer.Time;

            if (Timer.Callback)
            {
                Timer.Callback();
            }

            //콜백이 자기 자신을 지웠는지 다시 확인한다.
            iter = mTimerMap.find(ID);

            if (iter == mTimerMap.end())
            {
                continue;
            }

            if (Loop)
            {
                iter->second.ElapsedTime -= TargetTime;
            }
            else
            {
                mTimerMap.erase(iter);
            }

        }

    }


}

void CTimeManager::ClearTimer(FTimerHandle& Handle)
{
    if (!Handle.IsValid())
    {
        return;
    }

    auto iter = mTimerMap.find(Handle.GetID());

    //핸들은 ID가 0인지만 보고 유효하다고 답하기 때문에
    //이미 만료되어 맵에서 사라진 타이머의 핸들도 여기까지 들어온다.
    //맵에 실제로 남아있는지 확인한 뒤에 접근해야한다.
    if (iter != mTimerMap.end())
    {
        iter->second.Callback = nullptr;

        mTimerMap.erase(iter);
    }

    Handle.InValidate();
}
