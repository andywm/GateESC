#pragma once
#include <utility>

template<typename TransitionEnum>
struct RudimentaryFSM;

template<typename TransitionEnum>
struct FSMState
{
	//static_assert(std::is_enum<TransitionEnum>::value, "Template type must be an enum");
	using ThisFSM = RudimentaryFSM<TransitionEnum>;
	ThisFSM* FSM = nullptr;

	virtual void OnEnter(){}
	virtual void OnProcess(){}
	virtual void OnLeave(){}
};

template<typename TransitionEnum>
struct RudimentaryFSM
{
	//static_assert(std::is_enum<TransitionEnum>::value, "Template type must be an enum");
	using IntType = int;// std::underlying_type(TransitionEnum);
	using State = FSMState<TransitionEnum>;
	State* States[static_cast<IntType>(TransitionEnum::MAX)];
	IntType CurrentState = -1;

	State*& operator[](TransitionEnum Enum) {return States[static_cast<IntType>(Enum)]; }
	
	template<typename Type, typename ...ArgsList>
	void Add(TransitionEnum Enum, ArgsList&&...Args)
	{
		Type* Inst = new Type;
		Inst->FSM = this;
		Inst->Init(std::forward<ArgsList&&>(Args)...);
		(*this)[Enum] = Inst;
	}
	
	void Transition(TransitionEnum NewState)
	{
		if (CurrentState != -1)
		{
			States[CurrentState]->OnLeave();
		}
		CurrentState = static_cast<IntType>(NewState);
		States[CurrentState]->OnEnter();
	}

	void Process()
	{
		if (CurrentState != -1)
		{
			States[CurrentState]->OnProcess();
		}
	}
};