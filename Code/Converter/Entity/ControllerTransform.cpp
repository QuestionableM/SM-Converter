#include "ControllerTransform.hpp"

#include "Converter/Entity/Blueprint.hpp"
#include "Converter/Entity/Body.hpp"

#include "Utils/Console.hpp"

#include <algorithm>

SM_UNMANAGED_CODE

void ControllerTransform::Apply(
	SMBlueprint* pBlueprint,
	const std::vector<JointConnection>& bodyGraph,
	const std::unordered_map<std::size_t, std::size_t>& childToBodyMap)
{
	if (bodyGraph.empty()) return;

	// Root heuristic: score = (parentBody refs) - (childBody refs), highest wins
	std::unordered_map<std::size_t, int> v_rootScore;
	for (const auto& v_conn : bodyGraph)
	{
		v_rootScore[v_conn.parentBodyIdx]++;
		v_rootScore[v_conn.childBodyIdx]--;
	}

	std::unordered_map<std::size_t, std::size_t> v_entityCount;
	for (const auto& [v_entityIdx, v_bodyIdx] : childToBodyMap)
		v_entityCount[v_bodyIdx]++;

	std::size_t v_rootBody = bodyGraph[0].parentBodyIdx;
	bool v_foundRoot = false;
	int v_bestScore = 0;
	std::size_t v_bestEntityCount = 0;

	for (const auto& [v_bodyIdx, v_score] : v_rootScore)
	{
		const auto v_ecIt = v_entityCount.find(v_bodyIdx);
		const std::size_t v_entCount = (v_ecIt != v_entityCount.end()) ? v_ecIt->second : 0;

		if (!v_foundRoot
			|| v_score > v_bestScore
			|| (v_score == v_bestScore && v_entCount > v_bestEntityCount))
		{
			v_rootBody = v_bodyIdx;
			v_bestScore = v_score;
			v_bestEntityCount = v_entCount;
			v_foundRoot = true;
		}
	}

	struct Edge
	{
		std::size_t targetBody;
		glm::mat4 transform;
		bool hasPreset;
	};

	std::unordered_map<std::size_t, std::vector<Edge>> v_adjList;
	for (const auto& v_conn : bodyGraph)
	{
		v_adjList[v_conn.parentBodyIdx].push_back(
			{ v_conn.childBodyIdx, v_conn.localTransform, v_conn.hasPreset });
		v_adjList[v_conn.childBodyIdx].push_back(
			{ v_conn.parentBodyIdx, glm::inverse(v_conn.localTransform), v_conn.hasPreset });
	}

	// Preset edges first so BFS prefers joints that carry a transform
	for (auto& [v_bodyIdx, v_edges] : v_adjList)
	{
		std::stable_partition(v_edges.begin(), v_edges.end(),
			[](const Edge& e) { return e.hasPreset; });
	}

	// v_bodyTransforms doubles as visited set
	std::unordered_map<std::size_t, glm::mat4> v_bodyTransforms;
	v_bodyTransforms.reserve(v_rootScore.size());
	v_bodyTransforms[v_rootBody] = glm::mat4(1.0f);

	std::vector<std::size_t> v_queue;
	v_queue.reserve(v_rootScore.size());
	v_queue.push_back(v_rootBody);
	std::size_t v_queueIdx = 0;

	while (v_queueIdx < v_queue.size())
	{
		const std::size_t v_cur = v_queue[v_queueIdx++];
		const glm::mat4& v_curTransform = v_bodyTransforms[v_cur];

		const auto v_adjIt = v_adjList.find(v_cur);
		if (v_adjIt == v_adjList.end()) continue;

		for (const auto& v_edge : v_adjIt->second)
		{
			if (v_bodyTransforms.contains(v_edge.targetBody))
				continue;

			v_bodyTransforms[v_edge.targetBody] = v_curTransform * v_edge.transform;
			v_queue.push_back(v_edge.targetBody);
		}
	}

	v_bodyTransforms.erase(v_rootBody);

	if (v_bodyTransforms.empty()) return;

	for (SMEntity* v_pEntity : pBlueprint->m_objects)
	{
		if (v_pEntity->Type() == EntityType::Body)
		{
			SMBody* v_pBody = static_cast<SMBody*>(v_pEntity);
			v_pBody->ApplyPreTransformByBodyMap(v_bodyTransforms, childToBodyMap);
		}
		else
		{
			const std::size_t v_entityIdx = v_pEntity->GetIndex();
			const auto v_bodyIt = childToBodyMap.find(v_entityIdx);
			if (v_bodyIt == childToBodyMap.end()) continue;

			const auto v_transformIt = v_bodyTransforms.find(v_bodyIt->second);
			if (v_transformIt == v_bodyTransforms.end()) continue;

			v_pEntity->SetPreTransform(v_transformIt->second);
		}
	}
}
