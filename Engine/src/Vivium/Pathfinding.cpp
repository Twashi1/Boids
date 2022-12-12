#include "Pathfinding.h"

namespace Vivium {
	Pathfinding::Path Pathfinding::Calculate(const Vector2<int>& start, const Vector2<int>& end, const Map& map)
	{
		if (map.IsObstacle(end) || map.IsObstacle(start)) { return Path(); }
		if (start == end) { return Path(); }

		Node* node_map = new Node[map.Area()];
		Vector2<int> dim = map.GetDim();

		for (std::size_t i = 0; i < map.Area(); i++) {
			int y = i / dim.x;
			int x = i - (y * dim.x);

			node_map[i] = Node({ x, y });
		}

		// All nodes that have already been checked
		// TODO: bit-packing optimisation
		bool* closed_nodes = new bool[map.Area()];

		std::fill(closed_nodes, closed_nodes + map.Area(), false);

		// List of open nodes (nodes where not all paths have been checked)
		std::vector<Node> open_nodes;
		open_nodes.reserve(map.Area());

		// Create starting node and add to open nodes
		Node& start_node = node_map[map.ToIndex(start)];
		start_node.f_cost = 0.0f;
		start_node.g_cost = 0.0f;
		start_node.h_cost = 0.0f;
		start_node.parent_pos = start;

		open_nodes.push_back(start_node);
		
		bool found_dest = false;

		while ((!open_nodes.empty()) && (open_nodes.size() < map.Area())) {
			Node parent;

			do {
				float lowest_fcost = FLT_MAX;

				std::vector<Node>::iterator closest_it;

				for (std::vector<Node>::iterator it = open_nodes.begin(); it != open_nodes.end(); it = std::next(it)) {
					Node& current = *it;

					if (current.f_cost < lowest_fcost) {
						lowest_fcost = current.f_cost;
						closest_it = it;
					}
				}

				// This node has been investigated, move to next node
				parent = *closest_it;
				open_nodes.erase(closest_it);
			} while (map.IsObstacle(parent.pos) && !open_nodes.empty());

			Vector2<int> pos = parent.pos;
			closed_nodes[map.ToIndex(pos)] = true;

			for (int offset_y = -1; offset_y <= 1; offset_y++) {
				for (int offset_x = -1; offset_x <= 1; offset_x++) {
					// If we're checking a diagonal
					if (std::abs(offset_x) == std::abs(offset_y) && offset_y != 0) {
						// Check that no obstacles blocking diag path
						/*
						Going diagonal here is valid
						00 \
						00  \

						Going diagonal here is **not** valid (take the long path around)
						01 \
						10  \

						Going diagonal here is also **not** valid
						00 \	(instead takes this path) -¬
						10  \							   ¦
						*/

						// If either is an obstacle
						if (map.IsObstacle(parent.pos + Vector2<int>(offset_x, 0)) || map.IsObstacle(parent.pos + Vector2<int>(0, offset_y))) {
							// Stop considering this node, move to the next
							continue;
						}
					}

					Vector2<int> offset_vector(offset_x, offset_y);
					Vector2<int> look_pos = parent.pos + offset_vector;
					std::size_t look_idx = map.ToIndex(look_pos);

					if (map.IsWalkable(look_pos)) {
						// Found destination
						if (look_pos == end) {
							found_dest = true;
							node_map[look_idx].parent_pos = pos;

							auto path = Path(end, node_map, map);

							delete[] closed_nodes;
							delete[] node_map;
							
							return path;
						}
						else if (!closed_nodes[look_idx]) {
							float gnew = parent.g_cost + 1.0f;
							float hnew = Vector2<float>::Distance(look_pos, end);
							float fnew = gnew + hnew;

							Node& looking_node = node_map[look_idx];

							if (looking_node.f_cost > fnew) {
								looking_node.g_cost = gnew;
								looking_node.h_cost = hnew;
								looking_node.f_cost = fnew;

								looking_node.parent_pos = pos;
								open_nodes.push_back(looking_node);
							}
						}
					}
				}
			}
		}

		if (!found_dest) {
			LogTrace("Couldn't find destination pathfinding!");

			delete[] closed_nodes;
			delete[] node_map;

			return Path();
		}
	}
	
	std::vector<Pathfinding::Node> Pathfinding::Path::GetNodes() const { return m_Data; }
	
	std::vector<Vector2<int>> Pathfinding::Path::GetPositions() const
	{
		if (m_Data.empty()) return {};

		std::vector<Vector2<int>> positions(m_Data.size());

		for (std::size_t i = 0; i < m_Data.size(); i++) {
			positions[i] = m_Data[i].pos;
		}

		return positions;
	}
	
	std::vector<Vector2<int>> Pathfinding::Path::GetDirectionVectors() const
	{
		if (m_Data.empty()) return {};
		if (m_Data.size() == 1) return {0}; // Return no direction

		std::vector<Vector2<int>> directions(m_Data.size() - 1);

		for (std::size_t i = 0; i < m_Data.size() - 1; i++) {
			directions[i] = m_Data[i + 1].pos - m_Data[i].pos;
		}

		return directions;
	}

	Pathfinding::Path::Path(const Vector2<int>& end, Node* node_map, const Map& map)
	{
		std::vector<Node> reverse_path;

		Vector2<int> pos = end;
		std::size_t pos_idx = map.ToIndex(pos);

		while (node_map[pos_idx].parent_pos != pos) {
			reverse_path.push_back(node_map[pos_idx]);

			pos = node_map[pos_idx].parent_pos;
			pos_idx = map.ToIndex(pos);
		}

		reverse_path.push_back(node_map[pos_idx]);

		m_Data.resize(reverse_path.size());
		std::reverse_copy(reverse_path.begin(), reverse_path.end(), m_Data.begin());
	}

	Pathfinding::Path::Path(const std::vector<Node>& data)
		: m_Data(data)
	{}
	
	Pathfinding::Path::Path(std::vector<Node>&& data)
		: m_Data(std::move(data))
	{}
	
	Pathfinding::Map::Map(std::shared_ptr<bool[]> data, const Vector2<int>& dim)
		: m_Dim(dim), m_Data(data)
	{}

	void Pathfinding::Map::UpdateMap(bool* new_data)
	{
		for (std::size_t i = 0; i < m_Dim.x * m_Dim.y; i++) {
			m_Data.get()[i] = new_data[i];
		}
	}

	Vector2<int> Pathfinding::Map::GetDim() const
	{
		return m_Dim;
	}

	std::size_t Pathfinding::Map::Area() const
	{
		return m_Dim.x * m_Dim.y;
	}

	std::size_t Pathfinding::Map::ToIndex(const Vector2<int>& pos) const
	{
		return pos.x + pos.y * m_Dim.x;
	}

	bool Pathfinding::Map::IsObstacle(const Vector2<int>& pos) const
	{
		if (IsWithinDim(pos)) {
			return m_Data.get()[pos.x + pos.y * m_Dim.x];
		}

		return true;
	}

	bool Pathfinding::Map::IsWalkable(const Vector2<int>& pos) const
	{
		if (IsWithinDim(pos)) {
			return !m_Data.get()[pos.x + pos.y * m_Dim.x];
		}

		return false;
	}

	bool Pathfinding::Map::IsWithinDim(const Vector2<int>& pos) const
	{
		return pos.x >= 0 && pos.x < m_Dim.x && pos.y >= 0 && pos.y < m_Dim.y;
	}

	Pathfinding::Map::Map(const Map& other)
		: m_Dim(other.m_Dim), m_Data(other.m_Data)
	{}
	
	Pathfinding::Map::~Map() {}

	Pathfinding::Node::Node(const Vector2<int>& pos)
		: pos(pos)
	{}
}
