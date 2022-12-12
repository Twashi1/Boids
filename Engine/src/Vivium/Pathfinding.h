#pragma once

#include "Core.h"
#include "Vector2.h"
#include "Logger.h"

namespace Vivium {
	class VIVIUM_API Pathfinding {
	public:
		struct VIVIUM_API Node {
			Vector2<int> pos;
			Vector2<int> parent_pos = INT_MAX;

			float g_cost = FLT_MAX;
			float h_cost = FLT_MAX;
			float f_cost = FLT_MAX;

			Node() = default;

			Node(const Vector2<int>& pos);

			Node(const Node& other) = default;
			Node(Node&& other) noexcept = default;

			Node& operator=(const Node& other) = default;
			Node& operator=(Node&& other) noexcept = default;

			friend bool operator<(const Node& a, const Node& b) {
				return a.f_cost < b.f_cost;
			}
		};

		// Map of obstacles in the world, where true represents an obstacle, and false a walkable area
		class VIVIUM_API Map {
		private:
			// TODO: some bit-packing optimisation for storing the obstacle map
			std::shared_ptr<bool[]> m_Data = nullptr;
			Vector2<int> m_Dim = 0;

		public:
			Map() {}
			Map(std::shared_ptr<bool[]> data, const Vector2<int>& dim);

			void UpdateMap(bool* new_data);

			Vector2<int> GetDim() const;
			std::size_t Area() const;
			std::size_t ToIndex(const Vector2<int>& pos) const;

			bool IsObstacle(const Vector2<int>& pos) const;
			bool IsWalkable(const Vector2<int>& pos) const;
			bool IsWithinDim(const Vector2<int>& pos) const;

			Map(const Map& other);

			~Map();

			friend Pathfinding;
		};

		class VIVIUM_API Path {
		private:
			std::vector<Node> m_Data;

		public:
			std::vector<Node> GetNodes() const;
			std::vector<Vector2<int>> GetPositions() const;
			std::vector<Vector2<int>> GetDirectionVectors() const;

			Path() = default;

			Path(const Vector2<int>& end, Node* node_map, const Map& obstacle_map);
			Path(const std::vector<Node>& data);
			Path(std::vector<Node>&& data);

			Path(const Path& other) = default;
			Path(Path&& other) noexcept = default;

			Path& operator=(const Path& other) = default;
			Path& operator=(Path&& other) = default;

			friend Pathfinding;
		};

	public:
		static Path Calculate(const Vector2<int>& start, const Vector2<int>& end, const Map& map);
	};
}
