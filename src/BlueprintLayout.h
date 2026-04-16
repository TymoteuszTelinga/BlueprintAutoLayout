#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

struct Node
{
    uint32_t ID;
    std::string Name;
    float PosX = 0.f;
    float PosY = 0.f;
};

struct Edge
{
    uint32_t From;
    uint32_t To;
};

class BlueprintAutoLayout
{
public:
    BlueprintAutoLayout() {};
    ~BlueprintAutoLayout() {};

    bool ImportGraph(const std::filesystem::path& importpath);
    void CreateLayout();
    void ExportGraph(const std::filesystem::path& exportPath);

    void GenerateData();

    const std::unordered_map<uint32_t, Node>& GetNodes() const { return m_Nodes; };
    const std::vector<Edge>& GetEdges() const { return m_Edges; };

private:

	std::unordered_map<uint32_t, Node> m_Nodes;
	std::vector<Edge> m_Edges;
};