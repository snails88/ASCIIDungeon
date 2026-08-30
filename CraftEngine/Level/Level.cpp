#include "Level.h"

namespace Craft
{
	Level::Level()
	{}
	Level::~Level()
	{}

	void Level::OnInitialized()
	{
		hasInitialized = true;
	}

	void Level::BeginPlay()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (actor->HasBeganPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (actor && !actor->IsActive())
			{
				continue;
			}

			actor->Tick(deltaTime);
		}
	}

	void Level::Draw()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			actor->Draw();
		}
	}

	void Level::ProcessAddandDestroyActors()
	{
		for (auto iter = actorList.begin(); iter != actorList.end(); )
		{
			// 제거 요청된 액터인지 확인
			auto actor = *iter;
			if (actor->HasExpired())
			{
				iter = actorList.erase(iter);
				continue;
			}

			++iter;
		}

		if (addRequestedActorList.empty())
		{
			return;
		}

		for (const auto& actor : addRequestedActorList)
		{
			actorList.emplace_back(actor);
		}

		addRequestedActorList.clear();
	}

	void Level::SavePreviousActorStates()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor->IsActive())
				continue;

			actor->SavePreviousState();
		}
	}
}